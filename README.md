# Embedded Minigames Project

The **"Embedded Minigames"** project is a collection of minigames developed using technologies learned throughout the quarter in EEC 172 (Embedded Systems). The core hardware and technologies include:

- **CC3200 Microcontroller**: Central hub for communication between components.
- **Adafruit OLED Breakout Board**: Displays the graphical interface for games and menus.
- **AT&T S10-S3 Remote with Vishay Receiver**: Enables menu navigation and button-based gameplay for specific minigames.
- **Onboard Accelerometer**: Used for tilt-based motion control in certain games.
- **Amazon Web Services (AWS)**: Facilitates score and name submissions to an online leaderboard, integrated with email forwarding and ServiceNow®.

## Key Features

### 1. Minigames and Controls
- Games are played using either tilt controls (accelerometer), remote buttons, or a combination of both.
- Graphics and interface are implemented using the Adafruit GFX library, with communication via SPI and I2C protocols.

### 2. User Flow
- A main menu provides two options:
  - **Play Games**: Starts the game sequence.
  - **How to Play**: Displays instructions. Can go back to Main Menu if desired.
- Minigame Function Call: A predetermined sequence of minigame function calls now starts
  -  Full list of minigames on project website.
- Scores are updated dynamically, with a prompt at the end for name entry and leaderboard submission using Amazon Web Services, email forwarding, and ServiceNow®.

### 3. Leaderboard Integration
- After gameplay, users can upload their score and name via HTTP POST requests using Amazon Web Services (AWS), enabling display on a web-based leaderboard. This was accomplished by using ServiceNow® as a record keeping platform.

### 4. Usage
- The device is portable and requires only a USB power source.
- However, the breadboard and proper hardware peripherals must be wired properly. (Full list above and bill of materials is listed on project website)
- To restart, users must either recompile the program or replug the device after completion.

---

This project showcases the integration of embedded systems hardware and software development. Hardware/software debugging and troubleshooting was paramount.

Project Website: https://sites.google.com/ucdavis.edu/eec172-embedded-games

