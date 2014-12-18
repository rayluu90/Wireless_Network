{\rtf1\ansi\ansicpg1252\cocoartf1343\cocoasubrtf140
{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
\margl1440\margr1440\vieww14500\viewh8400\viewkind0
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural

\f0\fs24 \cf0 RFID-based Medicine Timer System \
Developed by Ray Luu, Yen Luu, Edward Loza\
Main file: readRFIDTags.ino\
\
How to run the program:\
	Install Arduino IDE \
	Load the Adafruit_NFCShield_I2C and Time into the Arduino library\
	Load the readRFIDTags.ino on to the sketch\
	Compile and upload the sketch on to the Arduino Uno R3 board \
	Open the serial monitor and wait for the output.\
 \
Description:\
This program will keep track of what time the patient take their medicines and when is the next time the patient supposes to take the medicine again.\
Before taking the medicine, scan the bottle, one of the following will happen:\
	The LED light turns GREEN\
		It is okay to take the medicine.\
	The LED light turns RED\
		Cannot take the medicine, the time is not up yet.\
	The LED light turns BLUE \
		First time the system sees the bottle\
The bottle was scanned too fast.\
The system cannot read the information on the bottle\
\
Data Structure:\
	Used Arrays to store all of the information to keep track of \
Number of bottle identified by UID of the RFID card\
Time to retake pills\
Number of pills\
How frequent retaking the pills\
Once there are no more pill in the bottle, the bottle will automatically \
get deleted from the array along with the information that associated with that bottle.\
\
}