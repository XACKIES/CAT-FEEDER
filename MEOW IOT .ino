#define BLYNK_TEMPLATE_ID "TMPLluKcGbjn"
  #define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN  "OxTugAzpqokar0kiqYX1UJ9xEj3uYce4"
#define BLYNK_PRINT Serial
#define TRIG_I     26
#define ECHO_I     27
#define TRIG_II    12
#define ECHO_II    14
#define TRIG_III   25
#define ECHO_III   33
#define LED        2
#define RF         5
#define PUMP_A     18
#define PUMP_B     19
#define PINSERVO   4

#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <ESP32Servo.h>

int DISTANEC_I   ;
int duration_I   ;
int DISTANEC_II  ;
int duration_II  ;
int DISTANEC_III ;
int duration_III ;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "ERROR 404";
char pass[] = "kittiphop";

long TIMEINPUT;
long rtc_sec;

//Setting Value
bool i = true;
int ST_TIME    =       120;  //Set period MODE TIMER work
int SELECT     =       1;    //START MODE
int setzero    =     175;    //SET SERVO
int setworking =     120;    //SET SERVO
int WTT        =      23;    // DISH WATER

BlynkTimer timer;
BlynkTimer SENSOR_I;
BlynkTimer SENSOR_II;
BlynkTimer SENSOR_III;
BlynkTimer SENSOR_IV;
WidgetRTC rtc;
Servo SERVO;

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  SERVO.attach(PINSERVO);
  timer.setInterval     (1000L  ,      clockDisplay       );
  SENSOR_I.setInterval  (1000L ,    SENSOR_ULTRASONIC_I   );
  SENSOR_II.setInterval (1000L ,    SENSOR_ULTRASONIC_II  );
  SENSOR_III.setInterval(1000L ,    SENSOR_ULTRASONIC_III );
}
void loop()
{
  Blynk.run();
  timer.run();
  SENSOR_I.run();   ///water amount
  SENSOR_II.run();  ///dish water
  SENSOR_III.run(); ///dish amount
}

BLYNK_CONNECTED()
{
  rtc.begin();
  Blynk.virtualWrite(V11, "   MEOW FEEDER  ");
}

BLYNK_WRITE(V3)
{
  int value = param.asInt();
  SERVO.write(setzero);
  PUMP(0);
  if ( value == 1)
  {
    SERVO.write(setzero);
    SELECT = 1;
    Blynk.virtualWrite(V3, value);
    Blynk.virtualWrite(V8, LOW  );
    Blynk.virtualWrite(V9, LOW  );
    Serial.println(SELECT);
    SELECTOR();
  }
  if ( value == 0)
  {
    Blynk.virtualWrite(V3, value);
    SELECTOR();
  }

}
BLYNK_WRITE(V8)
{
  int value = param.asInt();
  SERVO.write(setzero);
  PUMP(0);
  if ( value == 1 )
  {
    SELECT = 2;
    Blynk.virtualWrite(V8, value);
    Blynk.virtualWrite(V3, LOW  );
    Blynk.virtualWrite(V9, LOW  );
    Serial.println(SELECT);
    SELECTOR();

  }
  if ( value == 0)
  {
    Blynk.virtualWrite(V8, value);
    SELECTOR();
  }

}
BLYNK_WRITE(V9)
{
  int value = param.asInt();
  SERVO.write(setzero);
  PUMP(0);
  if ( value == 1)
  {
    SELECT = 3;
    Blynk.virtualWrite(V9, value);
    Blynk.virtualWrite(V3, LOW  );
    Blynk.virtualWrite(V8, LOW  );
    Serial.println(SELECT);
    SELECTOR();
  }
  if ( value == 0 )
  {
    Blynk.virtualWrite(V9, value);
    SELECTOR();
  }
}

