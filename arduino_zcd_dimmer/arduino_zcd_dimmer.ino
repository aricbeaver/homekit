/*
 * Uses ZCD output as interrupt to control triac AC duty cycle. Set above MAX_DIM_DUTY for 100% ON and below MIN_DIM_DUTY for completely OFF.
 * D2 = ZCD interrupt.
 * D3 = PWM interrupt for controlling duty cycle from ESP8266. Must be 1kHz frequency.
 * D5 = PWM output for controlling AC duty cycle to triac dimmer.
 */

#include "Dimmer.h"
#include "PWM.hpp"

#define DEBUG // Comment to disable serial port

#define MAX_DIM_DUTY 90
#define MIN_DIM_DUTY 20

PWM input_pwm(3);
Dimmer output_pwm(5);

int input_pwm_value = 0;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600); // Serial for debug
  #endif
  input_pwm.begin(true);
  output_pwm.begin();
  randomSeed(analogRead(0)); // this line needed?
}

void loop() {
  
  input_pwm_value = input_pwm.getValue() / 10; // This assumes PWM frequency is 1kHz.
  output_pwm.set(input_pwm_value);
  delay(100);

  #ifdef DEBUG
    Serial.print("Value: ");
    Serial.print(input_pwm_value);
    Serial.print("\tAGE: ");
    Serial.println(input_pwm.getAge());
  #endif

}
