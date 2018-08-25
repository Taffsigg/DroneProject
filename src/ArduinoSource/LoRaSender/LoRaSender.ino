#include <SPI.h>
#include <LoRa.h>

String readString, servo1, servo2, servo3, servo4;
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  LoRa.setPins(9,7,2);
   LoRa.begin(433E6);
  while (!Serial);
  //Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {

  //String data_in = Serial.readString();

readString = Serial.readString();
  delay(1);
  
//while (Serial.available()) {
 //   delay(1);  
  //  if (Serial.available() >0) {
   //   char c = Serial.read();  //gets one byte from serial buffer
   //   readString += c; //makes the string readString
  //  } 
 // }
  if (readString.length() >=0) {

      // expect a string like 07002100 containing the two servo positions      
      servo1 = readString.substring(0, 4); //get the first four characters
      servo2 = readString.substring(5, 9); //get the next four characters 
      servo3 = readString.substring(10, 14); //get the next four characters 
      servo4 = readString.substring(15, 19); //get the next four characters 
     Serial.println("x: " + servo1 + " y: " + servo2 + " z: " + servo3 + " rz: " + servo4);
  }
  
  
//  Serial.println("Sending packet: ");
 // Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print( servo1 + " " + servo2 + " " + servo3 + " " + servo4);
  LoRa.endPacket();
  delay(20);
}


