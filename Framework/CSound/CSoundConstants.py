from Framework.Constants import Constants
from Framework.CSound.Instrument import Instrument

class CSoundConstants:
    #PATHS
    SOUNDS_DIR = Constants.TAM_TAM_ROOT + "/Resources/Sounds"
    FILES_DIR = Constants.TAM_TAM_ROOT + "/Resources/"
    
    #SERVER
    SERVER_ADDRESS = "localhost"
    SERVER_PORT = 6783

    SERVER_REQUIRED = 0

    INIT_ATTEMPTS = 2
    INIT_DELAY = 1.0
    
    #SOUNDS
    
    # animals
    OUNK = "ounk"
    DOG = "dog"
    DUCK = "duck"
    BIRD = "bird"
    CAT = "cat"
    DUCK2 = "duck2"
    
    # synthesis
    ADD = "add"
    FM1 = "fm1"
    FM2 = "fm2"

    # melodic percussion
    GAM = "gam"
    GONG = "gong"
    PIANO = "piano"
    
    # non-melodic percussion
    DRUM1CHINE = "drum1chine"
    DRUM1CRASH = "drum1crash"
    DRUM1FLOORTOM = "drum1floortom"
    DRUM1HARDRIDE = "drum1hardride"
    DRUM1HATPEDAL = "drum1hatpedal"
    DRUM1HATSHOULDER = "drum1hatshoulder"
    DRUM1KICK = "drum1kick"
    DRUM1RIDEBELL = "drum1ridebell"
    DRUM1SNARE = "drum1snare"
    DRUM1SNARERIMSHOT = "drum1snarerimshot"
    DRUM1SNARESIDESTICK = "drum1snaresidestick"
    DRUM1SPLASH = "drum1splash"
    DRUM1TOM = "drum1tom"
    DRUM1KIT = "drum1kit"

    DRUM2DARBUKADOOM = "drum2darbukadoom"
    DRUM2DARBUKAFINGER = "drum2darbukafinger"
    DRUM2DARBUKAPIED = "drum2darbukapied"
    DRUM2DARBUKAPIEDSOFT = "drum2darbukapiedsoft"
    DRUM2DARBUKAROLL = "drum2darbukaroll"
    DRUM2DARBUKASLAP = "drum2darbukaslap"
    DRUM2DARBUKATAK = "drum2darbukatak"
    DRUM2HATFLANGER = "drum2hatflanger"
    DRUM2HATPIED = "drum2hatpied"

    DRUM2HATPIED2 = "drum2hatpied2"
    DRUM2TAMBOURINEPIED = "drum2tambourinepied"
    DRUM2TAMBOURINEPIEDSOFT = "drum2tambourinepiedsoft"
    DRUM2KIT = "drum2kit"

    DRUM3COWBELL = "drum3cowbell"
    DRUM3COWBELLTIP = "drum3cowbelltip"
    DRUM3CUP = "drum3cup"
    DRUM3DJEMBELOW = "drum3djembelow"
    DRUM3DJEMBEMID = "drum3djembemid"
    DRUM3DJEMBESIDESTICK = "drum3djembesidestick"
    DRUM3DJEMBESLAP = "drum3djembeslap"
    DRUM3DJEMBESTICKMID = "drum3djembestickmid"
    DRUM3METALSTAND = "drum3metalstand"
    DRUM3PEDALPERC = "drum3pedalperc"
    DRUM3RAINSTICK = "drum3rainstick"
    DRUM3TAMBOURINEHIGH = "drum3tambourinehigh"
    DRUM3TAMBOURINELOW = "drum3tambourinelow"
    DRUM3KIT = "drum3kit"

    # weird
    BOTTLE = "bottle"
    CLANG = "clang"
    CLING = "cling"
    DOOR = "door"
    LAUGH = "laugh"
    OW = "ow"
    SHEEP = "sheep"
    SPRING = "spring"
    TCHIWO = "tchiwo"
    WATER = "water"
    ZAP = "zap"

    # plucked string
    BASSE = "basse"
    GUIT = "guit"
    KOTO = "koto"

    # perc
    MARACAS = "maracas"
    MARIMBA = "marimba"
    TRIANGLE = "triangle"
    
    # woodwind
    CLARINETTE = "clarinette"
    FLUTE = "flute"
    TRUMPET = 'trumpet'
    VOIX = "voix"

    # recorded snds
    MIC1 = "mic1"
    MIC2 = "mic2"
    MIC3 = "mic3"
    MIC4 = "mic4"

    LOW, MID, HIGH, PUNCH = range( 4 )

    # Sounds categories: musicInst, animals, drum, people, electronic, concret, mic
    #INSTRUMENTS ( csound table, csound instrument, register, instrumentClass, category )
    INSTRUMENT_TABLE_OFFSET = 5000
    INST_FREE = 5000
    INST_TIED = 5001
    INST_PERC = 5002
    INST_SIMP = 5003
    INST_XFAD = 5004
    INST_KARP = 5005
    INST_FMSN = 5006
    INST_WAVE = 5007

    INSTRUMENTS = { 
                    OUNK :                    Instrument(  0, INST_SIMP, MID, 'melo', 'animals' ),
                    GAM :                     Instrument(  1, INST_PERC, HIGH, 'melo', 'musicInst' ),
                    GONG :                    Instrument(  2, INST_PERC, LOW, 'melo', 'musicInst' ),
                    GUIT :                    Instrument(  3, INST_TIED, MID, 'melo', 'musicInst' ),
                    KOTO :                    Instrument(  4, INST_PERC, HIGH, 'melo', 'musicInst' ),
                    CLARINETTE :              Instrument(  5, INST_TIED, MID, 'melo', 'musicInst' ),
                    FLUTE :                   Instrument(  6, INST_TIED, MID, 'melo', 'musicInst' ),
                    MIC1:                     Instrument(  7, INST_TIED, MID, 'melo', 'mic' ),
                    MIC2:                     Instrument(  8, INST_TIED, MID, 'melo', 'mic' ),
                    MIC3:                     Instrument(  9, INST_TIED, MID, 'melo', 'mic' ),
                    MIC4:                     Instrument( 10, INST_TIED, MID, 'melo', 'mic' ),
                    DRUM1HATPEDAL:            Instrument( 11, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM1HATSHOULDER:         Instrument( 12, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM1HARDRIDE:            Instrument( 13, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM1RIDEBELL:            Instrument( 14, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM1SNARE:               Instrument( 15, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM1SNARERIMSHOT:        Instrument( 16, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM1SNARESIDESTICK:      Instrument( 17, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM1CRASH:               Instrument( 18, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM1SPLASH:              Instrument( 19, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM1TOM:                 Instrument( 20, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM1FLOORTOM:            Instrument( 21, INST_SIMP, LOW, 'drum', 'drum'),
                    DRUM1CHINE:               Instrument( 22, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM1KICK:                Instrument( 23, INST_SIMP, LOW, 'drum', 'drum' ),
                    PIANO:                    Instrument( 24, INST_PERC, MID, 'melo', 'musicInst' ),
                    DOG:                      Instrument( 25, INST_SIMP, MID, 'melo', 'animals' ),
                    DUCK:                     Instrument( 26, INST_SIMP, MID, 'melo', 'animals' ),
                    DRUM2DARBUKADOOM:         Instrument( 27, INST_SIMP, LOW, 'drum', 'drum' ),
                    DRUM2DARBUKAPIED:         Instrument( 28, INST_SIMP, LOW, 'drum', 'drum' ),
                    DRUM2DARBUKAPIEDSOFT:     Instrument( 29, INST_SIMP, LOW, 'drum', 'drum' ),
                    DRUM2HATFLANGER:          Instrument( 30, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM2DARBUKATAK:          Instrument( 31, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM2DARBUKAFINGER:       Instrument( 32, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM2DARBUKAROLL:         Instrument( 33, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM2DARBUKASLAP:         Instrument( 34, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM2HATPIED:             Instrument( 35, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM2TAMBOURINEPIED:      Instrument( 36, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM2HATPIED2:            Instrument( 37, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM2TAMBOURINEPIEDSOFT:  Instrument( 38, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM3COWBELL:             Instrument( 39, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM3COWBELLTIP:          Instrument( 40, INST_SIMP, MID, 'drum', 'drum'),
                    DRUM3CUP:                 Instrument( 41, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM3DJEMBELOW:           Instrument( 42, INST_SIMP, LOW, 'drum', 'drum' ),
                    DRUM3DJEMBEMID:           Instrument( 43, INST_SIMP, HIGH, 'drum', 'drum' ),
                    DRUM3DJEMBESIDESTICK:     Instrument( 44, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM3DJEMBESLAP:          Instrument( 45, INST_SIMP, LOW, 'drum', 'drum' ),
                    DRUM3DJEMBESTICKMID:      Instrument( 46, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM3METALSTAND:          Instrument( 47, INST_SIMP, MID, 'drum', 'drum' ),
                    DRUM3PEDALPERC:           Instrument( 48, INST_SIMP, LOW, 'drum', 'drum' ),
                    DRUM3RAINSTICK:           Instrument( 49, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM3TAMBOURINEHIGH:      Instrument( 50, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    DRUM3TAMBOURINELOW:       Instrument( 51, INST_SIMP, PUNCH, 'drum', 'drum' ),
                    ADD:                      Instrument( 52, INST_TIED, MID, 'melo', 'electronic' ),
                    FM1:                      Instrument( 53, INST_PERC, MID, 'melo', 'electronic' ),
                    FM2:                      Instrument( 54, INST_PERC, MID, 'melo', 'electronic' ),
                    BIRD:                     Instrument( 55, INST_TIED, MID, 'melo', 'animals' ),
                    CAT:                      Instrument( 56, INST_TIED, MID, 'melo', 'animals' ),
                    DUCK2:                    Instrument( 57, INST_SIMP, MID, 'melo', 'animals' ),
                    BOTTLE:                   Instrument( 58, INST_TIED, MID, 'melo', 'concret' ),
                    CLANG:                    Instrument( 59, INST_SIMP, MID, 'melo', 'concret' ),
                    OW:                       Instrument( 60, INST_SIMP, MID, 'melo', 'people' ),
                    SHEEP:                    Instrument( 61, INST_SIMP, MID, 'melo', 'animals' ),
                    SPRING:                   Instrument( 62, INST_SIMP, MID, 'melo', 'concret' ),
                    WATER:                    Instrument( 63, INST_SIMP, MID, 'melo', 'concret' ),
                    ZAP:                      Instrument( 64, INST_TIED, MID, 'melo', 'electronic' ),
                    TRUMPET:                  Instrument( 65, INST_TIED, MID, 'melo', 'musicInst'),
                    MARACAS:                  Instrument( 66, INST_SIMP, MID, "melo", 'musicInst'),
                    MARIMBA:                  Instrument( 67, INST_TIED, MID, "melo", 'musicInst'),
                    TRIANGLE:                 Instrument( 68, INST_PERC, MID, "melo", 'musicInst'),
                    LAUGH:                    Instrument( 69, INST_TIED, MID, 'melo', 'people' ),
                    VOIX:                     Instrument( 70, INST_TIED, MID, 'melo', 'people' ), 
                    CLING:                    Instrument( 71, INST_SIMP, MID, 'melo', 'electronic' ),
                    TCHIWO:                   Instrument( 72, INST_SIMP, MID, 'melo', 'electronic' ),
                    DOOR:                     Instrument( 73, INST_SIMP, MID, 'melo', 'concret' ),
                    BASSE :                   Instrument( 74, INST_TIED, MID, 'melo', 'musicInst' )}
                    

    DRUM1INSTRUMENTS = {   24 :   DRUM1KICK,
                                                            26 : DRUM1FLOORTOM,
                                                            28 : DRUM1TOM,
                                                            30 : DRUM1CHINE,
                                                            32 : DRUM1SPLASH,  
                                                            34 : DRUM1CRASH,
                                                            36 : DRUM1SNARESIDESTICK,
                                                            38 : DRUM1SNARERIMSHOT,
                                                            40 : DRUM1SNARE,
                                                            42 : DRUM1RIDEBELL,
                                                            44 : DRUM1HARDRIDE,
                                                            46 : DRUM1HATSHOULDER,
                                                            48 : DRUM1HATPEDAL }

    DRUM2INSTRUMENTS = {   24 : DRUM2DARBUKADOOM,
                                                            26 : DRUM2DARBUKAPIED,
                                                            28 : DRUM2DARBUKAPIEDSOFT,
                                                            30 : DRUM2HATFLANGER,
                                                            32 : DRUM2DARBUKATAK,
                                                            34 : DRUM2DARBUKATAK,
                                                            36 : DRUM2DARBUKAFINGER,
                                                            38 : DRUM2DARBUKAROLL,
                                                            40 : DRUM2DARBUKASLAP,
                                                            42 : DRUM2HATPIED,
                                                            44 : DRUM2TAMBOURINEPIED,
                                                            46 : DRUM2HATPIED2,
                                                            48 : DRUM2TAMBOURINEPIEDSOFT } 

    DRUM3INSTRUMENTS = {   24 : DRUM3DJEMBELOW,
                                                            26 : DRUM3PEDALPERC,
                                                            28 : DRUM3DJEMBESLAP,
                                                            30 : DRUM3TAMBOURINEHIGH,
                                                            32 : DRUM3TAMBOURINELOW,
                                                            34 : DRUM3RAINSTICK,
                                                            36 : DRUM3DJEMBEMID,
                                                            38 : DRUM3DJEMBESIDESTICK,
                                                            40 : DRUM3DJEMBESTICKMID,
                                                            42 : DRUM3COWBELL,
                                                            44 : DRUM3COWBELLTIP,
                                                            46 : DRUM3CUP,
                                                            48 : DRUM3METALSTAND }              

    RECORDABLE_INSTRUMENTS = set( [ MIC1, MIC2, MIC3, MIC4 ] )
    RECORDABLE_INSTRUMENT_CSOUND_IDS = {  MIC1 : 7,
                                          MIC2 : 8,
                                          MIC3 : 9,
                                          MIC4 : 10 }

    #COMMANDS
    LOAD_INSTRUMENT_COMMAND = "perf.InputMessage('f%d 0 0 -1 \"%s\" 0 0 0')\n"
    PLAY_NOTE_COMMAND =       "perf.InputMessage('i %d.%d %f %f %f %f %f %f %d %f %f %d %f')\n"
    PLAY_NOTE_COMMAND_MINUS_DELAY =       "perf.InputMessage('i %d.%d %s %f %f %f %f %f %d %f %f %d %f')"
    PLAY_NOTE_OFF_COMMAND =   "perf.InputMessage('i %s.%s .2 0.01 1. 0. 0. 0.5 %d 0 0 0 0')\n" % ('%d', '%d', INSTRUMENT_TABLE_OFFSET )
    MIC_RECORDING_COMMAND =   "perf.InputMessage('i5201 0 10 %d')\n"
    UNLOAD_TABLES_COMMAND = "perf.InputMessage('i%d 0 0.1')\n" % INST_FREE

