""" 
Monitors the serial input from Arduino, looking for a line with either "activecomplete" or "quietcomplete"
These are sent when the user has stepped on all 4 steps in proper succession.

once these codes are received, we start listening on the mic
for activecomplete we listen for loud input, for quietcomplete we listen for a quiet (but present) input

Currently set so that if the value of the Arduino serial line is greater than 50
It starts recording and records for 2 seconds
Once the two seconds is done, it replays that recording 100 times
"""


"""Taking serial input and identifying if the photocell is uncovered"""
import sys
import pyaudio
import wave
import audioop
import time
#To edit range so they are all three digit values
from numpy import interp
import serial

def getAmbientAudio():
    CHUNK = 1024 
    FORMAT = pyaudio.paInt16 #paInt8
    CHANNELS = 2 
    RATE = 44100 #sample rate
    RECORD_SECONDS = 15

    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK) #buffer
    frames = []

    print "sensing ambient audio level"

    total = int(RATE / CHUNK * RECORD_SECONDS)
    total_rms = 0;
    for i in range(0, total):
        data = stream.read(CHUNK)
        frames.append(data) # 2 bytes(16 bits) per channel
        #rms gets the actual volume, since the data has lots of other info   
        rms = audioop.rms(data,2)
        total_rms += rms
        #rms_one = int(interp(rms, [1,4000],[1,1000]))
        #maybe I want to write a "head and tail character" so that I know when it ends
        # print rms
    avg_rms = total_rms/float(total)
    print "avg rms = " + str(avg_rms)
    return avg_rms
        
    stream.stop_stream()
    stream.close()

    # close PyAudio (5)
    p.terminate()


def checkaudio(level, baseline):
    CHUNK = 1024 
    FORMAT = pyaudio.paInt16 #paInt8
    CHANNELS = 2 
    RATE = 44100 #sample rate
    RECORD_SECONDS = 10

    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK) #buffer

    frames = []

    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        data = stream.read(CHUNK)
        frames.append(data) # 2 bytes(16 bits) per channel
        #rms gets the actual volume, since the data has lots of other info   
        rms = audioop.rms(data,2)
        print rms
        if level == 1 and rms > baseline*10:
            # active
            print "ACTIVE. SCREAMED. DONE."
            stream.stop_stream()
            stream.close()
            p.terminate()

            arduino.write('done')
            break
        if level == 2 and rms > baseline*5:
            # quiet
            print "ACTIVE. SCREAMED. DONE."
            stream.stop_stream()
            stream.close()
            p.terminate()

            arduino.write('done')
            break


    stream.stop_stream()
    stream.close()

    # close PyAudio (5)
    p.terminate()

    arduino.write('x')

ambientLevel = getAmbientAudio()


# open arduino stream
val = 0
full_val = ''
#From  http://www.olgapanades.com/blog/controlling-arduino-with-python/
arduino = serial.Serial('/dev/tty.usbmodemfa141', 9600)

def listenArduino():
    print 'listening on serial'
    while arduino.readline():
        #Reset variables
        val = 0
        full_val = ''
        #Pull the line
        try:
            val = arduino.readline()

            for char in val:
                #Make sure that the character isn't some weird thing
                if char.isalpha():
                    full_val = full_val + char
            print "Cleaned value is: " + full_val
            if full_val == 'activecomplete':
                print 'received activecomplete, now listening...'
                checkaudio(1, ambientLevel)
                return 1
            elif full_val == 'quietcomplete':
                print 'received quietcomplete, now listening...'
                checkaudio(2, ambientLevel)
                return 1
        except serial.serialutil.SerialException:
            pass


while 1:
    listenArduino()
    time.sleep(5)