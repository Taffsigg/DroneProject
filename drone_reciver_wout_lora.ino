#include "Nokia_5110.h"
#include <SFE_BMP180.h>
#include <HCSR04.h>
#include <NMEAGPS.h>


static NMEAGPS  gps; // This parses received characters
static gps_fix  fix; // This contains all the parsed pieces

//--------------------------
// CHECK CONFIGURATION

#if !defined(GPS_FIX_TIME) | !defined(GPS_FIX_DATE)
  #error You must define GPS_FIX_TIME and DATE in GPSfix_cfg.h!
#endif

#if !defined(NMEAGPS_PARSE_RMC) & !defined(NMEAGPS_PARSE_ZDA)
  #error You must define NMEAGPS_PARSE_RMC or ZDA in NMEAGPS_cfg.h!
#endif

#include <GPSport.h>


static const int32_t          zone_hours   = -5L; // EST
static const int32_t          zone_minutes =  0L; // usually zero
static const NeoGPS::clock_t  zone_offset  =
                                zone_hours   * NeoGPS::SECONDS_PER_HOUR +
                                zone_minutes * NeoGPS::SECONDS_PER_MINUTE;

// Uncomment one DST changeover rule, or define your own:
//#define USA_DST
#define EU_DST

#if defined(USA_DST)
  static const uint8_t springMonth =  3;
  static const uint8_t springDate  = 14; // latest 2nd Sunday
  static const uint8_t springHour  =  2;
  static const uint8_t fallMonth   = 11;
  static const uint8_t fallDate    =  7; // latest 1st Sunday
  static const uint8_t fallHour    =  2;
  #define CALCULATE_DST

#elif defined(EU_DST)
  static const uint8_t springMonth =  3;
  static const uint8_t springDate  = 31; // latest last Sunday
  static const uint8_t springHour  =  1;
  static const uint8_t fallMonth   = 10;
  static const uint8_t fallDate    = 31; // latest last Sunday
  static const uint8_t fallHour    =  1;
  #define CALCULATE_DST
#endif

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

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

int nRainIn = A1;
int nRainDigitalIn = 42;
int nRainVal;
boolean bIsRaining = false;
String strRaining;

UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);
UltraSonicDistanceSensor distanceSensor1(trigPin1, echoPin1);
UltraSonicDistanceSensor distanceSensor2(trigPin2, echoPin2);
UltraSonicDistanceSensor distanceSensor3(trigPin3, echoPin3);

const int buttonPin = 43;
int buttonState = 0; 
int lcdstate = 0;

double t1;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
pinMode(echoPin, INPUT);
    pinMode(trigPin1, OUTPUT);
pinMode(echoPin1, INPUT);
pinMode(trigPin2, OUTPUT);
pinMode(echoPin2, INPUT);
pinMode(trigPin3, OUTPUT);
pinMode(echoPin3, INPUT);
pinMode(nRainDigitalIn,INPUT);


Serial.print( F("NMEAtimezone.INO: started\n") );
  Serial.println( F("Looking for GPS device on " GPS_PORT_NAME ) );
  Serial.println( F("Local time") );
  gpsPort.begin( 9600 );
  #ifdef NMEAGPS_INTERRUPT_PROCESSING
    gpsPort.attachInterrupt( GPSisr );
  #endif

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

  lcd.setContrast(60);

 // lcd.setCursor(0, 0);
  //lcd.print("Clear methods");
}

void loop()
{

  GPSloop();
  buttonState = digitalRead(buttonPin);
  //Serial.println(buttonState);
  //Serial.println(lcdstate);


  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(LED_BUILTIN, HIGH);
    if(lcdstate != 1){
      lcdstate=1;
      lcd.clear();
    }else{
      lcdstate=0;
      lcd.clear();
    }
  } else {
    // turn LED off:
    digitalWrite(LED_BUILTIN, LOW);
  }

  nRainVal = analogRead(nRainIn);
  bIsRaining = !(digitalRead(nRainDigitalIn));
  
  if(bIsRaining){
    strRaining = "YES";
  }
  else{
    strRaining = "NO";
  }
  
 // Serial.print("Raining?: ");
  //Serial.print(strRaining);  
 // Serial.print("\t Moisture Level: ");
 // Serial.println(nRainVal);


      double a,P;
  
  // Get a new pressure reading:

  P = getPressure();

  // Show the relative altitude difference between
  // the new reading and the baseline reading:

  a = pressure.altitude(P,baseline);
 // Serial.println(P);
// Serial.print("relative altitude: ");
 //if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
 /// Serial.print(a,1);
 // Serial.println(" meters, ");

