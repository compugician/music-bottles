import pygame.mixer        			 # bring in mixing capabilities
import time
import RPi.GPIO as GPIO				 # define

GPIO.setmode(GPIO.BCM)				 # To use the pin names on the RPi3
GPIO.setup(22, GPIO.IN)				 #active state from the arduino
GPIO.setup(23, GPIO.IN) 			 # Setup the pins from the Arduino for input
GPIO.setup(24, GPIO.IN)
GPIO.setup(25, GPIO.IN)
chan1playing = False
chan2playing = False
chan3playing = False
pygame.mixer.init(22050, -16, 4) 	 # Begins the pygame mixer module 

while(1):
    beginning = 0
    musicSelected = False
    sound1 = pygame.mixer.Sound('music-files/boston1.wav')
    sound2 = pygame.mixer.Sound('music-files/boston2.wav')
    sound3 = pygame.mixer.Sound('music-files/boston3.wav')
    while(GPIO.input(22) or 1):
#         print("gpio22 is: '"+str(GPIO.input(22))+"'")
        if (beginning == 0):
                print("beginning == 0")
                chan1 = pygame.mixer.find_channel()
                chan1.play(sound1, -1)
                chan1.set_volume(0)
                chan2 = pygame.mixer.find_channel()
                chan2.play(sound2, -1)
                chan2.set_volume(0)
                chan3 = pygame.mixer.find_channel()
                chan3.play(sound3, -1)
                chan3.set_volume(0)
        if (GPIO.input(23)):
                print "input 23"
                chan1.set_volume(1)
                chan1playing = True
        elif (chan1playing):	
                volume1 = chan1.get_volume()
                if (volume1 < 0.1):
                        chan1playing = False
                        chan1.set_volume(0)
                else: 
                        chan1.set_volume(volume1 - 0.1)
        if (GPIO.input(24)):
                print "input 24"
                chan2.set_volume(1)
                chan2playing = True
        elif (chan2playing):
                volume2 = chan2.get_volume()
                if (volume2 < 0.1):
                        chan2playing = False
                        chan2.set_volume(0)
                else: 
                        chan2.set_volume(volume2 - 0.1)		
        if (GPIO.input(25)):
                print "input 25"
                chan3.set_volume(1)
                chan3playing = True
        elif (chan3playing):
                volume3 = chan3.get_volume()
                if (volume3 < 0.1):
                        chan3playing = False
                        chan3.set_volume(0)
                else: 
                        chan3.set_volume(volume3 - 0.1)
        beginning = 1
        print ("vol1: "+str(chan1.get_volume())+"vol2: "+str(chan2.get_volume())+"vol3: "+str(chan3.get_volume()))
    print("OUT OF WHILE! gpio22 is: '"+str(GPIO.input(22))+"'")
    beginning = 0
    chan1.stop()
    chan2.stop()
    chan3.stop()
	

