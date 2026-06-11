#include <Servo.h>  
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// --------------------- Pin Configration ------------------------------
#define trigPin 9
#define echoPin 10  
#define irPin 7   
SoftwareSerial mySerial(5, 6);// ( RX , TX )
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// --------------------- Constants Configration ------------------------------
int input = 1; /// For major Task
int score = 0;
int scrnDelay = 5000;
Servo myServo;  

void setup() {  
  Serial.begin(9600);  
  mySerial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
  pinMode(irPin, INPUT);  

  myServo.attach(6);
  myServo.write(60); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Element Explorer");
}  

float readDis()
{
  int numReadings = 10; 
  float sum = 0;

  for(int i = 0; i < numReadings; i++)
  {
    long duration = 0;
    float distance = 0;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);  

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);  

    duration = pulseIn(echoPin, HIGH, 30000);  
    distance = duration * 0.034 / 2;

    sum += distance;

    delay(5); 
  }

  float avgDistance = sum / numReadings;
  return avgDistance;
}

void loop() {  

  lcd.setCursor(0,0);
  lcd.print("Your Score is: ");
  lcd.print(score);

  if (mySerial.available() > 0) {
    input = mySerial.parseInt(); 
    Serial.print("Received input: ");
    Serial.println(input);
  }

  float distance = readDis();  
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 7 && distance < 8.1) 
  {   
    if(input==-1){
      Serial.println("The Answer is not received from the shooting yet....");
      //continue;
    }  
   
    delay(1000);
    int irState = digitalRead(irPin);  
    Serial.print("IR: ");
    Serial.println(irState);

    if (irState == LOW) myServo.write(5);
    else for(int i=60;i<=160;i++) {myServo.write(i);delay(10);} 
    delay(1000);        
    myServo.write(60); 
    
    //lcd.clear();
    if(input == int(irState)+1 )
    {
      lcd.setCursor(0,0);
      lcd.print("Right answer");
      lcd.setCursor(0,1);
      lcd.print("Great Job!");
      score++;
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("Wrong answer");
      lcd.setCursor(0,1);
      lcd.print("Try again!");
    } 
    delay(scrnDelay);
  }  
}