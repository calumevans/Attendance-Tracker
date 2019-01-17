/*
for this demonstation we had 2 RFID tags
  -one was programmed with the name 'Michael and the other with 'Rhys'
  -The serial output and two different LEDs show the whereabouts of the student
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN          53
#define led1            49
#define led2            48
#define RST_PIN         5

int counter1 = 0;
int counter2 = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);                                  // Create MFRC522 instance

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.begin(9600);                                              // Initialize serial communications with the PC
  SPI.begin();                                                     // Init SPI bus
  mfrc522.PCD_Init();                                              // Initialize MFRC522 card
}

void loop() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  byte block;
  byte len;
  MFRC522::StatusCode status;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("\n **Card Detected:**"));
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card
 
  byte buffer1[18];
  block = 4;
  len = 18;

  //checking for first name written in the RFID tag
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    return;
  }

  //PRINT FIRST NAME
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }

  //increasing the counter, detecting if the rfid tag is Michael or Rhys
  if (buffer1[0] == 'M' ){
    counter1= counter1 +1;
  }else if(buffer1[0] == 'R'){
    counter2= counter2 +1;
  }

  //Changing the LED and serial monitor output depending on if Michael or Rhys is in the room
  if(counter1 % 2 == 1){                                      //looking for an odd number to detect the state
    digitalWrite(led1, HIGH);
    if(buffer1[0]!='R'){
         Serial.print(" has entered the room \n");
    }}
    else if(counter1 % 2 == 0 and buffer1[0]=='M'){           //looking for an even number to detect the state
    digitalWrite(led1, LOW);  
    Serial.print(" has left the room \n");
    }
   if(counter2 % 2 == 1 and buffer1[0] == 'R' ){
    digitalWrite(led2, HIGH);
      if(buffer1[0]!='M'){
         Serial.print(" has entered the room \n");
      }
    }
    else if(counter2 % 2 == 0 and buffer1[0]=='R'){
    digitalWrite(led2, LOW);
    Serial.print(" has left the room \n");
    }
  
  delay(500);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
