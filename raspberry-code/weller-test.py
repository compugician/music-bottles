import pygame.mixer        			 # bring in mixing capabilities
import time
import RPi.GPIO as GPIO				 # define

GPIO.setmode(GPIO.BCM)				 # To use the pin names on the RPi3
GPIO.setup(23, GPIO.IN) 			 # Setup the pins from the Arduino for input
GPIO.setup(24, GPIO.IN)
GPIO.setup(25, GPIO.IN)
pygame.mixer.init(22050, -16, 4) 	 # Begins the pygame mixer module

chan1 = pygame.mixer.find_channel()
chan1.play(pygame.mixer.Sound('music-files/boston1.wav'), -1)
chan2 = pygame.mixer.find_channel()
chan2.play(pygame.mixer.Sound('music-files/boston2.wav'), -1)
chan3 = pygame.mixer.find_channel()
chan3.play(pygame.mixer.Sound('music-files/boston3.wav'), -1)

while True:
    chan1.set_volume(1 if GPIO.input(23) else 0)
    chan2.set_volume(1 if GPIO.input(24) else 0)
    chan3.set_volume(1 if GPIO.input(25) else 0)
    time.sleep(0.1)

chan1.stop()
chan2.stop()
chan3.stop()
