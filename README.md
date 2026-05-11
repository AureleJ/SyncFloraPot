# 🪴 SyncFloraPot

[![Site Web](https://img.shields.io/badge/Project-Live_Demo-brightgreen)](https://aurelejoblet.com/project/syncflorapot)
[![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20Android%20%7C%20iOS-blue)](#)
[![Status](https://img.shields.io/badge/Status-In--Progress-orange)](#)

**SyncFloraPot** is an end-to-end IoT ecosystem designed for smart plant care. It combines custom embedded firmware, a scalable backend, and a cross-platform mobile application to provide real-time monitoring and automated watering.

🔗 **Detailed Project Overview:** [aurelejoblet.com/project/syncflorapot](https://aurelejoblet.com/project/syncflorapot)

---

## 🚀 Overview

SyncFloraPot isn't just a smart pot, it's a complete monitoring solution. It tracks soil moisture, ambient temperature, and light levels, allowing users to interact with their plants through a dedicated mobile interface.

### Key Features
- **Real-time Data:** Live sensor feedback via MQTT/WebSockets.
- **Automated Irrigation:** Smart watering logic based on soil moisture thresholds.
- **Cross-Platform Control:** Monitor your plants from anywhere using the mobile app.
---

## 🛠 Technical Stack & Architecture

### 📁 [Firmware](./firmware)
* **Hardware:** ESP32 Microcontroller.
* **Framework:** ESP-IDF.
* **Sensors:** Capacitive soil moisture sensor, DHT22 (Temp/Hum), Light intensity sensor.
* **Communication:** Wi-Fi, MQTT protocol.

### 📁 [Client](./client)
* **Framework:** React Native / Expo.
* **Features:** Real-time dashboards, push notifications for low water levels, and manual override controls.

### 📁 [Backend](./backend)
* **Environment:** Node.js.
* **Database:** MongoDB/PostgreSQL for historical data logging.
* **Communication:** Integrated MQTT Broker for hardware-to-cloud messaging.

---

## 📂 Directory Structure

```txt
SyncFloraPot/
├── firmware/   # ESP32 Firmware code (ESP-IDF)
├── client/     # Mobile application (Android/iOS)
└── backend/    # Server logic and database management

```

---

## 🔧 Installation & Setup

1. **Clone the repo:**
```bash
git clone [https://github.com/AureleJ/SyncFloraPot.git](https://github.com/AureleJ/SyncFloraPot.git)

```

2. **Firmware:** Navigate to `/firmware` and follow the `idf.py build` instructions.
3. **Mobile App:** Run `npm install` in `/client` then `npx expo start`.
4. **Backend:** Configure your `.env` in `/backend` and run `npm start`.

---

## 👤 Author

**Aurèle Joblet** Engineering Student at ESIEA | Embedded Systems & Software Development

🌐 [Portfolio](https://aurelejoblet.com) | [LinkedIn](https://www.google.com/search?q=https://www.linkedin.com/in/aurele-joblet/)
