# HW6: IoT Project - Adafruit IO Data Publishing and Local CSV Logging

This repository contains all the source code, configuration files, and documentation generated for **Homework 6 (HW6)**. The project implements a complete IoT data pipeline, from an embedded device (ESP32) through the MQTT protocol to both a cloud platform (Adafruit IO) and a local data logger.

## 📂 Directory Structure Overview

The project is organized into the following key directories and files:

| Folder/File | Description |
| :--- | :--- |
| **`HW6_micros.pdf`** | The **formal project report** (originally requested in LaTeX/PDF format). This document contains the objectives, step-by-step procedure (A, B, C), and conclusion. |
| **`mqtt5/`** | **ESP32 Publisher Code.** This directory holds the firmware designed for the ESP32 (using ESP-IDF/C++). This code connects to Wi-Fi and Adafruit IO via MQTT. |
| **`Paho-MQTT/`** | **Python Local Logger.** This directory contains the Python script used for network monitoring and local logging. |
| **`Pictures/`** | **Proof and Test Images.** Contains all required screenshots documenting the project setup and results (Mosquitto, Paho-MQTT, Adafruit IO Dashboard). 
---

## 📝 Project Summary and Implementation Details

### 1. The Publisher (`mqtt5/`)

This directory contains the core application that publishes data to the cloud.

* **Platform:** ESP32 (simulated or real hardware).
* **Purpose:** To publish **randomly generated temperature** and **GPS coordinates** to Adafruit IO.
* **Geospatial Feature:** The coordinate payload is specifically designed to cycle through points that trace a **five-pointed star ($\star$) shape** over the **Jabil building in Guadalajara, Jalisco**.

**⚠️ Configuration Warning:**
The connection details are configured for my personal network and Adafruit IO account. To run this code, you **MUST** modify the code to include your own:
* Wi-Fi SSID and Password.
* Adafruit IO Username and Active Key.

### 2. The Local Logger (`Paho-MQTT/`)

This component fulfills the local data logging requirement.

* **Tool:** Python with the **Paho-MQTT** library.
* **Function:** Subscribes to the local Mosquitto broker to capture connection events and message traffic.
* **Output:** All captured data (timestamps, topics, and message metadata) is logged to the **`mqtt_capture.csv`** file, fulfilling the local data logging requirement based on the provided code template.

### 3. Proof of Work (`Pictures/`)

The following image files provide visual confirmation of the successful implementation of each step:

| File Name | Description |
| :--- | :--- |
| `Mosquitto_probe.png` | Proof of the running **local Mosquitto Broker** service. |
| `PAHO_MQTT_probes.png` | Screenshot of the **Python logger script** actively running and subscribing. |
| `adafruit_probe.png` | Confirmation of the successful **data transfer** to Adafruit IO (e.g., MQTT connection log). |
| `adafruit_data.png` | The **Adafruit IO Dashboard** showing the final visualization (star on the map and temperature gauge). |

----
