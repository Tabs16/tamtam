#include <python2.4/Python.h>

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <vector>
#include <map>
#include <cmath>

#include <csound/csound.h>
#include <alsa/asoundlib.h>

#define ACFG(cmd) {int err = 0; if ( (err = cmd) < 0) { fprintf(stderr, "ERROR: %s:%i (%s)\n", __FILE__, __LINE__, snd_strerror(err)); return err;} }
#define ERROR_HERE fprintf(stderr, "ERROR: %s:%i\n", __FILE__, __LINE__);


unsigned int SAMPLE_RATE = 16000;
snd_pcm_uframes_t PERIODS_PER_BUFFER = 4;
snd_pcm_uframes_t PERIOD_SIZE = (1<<8);

static int setparams (snd_pcm_t * phandle )
{
    snd_pcm_hw_params_t *hw;
    int srate_dir = 0;
    snd_pcm_uframes_t buffer_size = PERIOD_SIZE * PERIODS_PER_BUFFER, bsize, psize;

    ACFG(snd_pcm_hw_params_malloc(&hw));
    ACFG(snd_pcm_hw_params_any(phandle, hw));
    ACFG(snd_pcm_hw_params_set_access(phandle, hw, SND_PCM_ACCESS_RW_INTERLEAVED));
    ACFG(snd_pcm_hw_params_set_format(phandle, hw, SND_PCM_FORMAT_FLOAT));
    ACFG(snd_pcm_hw_params_set_rate_near(phandle, hw, &SAMPLE_RATE, &srate_dir));
    ACFG(snd_pcm_hw_params_set_channels(phandle, hw, 2));
    ACFG(snd_pcm_hw_params_set_buffer_size_near(phandle, hw, &buffer_size));
    ACFG(snd_pcm_hw_params_set_period_size_near(phandle, hw, &PERIOD_SIZE, 0));
    ACFG(snd_pcm_hw_params_get_buffer_size(hw, &bsize));
    ACFG(snd_pcm_hw_params_get_period_size(hw, &psize, 0));

    assert(bsize == buffer_size);
    assert(psize == PERIOD_SIZE);

    ACFG(snd_pcm_hw_params(phandle, hw));

    snd_pcm_hw_params_free (hw);
    return 0;
}
static int setswparams(snd_pcm_t *phandle)
{
    /* not sure what to do here */
    return 0;
}

static double pytime(const struct timeval * tv)
{
    return (double) tv->tv_sec + (double) tv->tv_usec / 1000000.0;
}

struct ev_t
{
    char type;
    int onset;
    bool time_in_ticks;
    MYFLT prev_secs_per_tick;
    MYFLT duration, attack, decay;
    std::vector<MYFLT> param;

    ev_t(char type, MYFLT * p, int np, bool in_ticks)
        : type(type), onset(0), time_in_ticks(in_ticks), param(np)
    {
        assert(np >= 4);
        onset = (int) p[1];
        duration = p[2];
        attack = np > 8 ? p[8]: 0.0; //attack
        decay = np > 9 ? p[9]: 0.0; //decay
        prev_secs_per_tick = -1.0;
        for (int i = 0; i < np; ++i) param[i] = p[i];

        param[1] = 0.0; //onset
    }
    bool operator<(const ev_t &e) const
    {
        return onset < e.onset;
    }
    void ev_print(FILE *f)
    {
        fprintf(f, "INFO: scoreEvent %c ", type);
        for (size_t i = 0; i < param.size(); ++i) fprintf(f, "%lf ", param[i]);
        fprintf(f, "\n");
    }
    void update(int idx, MYFLT val)
    {
        if ( (unsigned)idx >= param.size())
        {
            fprintf(stderr, "ERROR: updateEvent request for too-high parameter %i\n", idx);
            return;
        }
        if (time_in_ticks)
        {
            switch(idx)
            {
                case 1: onset = (int) val; break;
                case 2: duration =    val; break;
                case 8: attack =      val; break;
                case 9: decay  =      val; break;
                default: param[idx] = val; break;
            }
            prev_secs_per_tick = -1.0; //force recalculation
        }
        else
        {
            param[idx] = val;
        }
    }

