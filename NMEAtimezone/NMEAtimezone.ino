#include <NMEAGPS.h>
#include <SFE_BMP180.h>

static NMEAGPS  gps; // This parses received characters
static gps_fix  fix; // This contains all the parsed pieces

int gps_status = -1;

//--------------------------
// CHECK CONFIGURATION

#if !defined(GPS_FIX_TIME) | !defined(GPS_FIX_DATE)
  #error You must define GPS_FIX_TIME and DATE in GPSfix_cfg.h!
#endif

#if !defined(NMEAGPS_PARSE_RMC) & !defined(NMEAGPS_PARSE_ZDA)
  #error You must define NMEAGPS_PARSE_RMC or ZDA in NMEAGPS_cfg.h!
#endif

#include <GPSport.h>

//--------------------------
// Set these values to the offset of your timezone from GMT

static const int32_t          zone_hours   = +2L; // EST
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

//--------------------------

SFE_BMP180 pressure;
double baseline; // baseline pressure
double t1;

int nRainIn = A1;
int nRainDigitalIn = 42;
int nRainVal;
boolean bIsRaining = false;
String strRaining;


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

} // adjustTime

//--------------------------

void ReadSensors(){
  nRainVal = analogRead(nRainIn);
  bIsRaining = !(digitalRead(nRainDigitalIn));
  
  if(bIsRaining){
    strRaining = "YES";
  }
  else{
    strRaining = "NO";
  }
  Serial.println("Humidity: ");
  Serial.print("Raining?: ");
  Serial.print(strRaining);  
 Serial.print("\t Moisture Level: ");
  Serial.println(nRainVal);
 Serial.println("");

      double a,P;
  
  // Get a new pressure reading:
Serial.println("Pressure and Temperature: ");
  P = getPressure();
  a = pressure.altitude(P,baseline);
  Serial.println(P);
 Serial.print("Relative altitude: ");
 if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.println(" Meters, ");

   Serial.print("Temperature: ");
   Serial.print(t1);
  Serial.println(" *C ");
  
}

static void GPSloop()
{  
  while (gps.available( gpsPort )) {
    fix = gps.read();
    // Display the local time

    if (fix.valid.time && fix.valid.date) {
      digitalWrite(LED_BUILTIN, HIGH);
      adjustTime( fix.dateTime );
      Serial.println("Date | Time | Location: ");
      Serial << fix.dateTime;
      Serial.println();
      Serial.println("");
      Serial.print("Heading: ");
      Serial.print(fix.heading());
      Serial.println();
    }

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
    Serial.println("");
    ReadSensors();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("-------------------------------------------------------------------------------");
  }

} // GPSloop

//--------------------------

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  static void GPSisr( uint8_t c )
  {
    gps.handle( c );
  }
#endif

//--------------------------

void setup()
{
  Serial.begin(115200);
  Serial.print( F("NMEAtimezone.INO: started\n") );
  Serial.println( F("Looking for GPS device on " GPS_PORT_NAME ) );
  Serial.println( F("Local time") );
  pinMode(LED_BUILTIN, OUTPUT);
  
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
}

//--------------------------

void loop()
{
  checkGPS_Status();
  GPSloop();
}

void checkGPS_Status(){
  if(gps_status != -1){
    if(gps_status != fix.status){
      gps_status = fix.status;
        if(fix.status == fix.STATUS_NONE){
          Serial.println("None");
        }else if(fix.status == fix.STATUS_EST){
          Serial.println("Est");
        }else if(fix.status == fix.STATUS_TIME_ONLY){
          Serial.println("Time Only");
        }else if(fix.status == fix.STATUS_STD){
          Serial.println("Std");
        }else if(fix.status == fix.STATUS_DGPS){
          Serial.println("DGPS");
        }
  }
  }else{
    gps_status = fix.status;
    if(fix.status == fix.STATUS_NONE){
          Serial.println("None");
        }else if(fix.status == fix.STATUS_EST){
          Serial.println("Est");
        }else if(fix.status == fix.STATUS_TIME_ONLY){
          Serial.println("Time Only");
        }else if(fix.status == fix.STATUS_STD){
          Serial.println("Std");
        }else if(fix.status == fix.STATUS_DGPS){
          Serial.println("DGPS");
        }
  }
}

double getPressure()
{
  char status;
  double T,P,p0,a;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      t1 = T;
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
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
