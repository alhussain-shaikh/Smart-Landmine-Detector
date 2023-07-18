#define BLYNK_TEMPLATE_ID "TMPLdohFq4_V"
#define BLYNK_DEVICE_NAME "WIFI CAR"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial

#define APP_DEBUG

#include <TinyGPS++.h>     
#include <SoftwareSerial.h>

#include "BlynkEdgent.h"
//Motor PINs
#define ENA D0
#define IN1 D1
#define IN2 D6
#define IN3 D3
#define IN4 D4
#define ENB D5

#define mdt A0

int metalDetect = 0 ; 


bool forward = 0;
bool backward = 0;
bool left = 0;
bool right = 0;
int Speed;


// GPS Variable Declaration

static const int RXPin = 3, TXPin = 1;   
static const uint32_t GPSBaud = 9600; 

float latitude = 1.11 ;
float longitude = 1.12 ;

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V5);  // V5 for virtual pin of Map Widget

SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device

BlynkTimer timer;

float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS



//unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1;       // fixed location for now

BLYNK_WRITE(V0) {
  forward = param.asInt();
}
 
BLYNK_WRITE(V1) {
  backward = param.asInt();
}
 
BLYNK_WRITE(V2) {
  left = param.asInt();
}
 
BLYNK_WRITE(V3) {
  right = param.asInt();
}
 
BLYNK_WRITE(V4) {
  Speed = param.asInt();
}


void smartcar() {
  if (forward == 1) {
    carforward();
    Serial.println("carforward");
  } else if (backward == 1) {
    carbackward();
    Serial.println("carbackward");
  } else if (left == 1) {
    carturnleft();
    Serial.println("carfleft");
  } else if (right == 1) {
    carturnright();
    Serial.println("carright");
  } else if (forward == 0 && backward == 0 && left == 0 && right == 0) {
    carStop();
    Serial.println("carstop");
  }

// metal Detector Code 

  mineDetector();
    
 

  
}

void setup()
{
  Serial.begin(9600);
 

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

 pinMode(mdt,INPUT_PULLUP);
 analogWrite(mdt,0);



 
// Gps 
  Serial.println();
  ss.begin(GPSBaud);

  BlynkEdgent.begin();
  timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
      Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
  }
}


void loop() {


  
  smartcar();

   while (ss.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read()))
        displayInfo();
  }
    BlynkEdgent.run();
  timer.run();
}

// Gps 

void displayInfo()
{ 

  if (gps.location.isValid() ) 
  {
    
     latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
     longitude = (gps.location.lng()); 
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
    Blynk.virtualWrite(V6, String(latitude, 6));   
    Blynk.virtualWrite(V7, String(longitude, 6));  
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    spd = gps.speed.kmph();               //get speed
       Blynk.virtualWrite(V8, spd);
       
       sats = gps.satellites.value();    //get number of satellites
       Blynk.virtualWrite(V9, sats);

       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
       Blynk.virtualWrite(V10, bearing);               
  }
  

  Serial.println();
}


// ROBOCAR

void carforward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carbackward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carturnleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carturnright() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void mineDetector(){
  metalDetect = analogRead(mdt);

if(metalDetect>100){
  Serial.println("Landmine Detected ");
  Blynk.virtualWrite(V12,HIGH);
  Blynk.logEvent("metal_detect","Lattitude : " + String(latitude,6) + "Longitude : " + String(longitude));
  carStop();
}else{
  Serial.println("NO Landmine Present");
  Blynk.virtualWrite(V12,LOW);
}

delay(10);
}