    void event(CSOUND * csound, MYFLT secs_per_tick)
    {
        if (time_in_ticks && (secs_per_tick != prev_secs_per_tick))
        {
            param[2] = duration * secs_per_tick;
            param[8] = std::max(0.002f, attack * secs_per_tick);
            param[9] = std::max(0.002f, decay * secs_per_tick);
            prev_secs_per_tick = secs_per_tick;
            if (0) fprintf(stdout, "setting duration to %f\n", param[5]);
        }
        csoundScoreEvent(csound, type, &param[0], param.size());
    }
};
struct EvLoop
{
    FILE * _debug;

    int tick_prev;
    int tickMax;
    MYFLT rtick;
    MYFLT secs_per_tick;
    MYFLT ticks_per_step;
    typedef std::pair<int, ev_t *> pair_t;
    typedef std::multimap<int, ev_t *>::iterator iter_t;
    typedef std::map<int, iter_t>::iterator idmap_t;

    std::multimap<int, ev_t *> ev;
    std::multimap<int, ev_t *>::iterator ev_pos;
    std::map<int, iter_t> idmap;
    CSOUND * csound;
    void * mutex;

    EvLoop(CSOUND * cs) : _debug(NULL), tick_prev(0), tickMax(1), rtick(0.0), ev(), ev_pos(ev.end()), csound(cs), mutex(NULL)
    {
        setTickDuration(0.05);
        mutex = csoundCreateMutex(0);
    }
    ~EvLoop()
    {
        csoundLockMutex(mutex);
        for (iter_t i = ev.begin(); i != ev.end(); ++i)
        {
            delete i->second;
        }
        csoundUnlockMutex(mutex);
        csoundDestroyMutex(mutex);
    }
    void clear()
    {
        csoundLockMutex(mutex);
        for (iter_t i = ev.begin(); i != ev.end(); ++i)
        {
            delete i->second;
        }
        ev.erase(ev.begin(), ev.end());
        ev_pos = ev.end();
        idmap.erase(idmap.begin(), idmap.end());
        csoundUnlockMutex(mutex);
    }
    int getTick()
    {
        return (int)rtick % tickMax;
    }
    void setNumTicks(int nticks)
    {
        tickMax = nticks;
        if ((int)rtick > nticks)
        {
            int t = (int)rtick % nticks;
            rtick = t;
        }
    }
    void setTick(int t)
    {
        t = t % tickMax;
        rtick = (MYFLT)(t % tickMax);
        //TODO: binary search would be faster
        csoundLockMutex(mutex);
        ev_pos = ev.lower_bound( t );
        csoundUnlockMutex(mutex);
    }
    void setTickDuration(MYFLT d)
    {
        if (!csound) {
            fprintf(stderr, "skipping setTickDuration, csound==NULL\n");
            return;
        }
        secs_per_tick = d;
        ticks_per_step = PERIOD_SIZE / ( d * 16000);
        if (0) fprintf(stderr, "INFO: duration %lf := ticks_per_step %lf\n", d, ticks_per_step);
    }
    void step(FILE * logf)
    {
        rtick += ticks_per_step;
        int tick = (int)rtick % tickMax;
        if (tick == tick_prev) return;

        csoundLockMutex(mutex);
        int events = 0;
        int loop0 = 0;
        int loop1 = 0;
        const int eventMax = 8;  //NOTE: events beyond this number will be ignored!!!
        if (!ev.empty()) 
        {
            if (tick < tick_prev) // should be true only after the loop wraps (not after insert)
            {
                while (ev_pos != ev.end())
                {
                    if (logf) ev_pos->second->ev_print(logf);
                    if (events < eventMax) ev_pos->second->event(csound, secs_per_tick);
                    ++ev_pos;
                    ++events;
                    ++loop0;
                }
                ev_pos = ev.begin();
            }
            while ((ev_pos != ev.end()) && (tick >= ev_pos->first))
            {
                if (logf) ev_pos->second->ev_print(logf);
                if (events < eventMax) ev_pos->second->event(csound, secs_per_tick);
                ++ev_pos;
                ++events;
                ++loop1;
            }
        }
        csoundUnlockMutex(mutex);
        tick_prev = tick;
        if (logf && (events >= eventMax)) fprintf(logf, "WARNING: %i/%i events at once (%i, %i)\n", events,ev.size(),loop0,loop1);
    }
    void addEvent(int id, char type, MYFLT * p, int np, bool in_ticks)
    {
        ev_t * e = new ev_t(type, p, np, in_ticks);

        idmap_t id_iter = idmap.find(id);
        if (id_iter == idmap.end())
        {
            //this is a new id
            csoundLockMutex(mutex);

            iter_t e_iter = ev.insert(pair_t(e->onset, e));

            //TODO: optimize by thinking about whether to do ev_pos = e_iter
            ev_pos = ev.upper_bound( tick_prev );
            idmap[id] = e_iter;

            csoundUnlockMutex(mutex);
        }
        else
        {
            if (_debug) fprintf(_debug, "ERROR: skipping request to add duplicate note %i\n", id);
        }
    }
    void delEvent(int id)
    {
        idmap_t id_iter = idmap.find(id);
        if (id_iter == idmap.end())
        {
            if (_debug) fprintf(_debug, "ERROR: delEvent request for unknown note %i\n", id);
        }
        else
        {
            csoundLockMutex(mutex);
            iter_t e_iter = id_iter->second;//idmap[id];
            if (e_iter == ev_pos) ++ev_pos;

            delete e_iter->second;
            ev.erase(e_iter);
            idmap.erase(id_iter);

            csoundUnlockMutex(mutex);
        }
    }
    void updateEvent(int id, int idx, float val)
    {
        idmap_t id_iter = idmap.find(id);
        if (id_iter == idmap.end())
        {
            if (_debug) fprintf(_debug, "ERROR: updateEvent request for unknown note %i\n", id);
            return;
        }

        //this is a new id
        csoundLockMutex(mutex);
        iter_t e_iter = id_iter->second;
        ev_t * e = e_iter->second;
        int onset = e->onset;
        e->update(idx, val);
        if (onset != e->onset)
        {
            ev.erase(e_iter);

            e_iter = ev.insert(pair_t(e->onset, e));

            //TODO: optimize by thinking about whether to do ev_pos = e_iter
            ev_pos = ev.upper_bound( tick_prev );
            idmap[id] = e_iter;
        }
        csoundUnlockMutex(mutex);
    }
};
struct TamTamSound
{
    void * ThreadID;
    CSOUND * csound;
    char * csound_orc;
    enum {CONTINUE, STOP} PERF_STATUS;
    int verbosity;
    FILE * _debug;
    FILE * _light;
    int thread_playloop;
    int thread_measurelag;
    EvLoop * loop;

