import time
import serial
SYNC = 0x00 # Synchronization byte
ACK = 0x04 # Acknowledge byte
NACK = 0x02 # Not acknowledge byte
payloadLookup = [1, 2, 4, 8, 16, 32, 0, 0]
messageLength = 0
port = "/dev/ttyACM1"
message = [ n for n in range(0,34) ]
refreshTime = 0.10 # Rate to read data must be less than 300 mS
checkSumError = False
ser = serial.Serial(port, 2400, timeout =2 )
def main() :
	print "Sensor read"
	setup()
	mode = 0 # sensor mode to use
	changeMode(mode)
	print "running in sensor mode",mode
	lastRefresh = time.time()
	while True :
		if time.time() - lastRefresh > refreshTime : # send NACK and look at data
			ser.write(chr(NACK))
			lastRefresh = time.time()
			ser.flushInput() # remove old data
			getMessage() # get the data
			if not checkSumError :
				printMessage() # view data message
def setup():
	global ser
	print "initializing the sensor"
 	while not(message[0] == 0x90 and message[1] == 0x80 and checkSumError == False):
		getMessage()
	if ord(ser.read(1)) != ACK :
 		print("not got an ACK");
 	ser.write(chr(ACK)) # tell the sensor to go
	time.sleep(0.006) # give it time to finish sending the ACK
	ser.close()
	ser = serial.Serial(port, 57600, timeout =2 )

def getMessage() : # parse input stream into message
	global checkSumError, messageLength
	checkSum = 0xff
	command = ord(ser.read(1))
	if command == 0x00 or command == 0xff : # color sensor sometimes @@tathrows these
 		return 
	message[0] = command
	if (command & 0xC0) == 0 : # single byte
 		pass
 	else : # multibyte message
		checkSum ^= command;
 		payloadLength = payloadLookup[(command >> 3) & 7] # number bytes @@tain message
		if(command & 0xC0) == 0x80 :
 			payloadLength += 1 # info message has command byte following
 		for n in range(1, payloadLength + 1) : # read in the message up to the check sum
			message[n] = ord(ser.read(1))
			checkSum ^= message[n]
		message[payloadLength + 1] = ord(ser.read(1))
 		messageLength = payloadLength + 1
		if message[payloadLength + 1] != checkSum :
 			checkSumError = True # check sum error
 		else :
 			checkSumError = False # check sum fine
def printMessage() : # not the check sum
	print int(message[1]),
	print
def changeMode(newMode):
	if newMode <= 5 and newMode >= 0 :
		sendMessage(0x44, 0x11) # command write
		for n in range(0,3) :
			sendMessage(0x43, newMode & 0x7) # command mode
			ser.write(chr(NACK));
def sendMessage(cmd, data):
	cSum = 0xff ^ cmd ^ data;
	ser.write(chr(cmd))
	ser.write(chr(data))
	ser.write(chr(cSum))
if __name__ == '__main__':
	main()
