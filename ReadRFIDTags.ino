
/**************************************************************************/
/*! 
    @file     ReadRFIDTags.ino
    @author   Ray Luu,Yen Luu, Edward Loza
    Description:
This program will keep track of what time the patient take their medicines and when is the next time the patient supposes to take the medicine again.
Before taking the medicine, scan the bottle, one of the following will happen:
	The LED light turn GREEN
		It is okay to take the medicine.
	The LED light turn RED
		Cannot take the medicine, the time is not up yet.
	The LED light turn BLUE 
		First time the system sees the bottle
The bottle was scanned too fast.
The system cannot read the information on the bottle
Data Structure:
	Used Arrays to store all of the information to keep track of 
Number of bottle identified by UID of the RFID card
Time to retake pills
Number of pills
How frequent retaking the pills
Once there are no more pill in the bottle, the bottle will automatically 
get deleted from the array along with the information that associated with that bottle.
 
*/
/**************************************************************************/
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Arduino.h>
#include <Time.h>

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield
#define COMMON_ANODE

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

// Initialize arrays with 5 empty spaces 
// Store UID of the tag
String bottles_container[5];

// Store number of pill for each bottle
int bottle_value[5];

// Store the time
time_t time[5];

// Store time to retake pill
int time_retake_pill[5];

int redPin = 11;
int greenPin = 10;
int bluePin = 9;

int time_multipler;

void setup(void) {

  Serial.begin(115200);
  Serial.println("Hello!");
  Serial.println("I will help you keeping track of your medicines!");

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 
  
  setColor(0, 0, 0);   // setting colors to 0 turns the led off
  
  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if ( !versiondata ) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  // time_multipler = 60 is 1 minute 
  // time_multipler = 3600 is 1 hour 

  time_multipler = 3600;
  
  Serial.println("Please SCAN your bottle BEFORE taking your pill...");
}