    TamTamSound(char * orc)
        : ThreadID(NULL), csound(NULL), PERF_STATUS(STOP), verbosity(3), _debug(stderr), 
        _light(fopen("/sys/bus/platform/devices/leds-olpc/leds:olpc:keyboard/brightness", "w")),
        thread_playloop(0), thread_measurelag(0), loop(NULL)
    {
        if (1)
        {
            csound = csoundCreate(NULL);
        }
        else
        {
            csound = NULL;
        }
        csound_orc = strdup(orc);

        loop = new EvLoop(csound);
    }
    ~TamTamSound()
    {
        if (csound)
        {
            stop();
            delete loop;
            csoundDestroy(csound);
        }
        free(csound_orc);
        if (_debug) fclose(_debug);
    }
    uintptr_t thread_fn()
    {
        struct timeval tv;
        struct timeval tv0, tv1, tvd;

        double t_prev = 0.0; //value will be ignored
        double m = 0.0;
        int nloops = 0;
        long int nsamples = csoundGetOutputBufferSize(csound);
        long int nframes = nsamples/2; /* nchannels == 2 */ /* nframes per write */
        assert((unsigned)nframes == PERIOD_SIZE);
        float * buf = (float*)malloc(nsamples * sizeof(float));
        if (_debug) fprintf(_debug, "INFO: nsamples = %li nframes = %li\n", nsamples, nframes);

        int loops = 0;
        snd_pcm_t * phandle;
        ACFG(snd_pcm_open(&phandle, "default", SND_PCM_STREAM_PLAYBACK,0/*SND_PCM_NONBLOCK*/));
        if (setparams(phandle))
        {
            goto thread_fn_cleanup;
        }
        if (setswparams(phandle))
        {
            goto thread_fn_cleanup;
        }
        if (0 > snd_pcm_prepare(phandle))
        {
            ERROR_HERE;
            goto thread_fn_cleanup;
        }
        for (int i = 0; i < nframes; ++i)
        {
            buf[i*2] = buf[i*2+1] = 0.5 * sin( i / (float)nframes * 10.0 * M_PI);
        }

        while (PERF_STATUS == CONTINUE)
        {
            int err = 0;
            float *cbuf = csoundGetOutputBuffer(csound);
            gettimeofday(&tv0, 0);
            if (1 && csoundPerformBuffer(csound)) break;
            if (0) //check for computation time of csoundPerformBuffer
            {
                gettimeofday(&tv1, 0);
                tvd.tv_sec = tv1.tv_sec - tv0.tv_sec;
                tvd.tv_usec = tv1.tv_usec - tv0.tv_usec;

                if (tvd.tv_sec || (tvd.tv_usec > 10000))
                {
                    fprintf(stderr, "INFO: performBuffer time %lf (%li) (Scheduler got us!)\n", pytime(&tvd), nsamples);
                }
                else if ((nloops%50) == 0)
                {
                    fprintf(stderr, "INFO: performBuffer time %lf (%li) \n", pytime(&tvd), nsamples);
                }
            }
            assert(sizeof (MYFLT) == 4);

            if ((err = snd_pcm_writei (phandle, cbuf, nframes)) != nframes) 
            {
                const char * msg = NULL;
                snd_pcm_state_t state = snd_pcm_state(phandle);
                switch (state)
                {
                    case SND_PCM_STATE_OPEN:    msg = "open"; break;
                    case SND_PCM_STATE_SETUP:   msg = "setup"; break;
                    case SND_PCM_STATE_PREPARED:msg = "prepared"; break;
                    case SND_PCM_STATE_RUNNING: msg = "running"; break;
                    case SND_PCM_STATE_XRUN:    msg = "xrun"; break;
                    case SND_PCM_STATE_DRAINING: msg = "draining"; break;
                    case SND_PCM_STATE_PAUSED:  msg = "paused"; break;
                    case SND_PCM_STATE_SUSPENDED: msg = "suspended"; break;
                    case SND_PCM_STATE_DISCONNECTED: msg = "disconnected"; break;
                }
                //if (state != SND_PCM_STATE_XRUN)
                fprintf (stderr, "write to audio interface failed (%s)\nstate = %s\n", snd_strerror (err), msg);
                ACFG(snd_pcm_recover(phandle, err, 0));
                if (0 > snd_pcm_prepare(phandle))
                {
                    ERROR_HERE;
                    goto thread_fn_cleanup;
                }
                state = snd_pcm_state(phandle);

                assert(state == SND_PCM_STATE_PREPARED || state == SND_PCM_STATE_RUNNING);
            }
            if (thread_playloop)
            {
                loop->step(NULL);
            }
            ++nloops;
        }

thread_fn_cleanup:
    free(buf);
    snd_pcm_drain(phandle);

    snd_pcm_close (phandle);

    fprintf(stderr, "INFO: returning from performance thread\n");
    return 0;

    //NEVER REACHED!!!

        while ( (csoundPerformBuffer(csound) == 0) 
                && (PERF_STATUS == CONTINUE))
        {
            long nsamples = csoundGetOutputBufferSize(csound);
            MYFLT *buf =    csoundGetOutputBuffer(csound);
            MYFLT ssq = 0;
            for (int i = 0; i < nsamples; ++i) ssq += buf[i] * buf[i];
            if (_light)
            {
                char one = '1';
                char zero = '0';
                if (ssq > 0.2) fwrite(&one, 1, 1, _light);
                else           fwrite(&zero, 1, 1, _light);
                fflush(_light);
            }
            if (thread_measurelag)
            {
                gettimeofday(&tv, 0);
                double t_this = pytime(&tv);
                if (loops)
                {
                    if (m < t_this - t_prev)
                    {
                        m = t_this - t_prev;
                        if (_debug) fprintf(_debug, "maximum lag %lf\n", m);
                    }
                }
                t_prev = t_this;
            }
            if (thread_playloop)
            {
                loop->step(NULL);
            }
            ++loops;
        }
        fprintf(stderr, "INFO: aborted performance thread\n");
        return 0;
    }
    static uintptr_t csThread(void *clientData)
    {
        return ((TamTamSound*)clientData)->thread_fn();
    }
    int start()
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return 1;
        }
        if (!ThreadID)
        {
            int argc=3;
            char  **argv = (char**)malloc(argc*sizeof(char*));
            argv[0] = "csound";
            argv[1] ="-m0";
            argv[2] = csound_orc;
            if (_debug) fprintf(_debug, "loading file %s\n", csound_orc);

            //csoundInitialize(&argc, &argv, 0);
            csoundPreCompile(csound);
            csoundSetHostImplementedAudioIO(csound, 1, PERIOD_SIZE);
            int result = csoundCompile(csound, argc, &(argv[0]));
            free(argv);

            if (!result)
            {
                PERF_STATUS = CONTINUE;
                ThreadID = csoundCreateThread(csThread, (void*)this);
                return 0;
            }
            else
            {
                if (_debug) fprintf(_debug, "ERROR: failed to compile orchestra\n");
                ThreadID =  NULL;
                return 1;
            }
        }
        return 1;
    }
    int stop()
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return 1;
        }
        if (ThreadID)
        {
            PERF_STATUS = STOP;
            if (_debug) fprintf(_debug, "INFO: stop()");
            uintptr_t rval = csoundJoinThread(ThreadID);
            if (rval) if (_debug) fprintf(_debug, "WARNING: thread returned %zu\n", rval);
            ThreadID = NULL;
            csoundReset(csound);
            return 0;
        }
        return 1;
    }

    void scoreEvent(char type, MYFLT * p, int np)
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return ;
        }
        if ((verbosity > 2) && _debug)
        {
            fprintf(_debug, "INFO: scoreEvent %c ", type);
            for (int i = 0; i < np; ++i) fprintf(_debug, "%lf ", p[i]);
            fprintf(_debug, "\n");
        }
        csoundScoreEvent(csound, type, p, np);
    }
    void inputMessage(const char * msg)
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return ;
        }
        if (_debug && (verbosity > 2)) fprintf(_debug, "%s\n", msg);
        csoundInputMessage(csound, msg);
    }
    bool good()
    {
        return csound != NULL;
    }

    void setMasterVolume(MYFLT vol)
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return ;
        }
        MYFLT *p;
        if (!(csoundGetChannelPtr(csound, &p, "masterVolume", CSOUND_CONTROL_CHANNEL | CSOUND_INPUT_CHANNEL)))
            *p = (MYFLT) vol;
        else
        {
            if (_debug) fprintf(_debug, "ERROR: failed to set master volume\n");
        }
    }

    void setTrackpadX(MYFLT value)
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return ;
        }
        MYFLT *p;
        if (!(csoundGetChannelPtr(csound, &p, "trackpadX", CSOUND_CONTROL_CHANNEL | CSOUND_INPUT_CHANNEL)))
            *p = (MYFLT) value;
        else
        {
            fprintf(_debug, "ERROR: failed to set trackpad X value\n");
        }
    }

    void setTrackpadY(MYFLT value)
    {
        if (!csound) {
            fprintf(stderr, "skipping %s, csound==NULL\n", __FUNCTION__);
            return ;
        }
        MYFLT *p;
        if (!(csoundGetChannelPtr(csound, &p, "trackpadY", CSOUND_CONTROL_CHANNEL | CSOUND_INPUT_CHANNEL)))
            *p = (MYFLT) value;
        else
        {
            fprintf(_debug, "ERROR: failed to set trackpad Y value\n");
        }
    }
};

