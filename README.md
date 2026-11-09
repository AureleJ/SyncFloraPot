# 🪴 SyncFloraPot

[![Site Web](https://img.shields.io/badge/Project-Live_Demo-brightgreen)](https://aurelejoblet.com/project/syncflorapot)
[![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20Android-blue)](#)
[![Status](https://img.shields.io/badge/Status-In--Progress-orange)](#)

**SyncFloraPot** is an end-to-end IoT ecosystem designed for smart plant care. It combines custom embedded firmware, a custom KiCad-designed PCB, a time-series backend, and a native Android application to provide real-time monitoring and automated watering.

🔗 **Detailed Project Overview:** [aurelejoblet.com/project/syncflorapot](https://aurelejoblet.com/project/syncflorapot)

---

## 🚀 Overview

SyncFloraPot isn't just a smart pot, it's a complete monitoring solution. It tracks soil moisture, light level, and water reservoir level, allowing users to interact with their plants through a dedicated mobile interface. The onboard OLED screen runs **DianUI**, a custom graphics engine built from scratch for this kind of embedded display.

### Key Features
- **Real-time Data:** Live sensor feedback via MQTT, pushed to the app over WebSockets (Socket.io).
- **Automated Irrigation:** Smart watering logic based on soil moisture thresholds, pump driven through a switching transistor with a hardware safety timer.
- **Local Alerts:** On-device Android notifications (with a per-sensor cooldown) when soil moisture or water level gets too low.
- **BLE Provisioning:** Wi-Fi setup directly from the mobile app, no hardcoded credentials.

---

## 🛠 Technical Stack & Architecture

### 📁 [Firmware](./firmware)
* **Hardware:** ESP32-C3 Microcontroller.
* **Framework:** ESP-IDF, FreeRTOS.
* **Sensors:** Capacitive soil moisture sensor, LDR (light level), ultrasonic sensor (reservoir water level).
* **Communication:** Wi-Fi, MQTT, BLE (Wi-Fi provisioning).
* **Display:** OLED over I2C, driven by DianUI (custom C rendering engine).

### 📁 [Hardware](./hardware)
* **Design:** Custom PCB in KiCad features power supply, sensor interfacing and pump control.

### 📁 [Client](./client)
* **Framework:** Native Android, Kotlin + Jetpack Compose.
* **Features:** Real-time dashboard, local alert notifications, BLE provisioning flow, manual watering override.

### 📁 [Backend](./backend)
* **Environment:** Node.js, Express.
* **Database:** PostgreSQL with the TimescaleDB extension, for time-series sensor history.
* **Communication:** Mosquitto MQTT broker for device-to-cloud messaging, Socket.io for live updates to the app.
* **Auth:** JWT-based user authentication.

---

## 📂 Directory Structure

```txt
SyncFloraPot/
├── firmware/   # ESP32-C3 firmware (ESP-IDF, FreeRTOS)
├── hardware/   # KiCad PCB design 
├── client/     # Native Android app (Kotlin, Jetpack Compose)
└── backend/    # Server, database and MQTT broker setup

```

## 👤 Author

**Aurèle Joblet** - Engineering Student at ESIEA

🌐 [Portfolio](https://aurelejoblet.com) | [LinkedIn](https://www.linkedin.com/in/aurele-joblet/)
