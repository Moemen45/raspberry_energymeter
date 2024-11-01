#!/usr/bin/python2
import serial
import time
import paho.mqtt.client as mqtt

# MQTT Client Setup
mqttc = mqtt.Client("python_pub")
mqttc.connect("127.0.0.1", 1883)

# Serial Port Configuration
ser = serial.Serial()
ser.port = "/dev/ttyS0"
ser.baudrate = 38400
ser.bytesize = serial.EIGHTBITS
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_ONE
ser.timeout = 10
ser.xonxoff = False
ser.rtscts = False
ser.dsrdtr = False

# Attempt to open serial port
try:
    ser.open()
except Exception as e:
    print("Error opening serial port:", e)
    exit(1)

if ser.isOpen():
    ser.flushInput()
    ser.flushOutput()

    try:
        while True:
            if ser.in_waiting > 0:  # Check if data is available
                synchronisation = ser.read().decode('utf-8')

                if synchronisation == ':':
                    # Read the rest of the line containing the data
                    line = ser.readline().decode('utf-8').strip()
                    data = line.split()  # Split data by spaces

                    # Parse data and validate
                    if len(data) == 6:
                        try:
                            NodeId = int(data[0])
                            power1 = float(data[1])
                            current1 = float(data[2])
                            tension = float(data[3])
                            apparentPower1 = float(data[4])
                            powerFactor1 = float(data[5])

                            # Print values
                            print(f"NodeId: {NodeId}")
                            print(f"Power1: {power1}")
                            print(f"Current1: {current1}")
                            print(f"Tension: {tension}")
                            print(f"ApparentPower1: {apparentPower1}")
                            print(f"PowerFactor1: {powerFactor1}")

                            # Publish values to MQTT
                            mqttc.publish("/nodeID", NodeId)
                            mqttc.publish("/power1", power1)
                            mqttc.publish("/current1", current1)
                            mqttc.publish("/tension", tension)
                            mqttc.publish("/apparentPower1", apparentPower1)
                            mqttc.publish("/powerFactor1", powerFactor1)

                        except ValueError as e:
                            print("Error parsing data:", e)

    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        ser.close()  # Ensure serial port is closed on exit
else:
    print("Cannot open serial port.")
