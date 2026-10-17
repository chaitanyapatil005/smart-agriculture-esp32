
# Smart Agriculture for Efficient Cultivation in Hilly Regions 🌱

## 📌 Project Overview
An ESP32-based IoT system designed to monitor agricultural conditions and automate irrigation. The system uses multiple sensors and a Wi-Fi web dashboard to support efficient water management in hilly regions.

## 🎯 Objectives
- Monitor soil moisture and environmental conditions.
- Automate irrigation based on soil moisture.
- Prevent pump operation when water availability is critically low.
- Display sensor readings through a Wi-Fi web dashboard.
- Show sensor information on a 16×2 I2C LCD.

## 🛠️ Hardware Components
- ESP32 DevKit V1
- Capacitive Soil Moisture Sensor
- DHT11 Temperature and Humidity Sensor
- LDR Light Sensor
- Water-Level Sensor
- 16×2 I2C LCD
- Relay Module
- Water Pump

## 💻 Software and Technologies
- Arduino IDE
- Embedded C/C++
- ESP32 Wi-Fi Web Server
- HTML, CSS, JavaScript
- Sensor interfacing and automation

## ⚙️ Working Principle
1. Sensors collect soil and environmental readings.
2. ESP32 processes the sensor data.
3. If soil moisture is low and sufficient water is available, the pump is activated.
4. When sufficient soil moisture is reached, the pump is switched off.
5. If water is critically low, the pump remains off.
6. Sensor values and pump status are displayed on the web dashboard.

## 🔌 Pin Connections

| Component | ESP32 GPIO |
|---|---|
| Soil Moisture Sensor | GPIO 34 |
| DHT11 Data | GPIO 4 |
| LDR Sensor | GPIO 35 |
| Water-Level Sensor | GPIO 32 |
| Relay Module | GPIO 26 |

## 🚀 Future Improvements
- Crop-specific monitoring and recommendations.
- Historical sensor data and graphical trends.
- Cloud integration and remote monitoring.
- Solar-powered irrigation.

## 👨‍💻 Developer
Chaitanya Patil

Electronics and Telecommunication Engineering

LinkedIn: https://www.linkedin.com/in/chaitanya-patil-7a922a30a/
