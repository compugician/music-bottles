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
        self.chan.play(pygame.mixer.Sound(path), -1)
        self.target_volume = 0

    def tick(self, target_volume):
        current_volume = self.chan.get_volume()
        delta = 0.1 if current_volume < target_volume else -0.1 if current_volume > target_volume else 0
        self.chan.set_volume(current_volume + delta)

    def get_volume(self):
        return self.chan.get_volume()

    def stop(self):
        self.chan.stop()

track1 = Track('music-files/boston1.wav')
track2 = Track('music-files/boston2.wav')
track3 = Track('music-files/boston3.wav')

while True:
    track1.tick(1 if GPIO.input(23) else 0)
    track2.tick(1 if GPIO.input(24) else 0)
    track3.tick(1 if GPIO.input(25) else 0)
    print track1.get_volume(), track2.get_volume(), track3.get_volume()
    time.sleep(0.1)

track1.stop()
track1.stop()
track1.stop()