BLYNK_WRITE(V0)// SW FOOD
{
  int value = param.asInt();

  if ( SELECT == 1 && value == 1)
  {
    Blynk.virtualWrite(V0, value);
    SERVO.write(setworking);
    Serial.println("SERVO :");
    Serial.println(value);
    digitalWrite(LED, HIGH);      //LED CHECK LOGIC
  }

  else if ( SELECT == 1 && value == 0)
  {
    Blynk.virtualWrite(V0, value);
    SERVO.write(setzero);
    Serial.println("SERVO :");
    Serial.println(value);
    digitalWrite(LED, LOW);      //LED CHECK LOGIC
  }
  else
  {
    Blynk.virtualWrite(V0, 0);
  }
}
BLYNK_WRITE(V1)
{
  int value = param.asInt();

  if ( SELECT == 1 && value == 1)
  {
    Blynk.virtualWrite(V1, value);
    PUMP(value);
    Serial.println("PUMP :");
    Serial.println(value);
    digitalWrite(LED, HIGH);      //LED CHECK LOGIC
  }
  else if ( SELECT == 1 && value == 0)
  {
    Blynk.virtualWrite(V1, value);
    PUMP(value);
    Serial.println("PUMP :");
    Serial.println(value);
    digitalWrite(LED, LOW);      //LED CHECK LOGIC
  }
  else
  {
    Blynk.virtualWrite(V1, 0);
  }
}
void clockDisplay()
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  rtc_sec = (hour() * 60 * 60) + (minute() * 60) + second();
  TIME_WORKNIG();
  SELECTOR();
}
BLYNK_WRITE(V7)
{
  SELECTOR();
  if ( SELECT == 3)
  {
    long startTimeInSecs = param[0].asLong();
    TIMEINPUT = startTimeInSecs;
    Serial.print("TIME INPUT : ");
    Serial.print(TIMEINPUT);
    Serial.println();
    TIME_WORKNIG();
  }
}
void TIME_WORKNIG()
{
  IO();
  if (rtc_sec - TIMEINPUT  <= ST_TIME  && SELECT == 3 && rtc_sec - TIMEINPUT  >= 1 )
  {
    digitalWrite(LED, HIGH);
    AUTO();
    Serial.println("LED : HIGH");      //LED CHECK LOGIC
  }
  if ((rtc_sec - TIMEINPUT  > ST_TIME  && SELECT == 3) || rtc_sec < TIMEINPUT )
  {
    digitalWrite(LED,  LOW);
    if (digitalRead(RF) == LOW )
    {
      Blynk.virtualWrite(V6, "DISH FOOD: HIGH ");
      SERVO.write(setzero);
    }
    else if (digitalRead(RF) == HIGH)
    {
      Blynk.virtualWrite(V6, "DISH FOOD: LOW");
      SERVO.write(setzero);
    }
    if      (DISTANEC_II > WTT )
    {
      Blynk.virtualWrite(V4, "DISH WATER: LOW ");
      PUMP(0);
    }
    else if (  DISTANEC_II <= WTT )
    {
      Blynk.virtualWrite(V4, "DISH WATER: HIGH");
      PUMP(0);
    }
    Serial.println("LED : LOW");      //LED CHECK LOGIC
  }
}
void IO()
{
  pinMode(TRIG_I   , OUTPUT);
  pinMode(ECHO_I   ,  INPUT);
  pinMode(TRIG_II  , OUTPUT);
  pinMode(ECHO_II  ,  INPUT);
  pinMode(TRIG_III , OUTPUT);
  pinMode(ECHO_III ,  INPUT);
  pinMode(RF       ,  INPUT);
  pinMode(LED      , OUTPUT);
  pinMode(PUMP_A  , OUTPUT);
  pinMode(PUMP_B  , OUTPUT);
  SERVO.attach(PINSERVO);

}
void SENSOR_ULTRASONIC_I()///water amount
{
  IO();
  digitalWrite(TRIG_I, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_I, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_I, LOW);
  duration_I = pulseIn(ECHO_I, HIGH);
  DISTANEC_I = duration_I * 0.034 / 2;
  Serial.print("DISTANEC_I: ");
  Serial.println(DISTANEC_I);
  int kk = map(DISTANEC_I, 19, 13, 0, 100);
  Blynk.virtualWrite(V5, kk );
}
void SENSOR_ULTRASONIC_II() // water tray
{
  IO();
  digitalWrite(TRIG_II, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_II, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_II, LOW);
  duration_II = pulseIn(ECHO_II, HIGH);
  DISTANEC_II = duration_II * 0.034 / 2;
  //Serial.print("DISTANEC_II:");Serial.println(DISTANEC_II);
  // int kk = map(DISTANEC_III,20,13,0,100);
}
void SENSOR_ULTRASONIC_III() // water tray
{
  IO();
  digitalWrite(TRIG_III, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_III, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_III, LOW);
  duration_III = pulseIn(ECHO_III, HIGH);
  DISTANEC_III = duration_III * 0.034 / 2;
  //Serial.print("DISTANEC_III: ");Serial.println(DISTANEC_III);
  int k = map(DISTANEC_III, 16, 6, 0, 100); Blynk.virtualWrite(V2, k );
}
void AUTO()
{
  IO();
  if (digitalRead(RF) == LOW )
  {
    Blynk.virtualWrite(V6, "DISH FOOD: HIGH ");
    SERVO.write(setzero);
  }
  else if (digitalRead(RF) == HIGH)
  {
    Blynk.virtualWrite(V6, "DISH FOOD: LOW");
    SERVO.write(setworking);
  }

  if      (DISTANEC_II > WTT )
  {
    Blynk.virtualWrite(V4, "DISH WATER: LOW ");
    PUMP(1);
  }
  else if (  DISTANEC_II <= WTT )
  {
    Blynk.virtualWrite(V4, "DISH WATER: HIGH");
    PUMP(0);
  }
}

void MANUAL()
{
  IO();
  if (digitalRead(RF) == LOW )
  {
    Blynk.virtualWrite(V6, "DISH FOOD: HIGH ");
  }
  else if (digitalRead(RF) == HIGH)
  {
    Blynk.virtualWrite(V6, "DISH FOOD: LOW");
  }

  if      (DISTANEC_II > WTT )
  {
    Blynk.virtualWrite(V4, "DISH WATER: LOW ");
  }
  else if (  DISTANEC_II <= WTT )
  {
    Blynk.virtualWrite(V4, "DISH WATER: HIGH");
  }
}
void PUMP(int u)
{
  IO();
  if (u == 1)
  {
    digitalWrite(PUMP_A, HIGH);
    digitalWrite(PUMP_B, LOW);
  }
  if (u == 0)
  {
    digitalWrite(PUMP_A, LOW);
    digitalWrite(PUMP_B, LOW);
  }
}
void SELECTOR()
{
  IO();
  if (SELECT == 1)
  {
    Blynk.virtualWrite(V10, "MODE: MANNUAL");
    MANUAL();
  }
  if (SELECT == 2)
  {
    Blynk.virtualWrite(V10, "MODE:   AUTO ");
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    AUTO();
  }
  if (SELECT == 3)
  {
    Blynk.virtualWrite(V10, "MODE:   TIMER ");
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
  }
}
