# ESP32 MQTT Data Publisher for Adafruit IO

This repository contains the firmware code for an **ESP32 microcontroller** designed to publish telemetry data (GPS coordinates and temperature) to the **Adafruit IO cloud platform** using the MQTT protocol.

The primary goal of this project is to send geographical data points that form a **star ($\star$) shape** over the Jabil facility in Guadalajara, Jalisco, along with a randomly generated temperature reading.

***

## ⚙️ Prerequisites

To compile and run this code, you will need:

1.  An **ESP32 Development Board**.
2.  The **Arduino IDE** or PlatformIO configured for ESP32.
3.  Necessary libraries for MQTT (e.g., Adafruit MQTT Library) and Wi-Fi.

***

## 🌐 Configuration and Connectivity Setup

**IMPORTANT:** This code is configured with my personal Adafruit IO credentials and Wi-Fi network details to facilitate easy testing and deployment. **You MUST replace these placeholder values** with your own details to run the code successfully.

### 1. Wi-Fi Network Details

The code is currently configured to connect to a mobile hotspot network. Please update the following variables with your own Wi-Fi credentials:

| Variable | Description |
| :--- | :--- |
| `WIFI_SSID` | Your Wi-Fi network name (SSID). |
| `WIFI_PASS` | Your Wi-Fi network password. |

### 2. Adafruit IO Credentials

You must replace the placeholder information with your own **Adafruit IO** username and **Active Key**.

| Variable | Description |
| :--- | :--- |
| `AIO_USERNAME` | Your Adafruit IO Username. |
| `AIO_KEY` | Your Adafruit IO Active Key. |

***

## 🚀 How to Run the Code

1.  **Update Credentials:** Modify the Wi-Fi and Adafruit IO credential variables in the main source file (`.ino` or `.cpp`).
2.  **Create Feeds:** Ensure you have created the necessary feeds on your Adafruit IO account (e.g., `gps-feed` and `temperature-feed`).
3.  **Upload:** Connect your ESP32 board and upload the compiled firmware.
4.  **Monitor:** Check your Adafruit IO dashboard to confirm the data is being received. You should see the coordinates tracing a star pattern on your map block.

### Visualization Note

The latitude and longitude coordinates in the code are specifically chosen to trace a **five-pointed star shape** on the Adafruit IO map, centered near the Jabil Guadalajara campus. This demonstrates precise control over the published geographical data.

