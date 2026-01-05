# Smart Irrigation System
## Raspberry Pi Pico – IoT-Based Irrigation Control

## 📌 Overview

This project implements a **Smart Irrigation System** using a **Raspberry Pi Pico** and microcontroller-based circuitry. The system is designed to support **fully automatic, semi-automatic, and manual irrigation modes**, using environmental sensing and intelligent control to optimize water usage.

The system continuously monitors soil moisture conditions, controls sprinkler heads automatically, manages watering duration, detects intrusions for safety, and provides real-time feedback through displays and indicators. **Multithreading** is implemented to ensure high-performance operation.

This project was developed is  **focused on IoT systems, embedded design, and smart agriculture applications**.

---

## 🎯 Project Objectives

- Design an intelligent irrigation system using IoT principles
- Automatically detect dry soil areas using multiple sensors
- Control multiple sprinkler heads independently
- Adjust pump operation based on irrigation demand
- Provide real-time user feedback via display output
- Implement safety features such as intrusion detection
- Ensure high performance using multithreading
- Track system usage and indicate maintenance requirements

---

## ⚙️ System Requirements and Specifications

### Functional Requirements
- Automatic detection of dry soil areas
- Automatic activation of sprinkler heads
- Manual and semi-automatic control modes
- Watering duration control with countdown display
- Intrusion detection during watering
- Maintenance alert after repeated usage

### Non-Functional Requirements
- Reliable real-time performance
- User-friendly visual feedback
- Safe operation for humans and animals
- Efficient task execution using multithreading

---

## 🧪 Implemented Features 

### 1. Soil Moisture Detection
- Three sensors monitor soil moisture levels
- Each sensor determines whether its corresponding area is dry

### 2. Automatic Sprinkler Control 
- Three sprinkler heads are positioned near each sensor
- A sprinkler opens automatically when its nearby sensor detects dry soil

### 3. Pump Operation and Display Indication 
- A servo motor represents pump operation
- Three pump speed states indicate the number of active watering zones
- A display shows which area is currently being watered

### 4. Watering Period Control 
- Watering duration is controlled by a switch
- Maximum watering time is limited to 30 seconds
- Watering stops immediately if the switch is closed

### 5. Remaining Time Display 
- During watering, the remaining time (in seconds) is displayed in real time

### 6. Intrusion Detection and Alert System 
- An intrusion sensor detects human or animal presence
- An alert action is triggered if intrusion is detected during watering

### 7. Multithreading for High Performance 
- Multiple tasks run concurrently, including:
  - Sensor monitoring
  - Timer countdown
  - Intrusion detection
  - Display updates
- Multithreading ensures responsiveness and efficient system operation

### 8. Maintenance Indicator 
- The system counts the number of watering cycles
- After 30 irrigation cycles, a maintenance alert is displayed

---

## 🧩 System Design Overview

### Hardware Components
- Raspberry Pi Pico (Microcontroller)
- Soil moisture sensors (x3)
- Sprinkler valves / relays (x3)
- Servo motor (pump indicator)
- Intrusion sensor (PIR / ultrasonic)
- Display (LCD / OLED)
- Control switches
- Power supply

### Software Design
- Multi-mode control logic (automatic, semi-automatic, manual)
- Task-based multithreading architecture
- State-based system behavior
- Timer and interrupt-driven operations

---

## 🧠 Engineering Concepts Demonstrated
- IoT-based smart agriculture systems
- Embedded systems programming
- Multithreading and concurrent task execution
- Sensor-actuator integration
- Safety-critical system design
- Real-time monitoring and control

---

## 🛠 Technologies Used
- Raspberry Pi Pico
- Embedded C++
- GPIO and PWM control
- Multithreading / task scheduling
- IoT system design principles

---

## 🧪 Simulation and Testing
The system was tested through simulation and controlled input scenarios to verify:
- Correct sprinkler activation
- Accurate timing behavior
- Reliable intrusion detection
- Stable multithreaded execution
- Proper maintenance alert triggering

## SCHEMATIC
<img width="718" height="342" alt="image" src="https://github.com/user-attachments/assets/912e83dc-3255-4da1-b89c-2156e716f44a" />

## SCREENSHOTS
<img width="724" height="1169" alt="image" src="https://github.com/user-attachments/assets/237756d1-bcad-440c-9c9e-c61a4af2d444" />
<img width="941" height="1254" alt="image" src="https://github.com/user-attachments/assets/962ac233-feea-4064-8a59-0b6d8f34bc33" />
<img width="941" height="1254" alt="image" src="https://github.com/user-attachments/assets/49640998-63bb-4723-ba35-b5d92785643f" />
<img width="941" height="925" alt="image" src="https://github.com/user-attachments/assets/653362c9-314f-4024-97b5-83965cba9202" />










