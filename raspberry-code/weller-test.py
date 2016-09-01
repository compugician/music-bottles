import pygame.mixer  # bring in mixing capabilities
import time
import RPi.GPIO as GPIO  # define

GPIO.setmode(GPIO.BCM)  # To use the pin names on the RPi3
GPIO.setup(23, GPIO.IN)  # Setup the pins from the Arduino for input
GPIO.setup(24, GPIO.IN)
GPIO.setup(25, GPIO.IN)
pygame.mixer.init(22050, -16, 4)  # Begins the pygame mixer module


class Track:
    def __init__(self, path):
        self.chan = pygame.mixer.find_channel()
        self.path = path
        self.chan.play(pygame.mixer.Sound(self.path), -1)
        self.chan.set_volume(0.0)

    def tick(self, target_volume):
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
        self.chan.stop()
        self.chan.play(pygame.mixer.Sound(self.path), -1)
        


track1 = Track('music-files/jazz1.wav')
track2 = Track('music-files/jazz2.wav')
track3 = Track('music-files/jazz3.wav')
tracks = [track1, track2, track3]

while True:
    gpio1 = GPIO.input(23)
    gpio2 = GPIO.input(24)
    gpio3 = GPIO.input(25)    

#    if gpio1 is 0 and gpio2 is 0 and gpio3 is 0:
#        for track in tracks:
#            track.reset()
#        print "reset"

    track1.tick(1 if gpio1 else 0)
    track2.tick(1 if gpio2 else 0)
    track3.tick(1 if gpio3 else 0)

    print "gpio", '\t', gpio1, '\t', gpio2, '\t', gpio3
    print "volume", '\t', track1.get_volume(), '\t', track2.get_volume(), '\t', track3.get_volume()

    time.sleep(0.025)

track1.stop()
track1.stop()
track1.stop()
