# Temperature Measurement Circuit and Visualization System

This project demonstrates the design and implementation of a temperature measurement system capable of measuring and visualizing real-time temperature data on a web interface. The system utilizes an NTC thermistor, an ESP8266 microcontroller, 
and other electronic components (opamps, potentiometers, capacitors, etc.) to achieve accurate temperature measurement and efficient data visualization.

## Features
- **Temperature Measurement**: Measures temperature changes within the range of 0–60°C using an NTC thermistor.
- **Web-based Visualization**: Hosts a web interface for real-time temperature data visualization on any internet-connected device.
- **Real-time Processing**: Processes and amplifies the analog signal using operational amplifiers and ESP8266's ADC.
- **WiFi Connectivity**: Uses WiFi and web server libraries to enable data hosting and remote monitoring.
- **Debugging Support**: Debugged and verified via serial monitoring for high reliability.

## Components Used
- **NTC Thermistor**: For accurate temperature measurement.
- **ESP8266 Microcontroller**: For ADC input, data processing, and web hosting.
- **Operational Amplifiers**: To process and amplify the analog signal.
- **Other Components**: Potentiometers, capacitors, resistors, etc.
- **Power Supply**: -3.3V to +3.3V for circuit operation.

## Project Details
You can find the detailed circuit design and calculations in the [`project_details.pdf`](project_details.pdf) file.
The implementation can also be found in the [`temperature_measurement.ino`](temperature_measurement.ino) file, which contains the Arduino C++ code.

## Live Demo
Check out the live demo of the web interface here:


https://github.com/user-attachments/assets/c9c8a103-6e4c-4cf5-8589-cfd0bf4ea3c1

