/*
   Uno version smart light

   Created: 4/12/2019 10:20:22 PM
   Author : Gerankin-EIS
   C:\Users\Gerankin-EIS\AppData\Local\Temp
*/

#include <avr/io.h>


#define Relay_Light 2//(1<<PB2) //жёлтый
#define LED_B 1//(1<<PB1) //светодиод голубой
#define PIR   3//датчик движения
#define BTN_INC 0
#define PHOTO_SENSOR_PIN 5 //PIN5 mapped on analog ADC0
#define PHOTO_SENSOR_READ 0 //analog ADC0

#define BTN_INC_TIME 8000
#define calibrationTime 14 //секунд для инициализации таймера;
#define TIMEOUT_PIR 1000

// AVR ports
#define BTN_INC_AVR (1<<PB0)
#define PIR_AVR (1<<PB3)
#define LED_B_AVR (1<<PB1)
#define RELAY_LIGHT_AVR (1<<PB2)

volatile unsigned long delayBy = 0;
volatile boolean movementOn = false;

void initInterrupt(void)
{
  GIMSK |= (1 << PCIE);   // pin change interrupt enable
  PCMSK |= PIR_AVR; // pin change interrupt enabled for PCINT4
  //BTN_INC_AVR |
}
unsigned long startTime = 0;
ISR(PCINT0_vect)
{
  digitalWrite(LED_B, !digitalRead(LED_B));
  movementOn = true;
  unsigned long tmp = millis() + TIMEOUT_PIR;
  startTime = millis() ;
  if (delayBy < tmp) {
    delayBy = tmp;
  }
}

void setup()
{
  //DDRB |= (LED_B | Relay_Light) ;
  pinMode(Relay_Light, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(PIR, INPUT);

  pinMode(BTN_INC, INPUT);
  digitalWrite(BTN_INC, HIGH);

  pinMode(PHOTO_SENSOR_PIN, INPUT);
  //digitalWrite(PHOTO_SENSOR_PIN, HIGH);

  for (int i = 0; i < calibrationTime; i++) {
    digitalWrite(LED_B, !digitalRead(LED_B));
    delay(1000);
  }
  initInterrupt();
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
    int photo = analogRead(PHOTO_SENSOR_READ);
    if (photo > 300) {
      digitalWrite(Relay_Light, HIGH);
      activatedLight = true;
    }
    else {
      movementOn = false;
    }
  }

  if (activatedLight && (millis() > delayBy)) {

    if (incButtonClicked) {
      delayBy += BTN_INC_TIME;
      incButtonClicked = false;
      return;
    }

    digitalWrite(Relay_Light, LOW);
    movementOn = false;
    delayBy = 0;
    activatedLight = false;
  }


  if (activatedLight) {
    bool pressed = digitalRead(BTN_INC) == LOW;
    if ( pressed && btnIncCount < 5) {
      btnIncCount++;
    }
    if (!pressed && btnIncCount > 0) {
      btnIncCount--;
    }

    if (btnIncCount == 5) {
      incButtonClicked = true;
      btnIncCount = 0;
    }
  }

}
