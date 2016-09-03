#
# Music Bottles player code, based on code by Tomer Weller (tomerweller)
#
import pygame.mixer  # bring in mixing capabilities
import time
import RPi.GPIO as GPIO  # define

GPIO.setmode(GPIO.BCM)  # To use the pin names on the RPi3
GPIO.setup(17, GPIO.IN)  # Setup the pins from the Arduino for input
GPIO.setup(22, GPIO.IN)
GPIO.setup(23, GPIO.IN)
GPIO.setup(27, GPIO.IN)
#GPIO.setup(26, GPIO.IN, pull_up_down=GPIO.PUD_UP) 
GPIO.setup(19, GPIO.IN, pull_up_down=GPIO.PUD_UP) #buttons to choose music
GPIO.setup(13, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(6, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(5, GPIO.IN, pull_up_down=GPIO.PUD_UP)

pygame.mixer.init(22050, -16, 4)  # Begins the pygame mixer module


class Track:
    def __init__(self, path):
        self.chan = pygame.mixer.find_channel()
        self.path = path
        self.chan.play(pygame.mixer.Sound(self.path), -1)
        self.chan.pause()
        self.chan.set_volume(0.0)

    def tick(self, target_volume):
        if not isReset:
            self.chan.unpause()
            current_volume = self.chan.get_volume()
            if target_volume is 1:
                new_volume = 1;
            else:
                new_volume = 0.95 * current_volume
            self.chan.set_volume(new_volume if new_volume>0 else 0)

    def get_volume(self):
        return self.chan.get_volume()

    def stop(self):
        self.chan.stop()

    def reset(self):
        self.chan.play(pygame.mixer.Sound(self.path), -1)
        self.chan.pause()

    def load(self, newPath):
        self.path = newPath
        self.chan.play(pygame.mixer.Sound(self.path), -1)
        self.chan.pause()
        


track1 = Track('music-files/jazz1.wav')
track2 = Track('music-files/jazz2.wav')
track3 = Track('music-files/jazz3.wav')
tracks = [track1, track2, track3]

isReset = True
isAllMusicUp = True

while True:
    gpio1 = GPIO.input(22)
    gpio2 = GPIO.input(17)
    gpio3 = GPIO.input(23)    
    gpioReset = GPIO.input(27)    

    gpioJazz = GPIO.input(19)
    gpioClass = GPIO.input(13)
    gpioTech = GPIO.input(6)
    gpioBoston = GPIO.input(5)

    
    
    if (gpioJazz is 1 and gpioClass is 1 and gpioTech is 1 and gpioBoston is 1):
        isAllMusicUp = True

    if gpioJazz is 0 and isAllMusicUp:
        isAllMusicUp = False
        isReset = True
        track1.load('music-files/jazz1.wav')
        track2.load('music-files/jazz2.wav')
        track3.load('music-files/jazz3.wav')

    
    if gpioClass is 0 and isAllMusicUp:
        isAllMusicUp = False
        isReset = True
        track1.load('music-files/classic1.wav')
        track2.load('music-files/classic2.wav')
        track3.load('music-files/classic3.wav')

    
    if gpioTech is 0 and isAllMusicUp:
        isAllMusicUp = False
        isReset = True
        track1.load('music-files/synth1.wav')
        track2.load('music-files/synth2.wav')
        track3.load('music-files/synth3.wav')

    
    if gpioBoston is 0 and isAllMusicUp:
        isAllMusicUp = False
        isReset = True
        track1.load('music-files/boston1.wav')
        track2.load('music-files/boston2.wav')
        track3.load('music-files/boston3.wav')

    

    if gpioReset is 0:
        if not isReset:
            for track in tracks:
                track.reset()
            print "resetA"
            isReset = True

    # so that ticks are ignored until music actually is supposed to start playing
    if isReset and (gpio1 is 1 or gpio2 is 1 or gpio3 is 1):
        isReset = False
#        for track in tracks:
#            track.reset()
#        print "resetB"

    track1.tick(1 if gpio1 else 0)
    track2.tick(1 if gpio2 else 0)
    track3.tick(1 if gpio3 else 0)

    print "gpio", '\t', gpio1, '\t', gpio2, '\t', gpio3
    print "volume", '\t', track1.get_volume(), '\t', track2.get_volume(), '\t', track3.get_volume()

    time.sleep(0.025)

track1.stop()
track1.stop()
track1.stop()
