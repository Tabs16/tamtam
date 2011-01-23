import locale
locale.setlocale(locale.LC_NUMERIC, 'C')
import signal , time , sys , os, shutil
import pygtk
pygtk.require( '2.0' )
import gtk

import gobject
import time

import common.Util.Instruments
import common.Config as Config
from   common.Util.CSoundClient import new_csound_client
from   common.Util.Profiler import TP

from   Mini.miniTamTamMain import miniTamTamMain
from   common.Util.Trackpad import Trackpad
from   gettext import gettext as _
import commands
from sugar.activity import activity

class TamTamMini(activity.Activity):

    __gtype_name__ = 'TamTamMiniWindow'

    def __init__(self, handle):
        self.mini = None

        activity.Activity.__init__(self, handle)

        color = gtk.gdk.color_parse(Config.WS_BCK_COLOR)
        self.modify_bg(gtk.STATE_NORMAL, color)

        self.set_title('TamTam Mini')
        self.set_resizable(False)

        self.trackpad = Trackpad( self )
        self.trackpad.setContext('mini')

        self.connect('notify::active', self.onActive)
        self.connect('destroy', self.onDestroy)

        #load the sugar toolbar
        toolbox = activity.ActivityToolbox(self)
        self.set_toolbox(toolbox)
        self.activity_toolbar = toolbox.get_activity_toolbar()
        toolbox.show()

        self.mini = miniTamTamMain(self)
        self.mini.onActivate(arg = None)
        self.mini.updateInstrumentPanel()
        #self.modeList[mode].regenerate()

        self.activity_toolbar.share.hide()
        self.activity_toolbar.keep.hide()

        self.set_canvas(self.mini)
        self.show()

    def do_size_allocate(self, allocation):
        activity.Activity.do_size_allocate(self, allocation)
        if self.mini is not None:
            self.mini.updateInstrumentPanel()

    def onActive(self, widget = None, event = None):
        if widget.props.active == False:
            csnd = new_csound_client()
            csnd.connect(False)
        else:
            csnd = new_csound_client()
            csnd.connect(True)

    def onDestroy(self, arg2):
        if Config.DEBUG: print 'DEBUG: TamTam::onDestroy()'

        self.mini.onDestroy()

        csnd = new_csound_client()
        csnd.connect(False)
        csnd.destroy()

        gtk.main_quit()

# no more dir created by TamTam
    def ensure_dir(self, dir, perms=0777, rw=os.R_OK|os.W_OK):
        if not os.path.isdir( dir ):
            try:
                os.makedirs(dir, perms)
            except OSError, e:
                print 'ERROR: failed to make dir %s: %i (%s)\n' % (dir, e.errno, e.strerror)
        if not os.access(dir, rw):
            print 'ERROR: directory %s is missing required r/w access\n' % dir

    def read_file(self,file_path):
        self.metadata['tamtam_subactivity'] = 'mini'

    def write_file(self,file_path):
        f = open(file_path,'w')
        f.close()