TamTamSound * sc_tt = NULL;

static void cleanup(void)
{
    if (sc_tt)
    {
        delete sc_tt;
        sc_tt = NULL;
    }
}

#define DECL(s) static PyObject * s(PyObject * self, PyObject *args)
#define RetNone Py_INCREF(Py_None); return Py_None;

//call once at end
DECL(sc_destroy)
{
    if (!PyArg_ParseTuple(args, ""))
    {
        return NULL;
    }
    if (sc_tt)
    {
        delete sc_tt;
        sc_tt = NULL;
    }
    RetNone;
}
//call once at startup, should return 0
DECL(sc_initialize) //(char * csd)
{
    char * str;
    if (!PyArg_ParseTuple(args, "s", &str ))
    {
        return NULL;
    }
    sc_tt = new TamTamSound(str);
    atexit(&cleanup);
    if (sc_tt->good()) 
        return Py_BuildValue("i", 0);
    else
        return Py_BuildValue("i", -1);
}
//compile the score, connect to device, start a sound rendering thread
DECL(sc_start)
{
    if (!PyArg_ParseTuple(args, "" ))
    {
        return NULL;
    }
    return Py_BuildValue("i", sc_tt->start());
}
//stop csound rendering thread, disconnect from sound device, clear tables.
DECL(sc_stop) 
{
    if (!PyArg_ParseTuple(args, "" ))
    {
        return NULL;
    }
    return Py_BuildValue("i", sc_tt->stop());
}
DECL(sc_scoreEvent) //(char type, farray param)
{
    char ev_type;
    PyObject *o;
    if (!PyArg_ParseTuple(args, "cO", &ev_type, &o ))
    {
        return NULL;
    }
    if (o->ob_type
            &&  o->ob_type->tp_as_buffer
            &&  (1 == o->ob_type->tp_as_buffer->bf_getsegcount(o, NULL)))
    {
        if (o->ob_type->tp_as_buffer->bf_getreadbuffer)
        {
            void * ptr;
            size_t len;
            len = o->ob_type->tp_as_buffer->bf_getreadbuffer(o, 0, &ptr);
            if (0) fprintf(stderr, "writeable buffer of length %zu at %p\n", len, ptr);
            float * fptr = (float*)ptr;
            size_t flen = len / sizeof(float);
            sc_tt->scoreEvent(ev_type, fptr, flen);

            Py_INCREF(Py_None);
            return Py_None;
        }
        else
        {
            assert(!"asdf");
        }
    }
    assert(!"not reached");
    return NULL;
}
DECL(sc_setMasterVolume) //(float v)
{
    float v;
    if (!PyArg_ParseTuple(args, "f", &v))
    {
        return NULL;
    }
    sc_tt->setMasterVolume(v);
    Py_INCREF(Py_None);
    return Py_None;
}
DECL(sc_setTrackpadX) //(float v)
{
    float v;
    if (!PyArg_ParseTuple(args, "f", &v))
    {
        return NULL;
    }
    sc_tt->setTrackpadX(v);
    Py_INCREF(Py_None);
    return Py_None;
}
DECL(sc_setTrackpadY) //(float v)
{
    float v;
    if (!PyArg_ParseTuple(args, "f", &v))
    {
        return NULL;
    }
    sc_tt->setTrackpadY(v);
    Py_INCREF(Py_None);
    return Py_None;
}
DECL(sc_loop_getTick) // -> int
{
    if (!PyArg_ParseTuple(args, "" ))
    {
        return NULL;
    }
    return Py_BuildValue("i", sc_tt->loop->getTick());
}
DECL(sc_loop_setNumTicks) //(int nticks)
{
    int nticks;
    if (!PyArg_ParseTuple(args, "i", &nticks ))
    {
        return NULL;
    }
    sc_tt->loop->setNumTicks(nticks);
    RetNone;
}
DECL(sc_loop_setTick) // (int ctick)
{
    int ctick;
    if (!PyArg_ParseTuple(args, "i", &ctick ))
    {
        return NULL;
    }
    sc_tt->loop->setTick(ctick);
    RetNone;
}
DECL(sc_loop_setTickDuration) // (MYFLT secs_per_tick)
{
    float spt;
    if (!PyArg_ParseTuple(args, "f", &spt ))
    {
        return NULL;
    }
    sc_tt->loop->setTickDuration(spt);
    RetNone;
}
DECL(sc_loop_addScoreEvent) // (int id, int duration_in_ticks, char type, farray param)
{
    int qid;
    int inticks;
    char ev_type;
    PyObject *o;
    if (!PyArg_ParseTuple(args, "iicO", &qid, &inticks, &ev_type, &o ))
    {
        return NULL;
    }
    if (o->ob_type
            &&  o->ob_type->tp_as_buffer
            &&  (1 == o->ob_type->tp_as_buffer->bf_getsegcount(o, NULL)))
    {
        if (o->ob_type->tp_as_buffer->bf_getreadbuffer)
        {
            void * ptr;
            size_t len;
            len = o->ob_type->tp_as_buffer->bf_getreadbuffer(o, 0, &ptr);
            if (0) fprintf(stderr, "writeable buffer of length %zu at %p\n", len, ptr);
            float * fptr = (float*)ptr;
            size_t flen = len / sizeof(float);
            sc_tt->loop->addEvent(qid, ev_type, fptr, flen, inticks);

            Py_INCREF(Py_None);
            return Py_None;
        }
        else
        {
            assert(!"asdf");
        }
    }
    assert(!"not reached");
    return NULL;
}
DECL(sc_loop_delScoreEvent) // (int id)
{
    int id;
    if (!PyArg_ParseTuple(args, "i", &id ))
    {
        return NULL;
    }
    sc_tt->loop->delEvent(id);
    RetNone;
}
DECL(sc_loop_updateEvent) // (int id)
{
    int id;
    int idx;
    float val;
    if (!PyArg_ParseTuple(args, "iif", &id, &idx, &val ))
    {
        return NULL;
    }
    sc_tt->loop->updateEvent(id, idx, val);
    RetNone;
}
DECL(sc_loop_clear)
{
    if (!PyArg_ParseTuple(args, "" ))
    {
        return NULL;
    }
    sc_tt->loop->clear();
    RetNone;
}
DECL(sc_loop_playing) // (int tf)
{
    int i;
    if (!PyArg_ParseTuple(args, "i", &i ))
    {
        return NULL;
    }
    sc_tt->thread_playloop = i;
    RetNone;
}
DECL (sc_inputMessage) //(const char *msg)
{
    char * msg;
    if (!PyArg_ParseTuple(args, "s", &msg ))
    {
        return NULL;
    }
    sc_tt->inputMessage(msg);
    RetNone;
}

