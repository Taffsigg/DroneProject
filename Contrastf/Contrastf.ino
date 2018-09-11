#include "Nokia_5110.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Ultrasonic.h>

int trigPin = 30;
int echoPin = 31;

int trigPin1 = 32;
int echoPin1 = 33;

int trigPin2 = 34;
int echoPin2 = 35;

int trigPin3 = 36;
int echoPin3 = 37;

Ultrasonic ultrasonic(trigPin,echoPin); // (Trig PIN,Echo PIN)

Ultrasonic ultrasonic1(trigPin1,echoPin1); // (Trig PIN,Echo PIN)

Ultrasonic ultrasonic2(trigPin2,echoPin2); // (Trig PIN,Echo PIN)

Ultrasonic ultrasonic3(trigPin3,echoPin3); // (Trig PIN,Echo PIN)



long duration,duration1,duration2,duration3;
int distanceCm,distanceCm1,distanceCm2,distanceCm3;

static const int RXPin = 53, TXPin = 52;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

double baseline; // baseline pressure


void setup() {
  Serial.begin(9600);
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


  
}

void loop() {


  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

//  Serial.println(ultrasonic.Ranging(CM)); // CM or INC
  // Serial.println(ultrasonic1.Ranging(CM)); // CM or INC
  //  Serial.println(ultrasonic2.Ranging(CM)); // CM or INC
//     Serial.println(ultrasonic3.Ranging(CM)); // CM or INC
  ///Serial.println(" cm" );



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

    while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if(ultrasonic.Ranging(CM) < 5.0 && ultrasonic.Ranging(CM) > 4.0){
     Serial.println("S1 COLLIISION!" ); 
     Serial.println(ultrasonic.Ranging(CM) ); 
  }if(ultrasonic1.Ranging(CM) < 5.0&& ultrasonic1.Ranging(CM) > 4.0){
    Serial.println("S2 COLLIISION!" );
         Serial.println(ultrasonic1.Ranging(CM) ); 
  }if(ultrasonic2.Ranging(CM) < 5.0&& ultrasonic2.Ranging(CM) > 4.0) {
    Serial.println("S3 COLLIISION!" );
         Serial.println(ultrasonic2.Ranging(CM) ); 
  }if(ultrasonic3.Ranging(CM) < 5.0&& ultrasonic3.Ranging(CM) > 4.0){
    Serial.println("S4 COLLIISION!" );
         Serial.println(ultrasonic3.Ranging(CM) ); 
  }
  
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
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
