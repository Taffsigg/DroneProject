/*  
 * Check:  http://www.electronoobs.com/eng_robotica_tut5_2_1.php
 * 
 * 
A basic receiver test for the nRF24L01 module to receive 6 channels send a ppm sum
with all of them on digital pin D2.
Install NRF24 library before you compile
Please, like, share and subscribe on my https://www.youtube.com/c/ELECTRONOOBS
 */
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/


#include <SPI.h>
#include <LoRa.h>
////////////////////// PPM CONFIGURATION//////////////////////////
#define channel_number 4  //set the number of channels
//#define sigPin 3  //set PPM signal output pin on the arduino
//#define PPM_FrLen 27000  //set the PPM frame length in microseconds (1ms = 1000Âµs)
//#define PPM_PulseLen 400  //set the pulse length
//////////////////////////////////////////////////////////////////


//////////////////////CONFIGURATION///////////////////////////////
//#define CHANNEL_DEFAULT_VALUE 1500  //set the default servo value
#define FRAME_LENGTH 27000  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
//#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 3  //set PPM signal output pin on the arduino

static const int RXPin = 8, TXPin = 10;
static const uint32_t GPSBaud = 9600;

#define cell1_pin A0
#define cell2_pin A1
#define cell3_pin A2


#define relay_pin 5

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

int ppm[channel_number];
int t, y, p, r;

String incoming , servo1, servo2, servo3, servo4;

void setPPMValuesFromData() {
  ppm[0] = r;
  ppm[1] = p;
  ppm[2] = t;
  ppm[3] = y;
}

void resetData() {
  // 'safe' values to use when no radio input is detected
  t = 1500;
  y = 1500;
  p = 1500;
  r = 1500;
  setPPMValuesFromData();
}


void recvData() {  
int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
   // Serial.print("Received packet '");
    // read packet
    String incoming = "";
    while (LoRa.available()) {
    //  Serial.print((char)LoRa.read());
      incoming += (char)LoRa.read();
  
     // incoming += (char)LoRa.read(); //makes the string incoming
    }
  // Serial.println(incoming);
    //Serial.print("\n");
   // Serial.println("\n");
    

    // print RSSI of packet
    //Serial.print("' with RSSI ");
    //Serial.println(LoRa.packetRssi());

      if (incoming.length() >=0) {

      // expect a string like 07002100 containing the two servo positions      
      servo1 = incoming.substring(0, 4); //get the first four characters
      servo2 = incoming.substring(5, 9); //get the next four characters 
      servo3 = incoming.substring(10, 14); //get the next four characters 
      servo4 = incoming.substring(15, 19); //get the next four characters 
      
      int n1;  
      int n2;
      int n3;
      int n4;
      
      char carray1[6]; //magic needed to convert string to a number 
      servo1.toCharArray(carray1, sizeof(carray1));
      n1 = atoi(carray1); 
      
      char carray2[6];
      servo2.toCharArray(carray2, sizeof(carray2));
      n2 = atoi(carray2); 

      char carray3[6];
      servo3.toCharArray(carray3, sizeof(carray3));
      n3 = atoi(carray3);
       
      char carray4[6];
      servo4.toCharArray(carray4, sizeof(carray4));
      n4 = atoi(carray4); 

    
      r = n1;
      p = n2;
      t = n3;
      y = n4;

        Serial.print(t);
      Serial.print(" ");
      Serial.print(y);
      Serial.print(" ");
      Serial.print(p);
      Serial.print(" ");
      Serial.print(r);
     Serial.print("\n");
     

 // This sketch displays information every time a new sentence is correctly encoded.

      displayInfo();


     
  }
}

}
void setupPPM() {
 
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, 0);  //set the PPM signal pin to the default state (off)
  
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

}


 void printVolts()
{
  int sensorValue1 = analogRead(cell1_pin); //read the A0 pin value
  float voltage1 = sensorValue1 * (5.00 / 1023.00) * 2; //convert the value to a true voltage.

 // Serial.print("voltage1 = ");
 // Serial.print(voltage1 - 4.6); //print the voltage to Serial
 // Serial.print(" V ");

  int sensorValue2 = analogRead(cell2_pin); //read the A0 pin value
  float voltage2 = sensorValue2 * (5.00 / 1023.00) * 2; //convert the value to a true voltage.

//  Serial.print("voltage2 = ");
//  Serial.print(voltage2 - 4.6); //print the voltage to Serial
 // Serial.print(" V ");

  int sensorValue3 = analogRead(cell3_pin); //read the A0 pin value
  float voltage3 = sensorValue3 * (5.00 / 1023.00) * 2; //convert the value to a true voltage.

 // Serial.print("voltage3 = ");
 // Serial.print(voltage3 - 4.6); //print the voltage to Serial
//  Serial.print(" V ");

 // Serial.print(voltage3 - 4.6 + voltage2 - 4.6 + voltage1 - 4.6);

  //Serial.print("\n");

  if(voltage3 - 4.6 + voltage2 - 4.6 + voltage1 - 4.6 <= 10.8){
     digitalWrite(5, LOW);
  }else{
     digitalWrite(5, HIGH);
  }
  
}

void displayInfo()
{

    if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  
   while (ss.available() > 0)
    gps.encode(ss.read());
    
  if (gps.location.isUpdated())
  {
//    lcd.clear();
    Serial.print(F("LOCATION   Fix Age="));
    Serial.print(gps.location.age());
    Serial.print(F("ms Raw Lat="));
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.print(gps.location.rawLat().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLat().billionths);
    Serial.print(F(" billionths],  Raw Long="));
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.print(gps.location.rawLng().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLng().billionths);
    Serial.print(F(" billionths],  Lat="));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" Long="));
    Serial.println(gps.location.lng(), 6);
  }

  else if (gps.date.isUpdated())
  {
    Serial.print(F("DATE       Fix Age="));
    Serial.print(gps.date.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.date.value());
    Serial.print(F(" Year="));
    Serial.print(gps.date.year());
    Serial.print(F(" Month="));
    Serial.print(gps.date.month());
    Serial.print(F(" Day="));
    Serial.println(gps.date.day());
  }

  else if (gps.time.isUpdated())
  {
    Serial.print(F("TIME       Fix Age="));
    Serial.print(gps.time.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.time.value());
    Serial.print(F(" Hour="));
    Serial.print(gps.time.hour());
    Serial.print(F(" Minute="));
    Serial.print(gps.time.minute());
    Serial.print(F(" Second="));
    Serial.print(gps.time.second());
    Serial.print(F(" Hundredths="));
    Serial.println(gps.time.centisecond());
  }
}

void setup() {  
Serial.begin(9600);
  ss.begin(GPSBaud);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  LoRa.setPins(9,7,4);
  LoRa.begin(433E6);
  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
    while (1);
  }else{
    digitalWrite(2, HIGH);
    digitalWrite(5, LOW);
  }
  
  resetData();
  setupPPM();
}

void loop() {
  recvData();
  setPPMValuesFromData();
  printVolts();
  displayInfo();
}




/**************************************************/


#define clockMultiplier 1 // set this to 2 if you are using a 16MHz arduino, leave as 1 for an 8MHz arduino

ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if (state) {  //start pulse
    digitalWrite(sigPin, 1);
    OCR1A = PULSE_LENGTH * 2;
    state = false;
  } else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, 0);
    state = true;

    if(cur_chan_numb >= channel_number){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;// 
      OCR1A = (FRAME_LENGTH - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PULSE_LENGTH) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
/**************************************************/