#define MDECL(s) {""#s, s, METH_VARARGS, "documentation of "#s"... nothing!"},
static PyMethodDef SpamMethods[] = {
    {"sc_destroy", sc_destroy, METH_VARARGS,""},
    {"sc_initialize", sc_initialize, METH_VARARGS,""},
    {"sc_start", sc_start, METH_VARARGS,""},
    {"sc_stop", sc_stop, METH_VARARGS,""},
    {"sc_scoreEvent", sc_scoreEvent, METH_VARARGS, ""},
    {"sc_setMasterVolume", sc_setMasterVolume, METH_VARARGS, ""},
    {"sc_setTrackpadX", sc_setTrackpadX, METH_VARARGS, ""},
    {"sc_setTrackpadY", sc_setTrackpadY, METH_VARARGS, ""},
    MDECL(sc_loop_getTick)
    MDECL(sc_loop_setNumTicks)
    MDECL(sc_loop_setTick)
    MDECL(sc_loop_setTickDuration)
    MDECL(sc_loop_delScoreEvent)
    MDECL(sc_loop_addScoreEvent) // (int id, int duration_in_ticks, char type, farray param)
    MDECL(sc_loop_updateEvent) // (int id)
    MDECL(sc_loop_clear)
    MDECL(sc_loop_playing)
    MDECL(sc_inputMessage)
    {NULL, NULL, 0, NULL} /*end of list */
};

PyMODINIT_FUNC
initaclient(void)
{
    (void) Py_InitModule("aclient", SpamMethods);
}