if(lcdstate == 1){
  lcd.setCursor(0, 0);
  lcd.print("CollisionSensors: ");
  lcd.setCursor(0, 1);
  lcd.print("S1: ");lcd.print(distanceSensor.measureDistanceCm());if(distanceSensor.measureDistanceCm() >= 1 && distanceSensor.measureDistanceCm() <= 6 && distanceSensor.measureDistanceCm() != -1 ){lcd.println(" ALERT     ");}else{lcd.println(" CM      ");}
   lcd.setCursor(0, 2);
  lcd.print("S2: ");lcd.print(distanceSensor1.measureDistanceCm());if(distanceSensor1.measureDistanceCm() >= 1 && distanceSensor1.measureDistanceCm() <= 6 && distanceSensor1.measureDistanceCm() != -1){lcd.println(" ALERT     ");}else{lcd.println(" CM      ");}
   lcd.setCursor(0, 3);
  lcd.print("S3: ");lcd.print(distanceSensor2.measureDistanceCm());if(distanceSensor2.measureDistanceCm() >= 1 && distanceSensor2.measureDistanceCm() <= 6 && distanceSensor2.measureDistanceCm() != -1){lcd.println(" ALERT     ");}else{lcd.println(" CM      ");}
   lcd.setCursor(0, 4);
  lcd.print("S4: ");lcd.print(distanceSensor3.measureDistanceCm());if(distanceSensor3.measureDistanceCm() >= 1 && distanceSensor3.measureDistanceCm() <= 6 && distanceSensor3.measureDistanceCm() != -1){lcd.println(" ALERT     ");}else{lcd.println(" CM      ");}
}else{
  lcd.setCursor(0,0);
   lcd.print("Time: ");lcd.print(fix.dateTime.hours);lcd.print(":");lcd.print(fix.dateTime.minutes);lcd.print(":");lcd.print(fix.dateTime.seconds);
  
  
   lcd.setCursor(0,1);
    lcd.print("Lat:  ");lcd.println(fix.latitude(), 6);
    lcd.setCursor(0,2);
    lcd.print("Long: ");lcd.println(fix.longitude(), 6);
    lcd.setCursor(0,3);
    lcd.print("Hum: ");lcd.print(nRainVal);lcd.print(" R: ");lcd.print(strRaining);lcd.print(" ");
    lcd.setCursor(0,4);
    lcd.print("Temp: ");lcd.print(t1,1);lcd.print(" *C "); 
    lcd.setCursor(0,5);
    lcd.print("Alt: ");lcd.print(a,1);lcd.println(" M  ");
}





}


void adjustTime( NeoGPS::time_t & dt )
{
  NeoGPS::clock_t seconds = dt; // convert date/time structure to seconds

  #ifdef CALCULATE_DST
    //  Calculate DST changeover times once per reset and year!
    static NeoGPS::time_t  changeover;
    static NeoGPS::clock_t springForward, fallBack;

    if ((springForward == 0) || (changeover.year != dt.year)) {

      //  Calculate the spring changeover time (seconds)
      changeover.year    = dt.year;
      changeover.month   = springMonth;
      changeover.date    = springDate;
      changeover.hours   = springHour;
      changeover.minutes = 0;
      changeover.seconds = 0;
      changeover.set_day();
      // Step back to a Sunday, if day != SUNDAY
      changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
      springForward = (NeoGPS::clock_t) changeover;

      //  Calculate the fall changeover time (seconds)
      changeover.month   = fallMonth;
      changeover.date    = fallDate;
      changeover.hours   = fallHour - 1; // to account for the "apparent" DST +1
      changeover.set_day();
      // Step back to a Sunday, if day != SUNDAY
      changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
      fallBack = (NeoGPS::clock_t) changeover;
    }
  #endif

  //  First, offset from UTC to the local timezone
  seconds += zone_offset;

  #ifdef CALCULATE_DST
    //  Then add an hour if DST is in effect
    if ((springForward <= seconds) && (seconds < fallBack))
      seconds += NeoGPS::SECONDS_PER_HOUR;
  #endif

  dt = seconds; // convert seconds back to a date/time structure

}

static void GPSloop()
{  
  while (gps.available( gpsPort )) {
    fix = gps.read();
    // Display the local time

    if (fix.valid.time && fix.valid.date) {
      adjustTime( fix.dateTime );

      Serial.println(fix.dateTime.date);

      //lcd << fix.dateTime;
     // Serial.println();
      
    Serial.print( F("Location: ") );
    if (fix.valid.location) {
      Serial.print( fix.latitude(), 6 );
      Serial.print( ',' );
      Serial.print( fix.longitude(), 6 );

    }

    Serial.print( F(", Altitude: ") );
    if (fix.valid.altitude)
      Serial.print( fix.altitude() );

    Serial.println();
    
    
    }
    Serial.println();
  }

  

} // GPSloop

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  static void GPSisr( uint8_t c )
  {
    gps.handle( c );
  }
#endif


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

    

      // Serial.print("temperature: ");
     // Serial.print(T,2);
     // Serial.println(" deg C, ");
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