void loop(void) {

  setColor(0, 0, 0);   // setting colors to 0 turns the led off
  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  String time_printout = " seconds!";
  
  if ( success ) {
    Serial.println("");
    
    Serial.print("The current time is: " );
    Serial.print( now() );
    Serial.println( time_printout );

    if( uidLength == 4 )
    {
      String scanned_bottle ;
      
      // add each value of the UID
      for( int i = 0; i < uidLength; i++ ) 
        scanned_bottle += uid[i];
      
      
      // get the size of the bottle container
      int num_bottles = sizeof( bottles_container ) / sizeof( String );

      // for each bottle in the bottle container
      for( int bottle = 0; bottle < num_bottles; bottle++ ) {
        
        // get the current bottle
         String current_bottle = bottles_container[ bottle ];
         
         // if the current bottle is in the bottle container
         if( current_bottle == scanned_bottle )  {
            // get the number of pill in the current bottle
            int current_bottle_num_pills = bottle_value[ bottle ];
            
            Serial.println("Welcome back! You are currently taking the medicines from this bottle!");
            Serial.print("The bottle has ");
            Serial.print(current_bottle_num_pills);
            Serial.println(" pills left"); 
            
            // if the number of pill is more than 0       
            if( current_bottle_num_pills > 0 ) {
              
              // Get the current time
              time_t current_time = now();
              
              int time_flexible;
              
              int time_retake = time_retake_pill[ bottle ];
              
              // if time retake is less than 4 hours
              // Then no time flexible is needed
              if( time_retake < 4  )
                time_flexible = 0;
              // if time retake is more than 4 but less than 12
              // Then the patience can take half hour before the time is up
              else if(time_retake >= 4 && time_retake < 12 )
                time_flexible = time_multipler << 1;
              // if time retake is more than 12 but less than 24
              // Then the patience can take 1 hour before the time is up              
              else if( time_retake >= 12 && time_retake < 24  )
                time_flexible = time_multipler;
               // if time retake is more than 24 
               // then can take 2 hours before
              else
                time_flexible = time_multipler >> 1;
              
              // if time is up
              if( current_time >= time[ bottle ] - time_flexible ) {  
                  // decrement the number of pill
                  int num_pill_left = current_bottle_num_pills - 1;
                  
                  Serial.println("Please TAKE one!");
                  Serial.print("Please come back in ");
                  Serial.print( time_retake_pill[ bottle ] );
                  Serial.println( time_printout ); 
                  
                  // after decrement the number of pill
                  // put the number of pill back
                  bottle_value[ bottle ] = num_pill_left;
                  
                  // Reset the timer
                  time[ bottle ] = current_time + time_retake_pill[ bottle ];
                  
                  // if the number of pill is down to 0
                  if( num_pill_left == 0 ) {
                    Serial.println("There is no more pills left after this!"); 
                    Serial.println("You are done with this bottle.");
                    Serial.println("Removing from the list...");
                    
                    // Remove the bottle from the bottle container
                    // shift all the bottle forward
                    for( int current_bottle_position = bottle; current_bottle_position < num_bottles; current_bottle_position++ )
                    {
                      if( current_bottle_position == num_bottles - 1 ){
                        bottles_container[ current_bottle_position ] = "";
                        bottle_value[ current_bottle_position ] = '\0';
                        time[ current_bottle_position ] = '\0';
                        time_retake_pill[ current_bottle_position ] = '\0';
                        break;
                      }
                      bottles_container[ current_bottle_position ] = bottles_container[ current_bottle_position + 1 ];
                      bottle_value[ current_bottle_position ] = bottle_value[ current_bottle_position + 1 ];
                      time[ current_bottle_position ] = time[ current_bottle_position + 1 ];
                      time_retake_pill[ current_bottle_position ] = time_retake_pill[ current_bottle_position + 1 ];
                    } 
                  }
                  setColor( 0, 255, 0 ); // green, take the pill
                  delay( 3000 );         // green LED for 3 seconds
              }
              else {
              // if time is not up
                Serial.println("YOU CANNOT TAKE THE PILL FOM THIS BOTTLE RIGHT NOW!"); 
                Serial.println("TIME IS NOT UP YET!");
                
                // Display red LED 
                setColor( 255, 0, 0 ); // red, don't take the pill
                delay( 3000 );         // red LED for 3 seconds
              }
            }
            break;
         }
         else {
         // if the scanned bottle != the current bottle  
        
           int num_of_pills;
           
           // if current bottle is null
           // The scanned bottle is not in the bottle container
           if( current_bottle == NULL ){
             
              // Use default key to access the information 
              uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
              
              // Accessing the key information using the default key
              success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
              
              // if successfully authenticated
              if (success)
              {
                uint8_t data[16];
                
                // Try to read the contents from the bottle
                success = nfc.mifareclassic_ReadDataBlock(4, data);
                
                // if successfully read the information from the bottle
                if (success)
                {  
                  // get the number of pill from the bottle  
                  num_of_pills = data[ 0 ];
                  time_retake_pill[ bottle ] = data[ 1 ] * time_multipler;        
                 
                  Serial.println("Welcome! This is to your new bottle!");
                  Serial.print("The bottle has ");
                  Serial.print( num_of_pills );
                  Serial.println(" pills");
                  
                  Serial.print("Time to retake the pills is: every ");
                  Serial.print( time_retake_pill[ bottle ] );
                  Serial.println( time_printout );
                  
                  Serial.println("Please TAKE one!");
                  
                  Serial.print("Please come back in ");
                  Serial.print( time_retake_pill[ bottle ] );
                  Serial.println( time_printout );
                  
                  // put the bottle in the bottle container
                  bottles_container[ bottle ] = scanned_bottle;
                  // decrement number of pill
                  bottle_value[ bottle ] = num_of_pills - 1;
                   
                  // Get the current time
                  time_t current_time = now();
                  
                  // set the timer
                  time[ bottle ] = current_time + time_retake_pill[ bottle ];
                  
                  // Display green LED to take the pill
                  setColor( 0, 255, 0 ); // green, take the pill
                  delay( 3000 );         // green LED for 3 seconds  
                }
                else {
                // if cannot read the information from the bottle
                // Display BLUE LED for 2 seconds               
                  setColor( 0, 0, 255 ); // blue, can't read info on the card
                  delay( 2000 );         // blue LED for 2 seconds
                }
              }
              else {
              // if cannot assess the bottle, the key might be invavlid              
                 setColor( 0, 0, 255 ); // blue, can't read info on the card
                 delay( 2000 );         // blue LED for 2 seconds   
              }           
            break;
           }
         }
      }
      
      Serial.println("");
    }
    
    // delay 1 second before for next scan
    delay(1000);
  }
}

void setColor(int red, int green, int blue){
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue); 
}

