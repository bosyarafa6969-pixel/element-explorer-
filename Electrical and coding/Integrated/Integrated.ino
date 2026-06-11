#include <SoftwareSerial.h>
#include <Servo.h>

// --------------------- Pin Configration ------------------------------
#define liftServo_pin 9
#define gateIr 4

#define in1 7
#define in2 8
#define motorPin 3
#define ENCODER_A 2 // Encoder channel A (interrupt pin recommended)
#define ENCODER_B A0 // Encoder channel B (optional)

#define sel 12

// --------------------- Serial Pins Configration ------------------------------
SoftwareSerial bluetooth(10, 11);// ( RX , TX )
SoftwareSerial mySerial(5, 6);// ( RX , TX )

// --------------------- Constants Configration ------------------------------
#define upAngle 90
#define downAngle 0
#define PPR 374 // Pulses Per Revolution

#define E_a 50


// --------------------- Global Varialble ------------------------------
Servo liftServo;
volatile int input = 0;
int liftangle = 0;
///         spdR,spd1,spd2
int spd[3]={70 ,150 ,200}; 
///       JR,J1,J2
int J[3]={0 ,10,87};
volatile long pulseCount = 0; /// pulse counter for on rotation
volatile long totalCount = 0; /// pulse counter for correcting the accumelated error
volatile bool done = false;


// ================= INTERRUPT =================
void countPulse() 
{  
  pulseCount++;
  totalCount++;
  if (!done && pulseCount >= (PPR - J[input])) 
  {
    pulseCount=0;
    done = true;
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    analogWrite(motorPin, 0);
  }
}

void accmltdE()
{
  volatile long E = totalCount%PPR;
  Serial.println("Acculemlated Error = " + E); 
  if(E>=E_a)
  {
    Serial.println("Correcting Acculemlated Error...."); 
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    done = false;
    input = 0;
    pulseCount = E;
    analogWrite(motorPin, spd[0]);
    while(!done)
    {
      Serial.print("Pulses: ");
      Serial.println(pulseCount);
    }
  }
}

void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), countPulse, RISING);
  pulseCount = 0;
  totalCount = 0;
  done = false;

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(motorPin, 0); 

  pinMode(sel, OUTPUT);
  pinMode(liftServo_pin, OUTPUT);
  pinMode(gateIr, INPUT);
  liftServo.attach(liftServo_pin);
  liftServo.write(downAngle);
  
  Serial.begin(9600); 
  bluetooth.begin(9600);
  mySerial.begin(9600);
  Serial.println("Bleutooth Online. Connect via Phone App.");
  //delay(30000);
}

void loop() {

  // --------------------- Lifting Mechanism ------------------------------
 
  /*---------------------------------------*/ liftangle=0;///for major_task/*---------------------------------------*/
  bool ir = digitalRead(gateIr);
  if( !ir && liftangle == 0)
  {
    liftangle = 1;
    liftServo.write(upAngle);
    Serial.print(ir);
    Serial.println(" Lifting.....");
    delay(500);
    Serial.print(ir);
    Serial.println(" Going down.....");
    for(int i = upAngle; i>= downAngle;--i) 
    {
      liftServo.write(i);
      delay(50);
    }
  }
  
  // --------------------- Shooting Mechanism ------------------------------
  
  /*---------------------------------------*/ liftangle=1;///for major_task/*---------------------------------------*/
  
  bluetooth.listen();

  if (bluetooth.available() > 0 && liftangle == 1) 
  {  
    input = int(bluetooth.parseInt());
    if(input != 1 && input != 2) Serial.print("Phone is Connected, Send Input");
    else
    {
      done = false;
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      Serial.print("Phone sent value: ");
      Serial.println(input);
      mySerial.listen();
      mySerial.println(input); 
      analogWrite(motorPin, spd[input]);
      while(!done)
      {
        Serial.print("Pulses: ");
        Serial.println(pulseCount);
      }
    }
    input = 0;
    liftangle = 0 ;
  }
}