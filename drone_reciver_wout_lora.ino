#include "Nokia_5110.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <HCSR04.h>

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 52, TXPin = 53;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

SFE_BMP180 pressure;

double baseline; // baseline pressure

int trigPin = 50;
int echoPin = 51;

int trigPin1 = 48;
int echoPin1 = 49;

int trigPin2 = 46;
int echoPin2 = 47;

int trigPin3 = 44;
int echoPin3 = 45;

#define RST 2
#define CE 3
#define DC 4
#define DIN 5
#define CLK 6

Nokia_5110 lcd = Nokia_5110(RST, CE, DC, DIN, CLK);

UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);
UltraSonicDistanceSensor distanceSensor1(trigPin1, echoPin1);
UltraSonicDistanceSensor distanceSensor2(trigPin2, echoPin2);
UltraSonicDistanceSensor distanceSensor3(trigPin3, echoPin3);

double t1;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

    pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
    pinMode(trigPin1, OUTPUT);
pinMode(echoPin1, INPUT);
pinMode(trigPin2, OUTPUT);
pinMode(echoPin2, INPUT);
pinMode(trigPin3, OUTPUT);
pinMode(echoPin3, INPUT);

   if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }

  // Get the baseline pressure:
  
  baseline = getPressure();
  
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  

  lcd.setContrast(32);

 // lcd.setCursor(0, 0);
  //lcd.print("Clear methods");
}

void loop()
{
delay(250);


      double a,P;
  
  // Get a new pressure reading:

  P = getPressure();

  // Show the relative altitude difference between
  // the new reading and the baseline reading:

  a = pressure.altitude(P,baseline);
  
  Serial.print("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.println(" meters, ");



  lcd.setCursor(0,5);
  lcd.print("T:");lcd.print(t1,1);lcd.print("*C "); lcd.print("A:");lcd.print(a,1);lcd.println("M  ");


  lcd.setCursor(0, 1);
  lcd.print("S1: ");lcd.print(distanceSensor.measureDistanceCm());if(distanceSensor.measureDistanceCm() > 0 && distanceSensor.measureDistanceCm() < 6 && distanceSensor.measureDistanceCm() != -1 ){lcd.println(" ALERT     ");}else{lcd.println(" CM     ");}
   lcd.setCursor(0, 2);
  lcd.print("S2: ");lcd.print(distanceSensor1.measureDistanceCm());if(distanceSensor1.measureDistanceCm() > 0 && distanceSensor1.measureDistanceCm() < 6 && distanceSensor1.measureDistanceCm() != -1){lcd.println(" ALERT     ");}else{lcd.println(" CM     ");}
   lcd.setCursor(0, 3);
  lcd.print("S3: ");lcd.print(distanceSensor2.measureDistanceCm());if(distanceSensor2.measureDistanceCm() > 0 && distanceSensor2.measureDistanceCm() < 6 && distanceSensor2.measureDistanceCm() != -1){lcd.println(" ALERT     ");}else{lcd.println(" CM     ");}
   lcd.setCursor(0, 4);
  lcd.print("S4: ");lcd.print(distanceSensor3.measureDistanceCm());if(distanceSensor3.measureDistanceCm() > 0 && distanceSensor3.measureDistanceCm() < 6 && distanceSensor3.measureDistanceCm() != -1){lcd.println(" ALERT     ");}else{lcd.println(" CM     ");}
  



    while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
  
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());

    lcd.setCursor(0,0);
    lcd.print("  ");lcd.print(gps.time.hour()); lcd.print(":"); lcd.print(gps.time.minute());lcd.print(" / "); lcd.print(gps.date.month()); lcd.print("/"); lcd.print(gps.date.day()); lcd.print("/");lcd.println(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

double getPressure()
{
  char status;
  double T,P,p0,a;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {

    

       Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.println(" deg C, ");
      t1 = T;

      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
