/*
   Uno version smart light

   Created: 4/12/2019 10:20:22 PM
   Author : Gerankin-EIS
*/

#include <avr/io.h>


#define LightSensor_PIN PB2
#define Relay_Light 5//(1<<PB0) //жёлтый
#define LED_B 3//(1<<PB1) //светодиод голубой
//#define LED_G 4//(1<<PB3) //светодиод зелёный 
#define PIR   2//датчик движения
#define calibrationTime 14 //секунд для инициализации таймера;
#define TIMEOUT_PIR 5000
#define BTN_INC 4
#define BTN_INC_TIME 5000 * 2
volatile unsigned long delayBy = 0;
volatile boolean movementOn = false;
void setup()
{
  //DDRB |= (LED_B | Relay_Light) ;
  pinMode(Relay_Light, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(BTN_INC, INPUT);
  digitalWrite(BTN_INC, HIGH);
   Serial.begin(9600);
  for (int i = 0; i < calibrationTime; i++) {
    digitalWrite(LED_B, !digitalRead(LED_B));
    delay(1000);
  }
  attachInterrupt(digitalPinToInterrupt(PIR), pin_ISR, RISING );
}
void pin_ISR()
{
  movementOn = true;
  unsigned long tmp = millis() + TIMEOUT_PIR;
  if (delayBy < tmp) {
    delayBy = tmp;
    Serial.println("inc time");
  }
}
volatile bool activatedLight = false;
char btnIncCount = 0;
bool incButtonClicked = false;
void loop()
{
  if (! movementOn ) {
    return;
  }

  if (!activatedLight) {
    Serial.println("Movement");
    int photo = analogRead(0);
    Serial.println(photo);
    Serial.println(delayBy);
    if (photo > 300) {
      Serial.println("Light ON");
      digitalWrite(Relay_Light, HIGH);
      activatedLight = true;
    }
    else{
      movementOn = false;
      Serial.println("Skip movement");
    }
  }


  if (activatedLight && (millis() > delayBy)) {
    if(incButtonClicked){
      delayBy += BTN_INC_TIME;
      incButtonClicked = false;
      Serial.println("Added BTN time");
      return;
    }
    Serial.println("Time over, Light OFF");
    digitalWrite(Relay_Light, LOW);
    movementOn = false;
    delayBy = 0;
    activatedLight = false;
  }
  if(activatedLight){
    bool pressed = digitalRead(BTN_INC) == LOW;
    if( pressed && btnIncCount<5){
      btnIncCount++;
    }
    if(!pressed && btnIncCount >0){
      btnIncCount--;
    }
    
    if(btnIncCount == 5){
      incButtonClicked = true;
      btnIncCount = 0;
    }
  }
}
