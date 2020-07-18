#include <SerialButtons.h>
#include <Button.h>

#include "Arduino.h"

/**
 * initializing pins & constants
 * 
 * pin 2  :(OUTPUT) => led showing that the board is on
 * pin 3  :(INPUT)  => switch 3 (button 7)
 * pin 4  :(INPUT)  => switch 2 (button 6)
 * pin 5  :(INPUT)  => switch 1 (button 5)
 * pin 6  :(OUTPUT) => led for switch 3
 * pin 7  :(OUTPUT) => led for switch 2
 * pin 8  :(OUTPUT) => led for switch 1
 * pin 9  :(OUTPUT) => led for serial button 4
 * pin 10 :(OUTPUT) => led for serial button 3
 * pin 11 :(OUTPUT) => led for serial button 2
 * pin 12 :(OUTPUT) => led for serial button 1
 * pin 13 :(OUTPUT) => led for serial button 0
 * pin A0 :(INPUT)  => serial buttons 0 to 4
 */
const uint8_t powerOnLed = 2;
const uint8_t maxLedPin = 13;
const uint8_t minSerialLedPin = 9;
const uint8_t minLedPin = 6;
const uint8_t serialButtonsPin = A0;

/**
 * debug flag - turn it on to display messages on the serial monitor
 */
bool debug = true;

/**
 * initializing buttons 
 * 
 * Serial buttons are on an analog pin, and use a scale of resistance
 * to know which button is pressed. When one button is pressed, it turns
 * the corresponding led on, and switches the others off. Used for toggling
 * between channels 1-5 of the controlled device
 * 
 * Switch buttons are on digital pins and each toggle a separate led on and off.
 * Used for toggling effects on and off.
 */
SerialButtons serialButtons(serialButtonsPin, debug);
Button button1(5, debug);
Button button2(4, debug);
Button button3(3, debug);

/**
 * Sends a midi signal based on a button pressed, and a on / off flag
 * 
 * @param uint8_t number of the button called (1 to 8)
 * 
 * @param bool on / off flag for the button
 */
void sendMidiSignal(uint8_t buttonNumber, bool on)
{
  // TODO: implement midi through bluetooth LTE
  Serial.print("Sending midi signal:: ");
  Serial.print(buttonNumber);
  Serial.print(" - ");
  Serial.println(on ? "on" : "off");
}

/**
 * callback invoked when one of the serial buttons is pressed.
 * Toggles the led of the button pressed on, and the others off.
 * Sends a midi signal for that button.
 * 
 * We loop through output pins 9 to 13. Matching leds are found by
 * calculating the difference between the max pin (13) and the button number
 * button 0 => led 13
 * button 1 => led 12
 * button 2 => led 11
 * button 3 => led 10
 * button 4 => led 9
 * 
 * @param uint8_t number of the button pressed
 */
void buttonPressed(uint8_t buttonNumber)
{
  {
    for (uint8_t i = minSerialLedPin; i <= maxLedPin; i++)
    {
      if (i == maxLedPin - buttonNumber)
      {
        digitalWrite(i, HIGH);
        sendMidiSignal(buttonNumber, true);
      }
      else
      {
        digitalWrite(i, LOW);
      }
    }
  }
}

/**
 * callback invoked when one of the serial buttons is pressed.
 * Detects which button is called based on the analog value, and
 * invokes the buttonPressed method with the button number.
 * 
 * Button 0 => 0 Ω     - analog value 1023 (min 1000)
 * Button 1 => 560 Ω   - analog value 969  (min 900)
 * Button 2 => 4.7 kΩ  - analog value 696  (min 600)
 * Button 3 => 10 kΩ   - analog value 512  (min 400)
 * Button 4 => 1 MΩ    - analog value 8    (min 4)
 * 
 * No button pressed will yield an analog value of 0
 * 
 * values below are set with a margin to account for the tolerance of
 * the resistance
 * 
 * @param uint16_t value read on the analog pin of the serial buttons
 */
void serialButtonPressed(uint16_t value)
{
  uint16_t buttonValues[5] = {1000, 900, 600, 400, 4};

  if (value >= buttonValues[0])
  {
    buttonPressed(0);
  }
  else if (value >= buttonValues[1])
  {
    buttonPressed(1);
  }
  else if (value >= buttonValues[2])
  {
    buttonPressed(2);
  }
  else if (value >= buttonValues[3])
  {
    buttonPressed(3);
  }
  else if (value >= buttonValues[4])
  {
    buttonPressed(4);
  }
}

/**
 * callback invoked when a switch is pressed.
 * Matching led pin is found by calculating switch pin + 3
 * Matching button number is 10 - switch pin
 * 
 * Switch 1: pin 3 => led 6 - button 7
 * Switch 2: pin 4 => led 7 - button 6
 * Switch 3: pin 5 => led 8 - button 5
 * 
 * @param uint8_t pin of the switch pressed
 * 
 * @param bool on / off status of the button
 */
void switchPressed(uint8_t switchPin, bool on)
{
  digitalWrite(switchPin + 3, on ? HIGH : LOW);
  sendMidiSignal(10 - switchPin, on);
}

/* Arduino methods */

/**
 * During setup, we start a serial session if the debug flag is on,
 * and we initialize the output pins for the leds.
 * We also switch the power indicator led on.
 */
void setup()
{
  if (debug)
  {
    Serial.begin(9600);
  }

  // TODO: initialize Bluetooth LTE

  pinMode(powerOnLed, OUTPUT);
  digitalWrite(powerOnLed, HIGH);

  for (uint8_t i = minLedPin; i <= maxLedPin; i++)
  {
    pinMode(i, OUTPUT);
  }
}

/**
 * In the loop, we use the public method on the buttons to know when they
 * are pressed, and trigger the corresponding action
 */
void loop()
{
  serialButtons.use(serialButtonPressed);
  button1.use(switchPressed);
  button2.use(switchPressed);
  button3.use(switchPressed);
}
