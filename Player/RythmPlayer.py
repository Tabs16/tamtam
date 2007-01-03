import pygtk
pygtk.require( '2.0' )
import gtk 
import gobject

from Framework.Constants import Constants
from Framework.CSound.CSoundNote import CSoundNote
from Framework.CSound.CSoundClient import CSoundClient
from Framework.CSound.CSoundConstants import CSoundConstants

class RythmPlayer:
    def __init__( self, client ):
	self.csnd = client
        self.notesList = []
        self.tempo = 120
        self.currentTick = 0
        self.playbackTimeout = None
        self.beat = 4
	self.playState = 0

    def getCurrentTick(self):
        # used by keyboard
        return self.currentTick

    def setTempo( self, tempo ):
        self.tempo = tempo
        if self.playbackTimeout != None:
            gobject.source_remove(self.playbackTimeout)
	    self.playState = 0
            self.startPlayback()

    def playing( self ):
        return self.playbackTimeout != None
    
    def startPlayback( self ):
        if not self.playState:
            self.playbackTimeout = gobject.timeout_add( int(60000/self.tempo/12) , self.handleClock )
        #and call it right away too.
            self.handleClock()
	    self.playState = 1

    def stopPlayback( self ):
        if self.playbackTimeout != None:
            gobject.source_remove( self.playbackTimeout )
            self.playbackTimeout = None
            self.shutOff()
	    self.playState = 0

    def handleClock( self ) :
        rval = ""
        for stream in self.notesList:
            for note in stream:
                if note.onset == self.currentTick:
                    note.play()

        
        self.currentTick = self.currentTick + 1
        if self.currentTick >= (Constants.TICKS_PER_BEAT * self.beat):
            self.currentTick = 0

        return True

    def shutOff( self ):
        for track in range( Constants.NUMBER_OF_TRACKS ):
            for inst in [CSoundConstants.INST_TIED, CSoundConstants.INST_SIMP, CSoundConstants.INST_PERC]:
                self.csnd.sendText( CSoundConstants. PLAY_NOTE_OFF_COMMAND % ( inst, track ) )
                      
