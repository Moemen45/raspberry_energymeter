# Real-Time Monitoring System for AC Current and Voltage

This project enables real-time monitoring of AC current and voltage using the **Lechacal Board**—an electronic board equipped with AC ports and based on an Atmel AVR32DB28 microcontroller. The board collects and transmits AC data to a Raspberry Pi for MQTT publishing and visualization on EmonCMS.

## System Architecture

### Hardware Components
- **Lechacal Board**: 
  - Equipped with three AC current ports and one AC voltage port.
  - Built around an AVR32DB28 microcontroller, it handles ADC readings for each channel, applies offset corrections, and sends data over UART.
- **Raspberry Pi (Data Collector)**: 
  - Receives UART data from the Lechacal board, parses it, and publishes to MQTT for real-time monitoring.
- **Raspberry Pi (EmonCMS Server)**: 
  - Runs EmonCMS to graph and store the received data from MQTT topics.

### Data Flow
1. **Data Acquisition**: 
   - The Lechacal board continuously reads ADC values for each AC port and corrects offsets to generate accurate voltage and current readings.
2. **Data Transmission**: 
   - Corrected data is transmitted in a specific format over UART to the Raspberry Pi.
3. **MQTT Publishing**: 
   - The Raspberry Pi parses the UART data and publishes it to designated MQTT topics.
4. **Data Visualization**: 
   - Another Raspberry Pi, running EmonCMS, subscribes to these MQTT topics for real-time visualization and data storage.

