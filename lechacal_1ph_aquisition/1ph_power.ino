#include "aquisition.h"                    // Include acquisition library
#include <math.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>                // For UART communication with Lechacal

// Pin Definitions for Software Serial
#define RX    9                            // RX pin for UART
#define TX    8                            // TX pin for UART
SoftwareSerial Serial1(RX, TX);

// Energy Monitor Setup for Single Phase
EnergyMonitor ct1;

// Offset and Calibration Constants
int offsetV, offsetI1;
float Ical1 = 90.9;                        // Calibration constant for current sensor
float Vcal = 501.26;                       // Calibration constant for voltage

// Pin Assignments for Sensors
#define analogPin_V  7
#define analogPin_I1 0

// Node and Communication Settings
#define NodeId 11
const unsigned long BAUD_RATE = 38400;
const int TIME_BETWEEN_READINGS = 3000;    // Time between readings in ms

// Measurement Variables
float power1, tension, apparentPower1, powerFactor1, current1;
char synchronisation;

// Timer Variables
unsigned long now = 0;                     // Current time for sample tracking
unsigned long last_sample = 0;             // Last sample timestamp

void setup() {
    delay(100);
    pinMode(RX, INPUT);
    pinMode(TX, OUTPUT);
    Serial1.begin(BAUD_RATE);
    Serial1.println("OK");
}

void loop() {
    now = millis();

    // Take a new sample if enough time has passed
    if ((now - last_sample) > TIME_BETWEEN_READINGS) {
        // Calculate offsets
        offsetI1 = ct1.calcVI_offset(analogPin_I1, 20, 200);
        offsetV = ct1.calcVI_offset(analogPin_V, 20, 200);

        // Calculate RMS values for single phase
        bool FFT_result = ct1.calcVI_rms(analogPin_I1, analogPin_V, offsetI1, offsetV, 1);

        // Process current and power values
        current1 = ct1.Irms >= 0.003 ? ct1.Irms * Ical1 : 0;
        tension = ct1.Vrms * Vcal;
        power1 = ct1.realPower;
        apparentPower1 = ct1.apparentPower;
        powerFactor1 = ct1.powerFactor * 100;

        // Synchronization and Data Transfer
        if (Serial1.available()) {
            synchronisation = Serial1.read();
        }
        
        if (synchronisation == ':') {
            synchronisation = '0';
            
            // Send formatted data to Serial1
            Serial1.print(NodeId);
            Serial1.print(" ");
            Serial1.print(power1, 1);
            Serial1.print(" ");
            Serial1.print(current1, 2);
            Serial1.print(" ");
            Serial1.print(tension, 1);
            Serial1.print(" ");
            Serial1.print(apparentPower1, 1);
            Serial1.print(" ");
            Serial1.print(powerFactor1, 1);
            Serial1.println();
        }

        // Update last sample time
        last_sample = now;
    }
}
