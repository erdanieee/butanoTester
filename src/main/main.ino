/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.
 
 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 
 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.
 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND
 
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
 
*/

/*
 * Funcionamiento:
 * pulsación normal -> mostrar los valores
 * pulsación larga (> 5s) -> establecer tara bombona llena
 * pulsación muy larga (> 10s) -> calibración sin peso
 */

#include "HX711.h"
#include "main.h"
#include <avr/sleep.h>

#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define zero_factor 8421804 //This large value is obtained using the SparkFun_HX711_Calibration sketch

  
#define SERIAL_RX   0
#define SERIAL_TX   1
#define BUTTON      2
#define CLK         3
#define DOUT        4
#define LED         13

HX711 scale(DOUT, CLK);
MODE modo;
float r;

unsigned long buttonTime,
              displayTime;

void setup() {
  Serial.begin(9600);
  Serial.println("Butano tester v0.1");

  pinMode(BUTTON, INPUT_PULLUP); 
  pinMode(LED, OUTPUT);

  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.set_offset(zero_factor); //Zero out the scale using a previously known zero_factor
  //scale.tare();  //Assuming there is no weight on the scale at start up, reset the scale to 0
  
  attachInterrupt(0, wakeUp_ISR, LOW); // use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void wakeUp_ISR()        // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
}

void sleepNow(){
  delay(100);
    
  scale.power_down();       //shut down hx7100
  
  sleep_enable();          // enables the sleep bit in the mcucr register so sleep is possible. just a safety pin  
  sleep_mode();            // here the device is actually put to sleep!!
                           // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  
  sleep_disable();         // first thing after waking from sleep:
  detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                           // wakeUpNow code will not be executed
                           // during normal running time.
  scale.power_up(); 
  mode = NORMAL;  
}

void loop() { 
  switch(modo){
    case NORMAL:
      r = scale.get_units(10);
      //TODO: display refresh

      //Serial print
      Serial.print(scale.get_units(10), 1); //scale.get_units() returns a float
      Serial.print(" kg"); //You can change this to kg but you'll need to refactor the calibration_factor
      Serial.println();

      delay(5000);
      
      sleepNow();
      break;

    case SETUP:
      break;

    case CALIBRATE:
      break;
    
  }
  
  Serial.print("Reading: ");
  
}
