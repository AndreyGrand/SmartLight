/*
   Uno version smart light

   Created: 4/12/2019 10:20:22 PM
   Author : Gerankin-EIS
   C:\Users\Gerankin-EIS\AppData\Local\Temp
*/

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define Relay_Light 2//(1<<PB2) //жёлтый
#define LED_B 1//(1<<PB1) //светодиод голубой
#define LED_G 4
#define PIR   3//датчик движения
#define BTN_INC 0
#define PHOTO_SENSOR_PIN 5 //PIN5 mapped on analog ADC0
#define PHOTO_SENSOR_READ 0 //analog ADC0

#define BTN_INC_TIME TIMEOUT_PIR * 2
#define calibrationTime 14 //секунд для инициализации таймера;
#define TIMEOUT_PIR 8000

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
  PCMSK |= (PIR_AVR); // pin change interrupt enabled for PCINT4
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

  pinMode(PIR, INPUT);
  //digitalWrite(PIR, HIGH);
  initPins();
  
    for (int i = 0; i < calibrationTime; i++) {
      digitalWrite(LED_B, !digitalRead(LED_B));
      delay(1000);
    }
  
  initInterrupt();
}

volatile bool activatedLight = false;
char btnIncCount = 0;
bool incButtonClicked = false;
void sleep() {
  teardownPins();
  //   GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  //   PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  // PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
  initPins();
} // sleep


void loop()
{
  if (! movementOn ) {
    sleep();
    return;
  }
  digitalWrite(LED_G, incButtonClicked);
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
    btnIncCount = 0;
  }


  if (activatedLight) {
    bool pressed = digitalRead(BTN_INC) == LOW;
    if ( pressed && btnIncCount < 5) {
      btnIncCount++;
    }
    if (!pressed && btnIncCount > 0) {
      btnIncCount--;
    }

    if (btnIncCount == 5 ) {
      incButtonClicked = true;
      digitalWrite(LED_B, !digitalRead(LED_B));
      btnIncCount = 0;
    }
  }
}
void initPins() {
  pinMode(Relay_Light, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BTN_INC, INPUT);
  digitalWrite(BTN_INC, HIGH);

  pinMode(PHOTO_SENSOR_PIN, INPUT);
}

void teardownPins()
{
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  pinMode(A1, INPUT);
  analogWrite(A1, 0);
  pinMode(A2, INPUT);
  analogWrite(A2, 0);
  // pinMode(A3,INPUT);
  // analogWrite(A3, 0);
}
