# ESP32 ECU Simulator

An embedded C++ project built on ESP32 using Bluepad32 and an Xbox controller to simulate basic ECU behavior.

## Features

- Engine ON/OFF toggle using the A button
- Right Trigger controls throttle input
- Idle RPM set to 800
- RPM increases linearly with throttle
- Engine temperature model based on RPM
- Cooling system activates above 100°C and deactivates below 90°C
- Ambient temperature can be set through the Serial Monitor

## Hardware and tools

- ESP32-WROOM-32
- Xbox controller
- Arduino IDE
- Bluepad32 library

## Project logic

- The A button toggles the engine state
- When the engine is OFF, RPM is 0 and the engine cools down toward ambient temperature
- When the engine is ON, RPM starts from idle and increases with throttle input
- Engine temperature rises based on normalized RPM
- If temperature exceeds 100°C, the cooling system turns ON
- Cooling remains active until the temperature drops below 90°C

## Purpose

This project was created to practice embedded C++, controller input handling, real-time logic, and simple automotive-style simulation on ESP32.
