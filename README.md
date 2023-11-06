# Digital Volt-Ammeter Project

## Overview
This project demonstrates a basic digital voltmeter and ammeter using an ATmega8 microcontroller, LCD display (1602), voltage divider, current shunt, and an operational amplifier. It can accurately measure voltage in the range of 0 to 25 volts and current from 0 to 2.5 amperes. The measurements are displayed on the LCD.

## Components
- ATmega8 microcontroller
- LCD display 1602
- Voltage divider (100 kΩ and 10 kΩ resistors)
- Current shunt (0.1 Ω)
- Operational amplifier (op-amp)

## Voltage Measurement
Voltage is measured using a voltage divider, ensuring the input voltage doesn't exceed 55V. The measurements are calculated using the ADC and displayed on the LCD.

## Current Measurement
Current is measured with a current shunt, amplified by an op-amp, and calculated using the ADC. The results are displayed on the LCD.

## Program
The microcontroller's program continuously takes measurements, calculates average values from 250 samples, and displays the results on the LCD. The code is written in C, providing a beginner-friendly introduction to digital measurement.

---

For more detailed information, please refer to the project's documentation and source code.
