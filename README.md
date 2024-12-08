# Embedded Games Project

The **"Embedded Games"** project is a collection of minigames developed using technologies learned throughout the quarter. The core hardware and technologies include:

- **CC3200 Microcontroller**: Central hub for communication between components.
- **Adafruit OLED Breakout Board**: Displays the graphical interface for games and menus.
- **AT&T S10-S3 Remote with Vishay Receiver**: Enables menu navigation and button-based gameplay for specific minigames.
- **Onboard Accelerometer**: Used for tilt-based motion control in certain games.
- **Amazon Web Services (AWS)**: Facilitates score and name submissions to an online leaderboard, integrated with email forwarding and ServiceNow®.

## Key Features

### 1. Minigames and Controls
- Games are played using either tilt controls (accelerometer), remote buttons, or a combination of both.
- Graphics and interface are implemented using the Adafruit GFX library, with communication via SPI and I2C protocols.

### 2. Graphical User Interface (GUI)
- A main menu provides two options:
  - **Play Games**: Starts the game sequence.
  - **How to Play**: Displays instructions.
- Scores are updated dynamically, with a prompt at the end for name entry and leaderboard submission.

### 3. Leaderboard Integration
- After gameplay, users can upload their score and name via HTTP POST requests to AWS, enabling display on a web-based leaderboard.

### 4. Usage
- The device is portable and requires only a USB power source.
- To restart, users must either recompile the program or replug the device after completion.

---

This project showcases the integration of embedded systems hardware and software development.

Website URL: https://sites.google.com/ucdavis.edu/eec172-embedded-games
