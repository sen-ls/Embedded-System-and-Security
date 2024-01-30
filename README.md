# Embedded-System-and Security Lab 

## Lab1: Morse Code LED Transmitter

This project implements a Morse code transmission system using an LED to represent the Morse code signals. It is designed for a microcontroller platform where `LED1` is used for signaling, and `Button1` and `Button2` are used for user inputs. The Morse code follows the ITU standard M.1677-1 for letters and numbers.

### Part A: Continuous Morse Code Transmission

#### Features

- Transmits the string "I CAN MORSE" in Morse code via `LED1`.
- The string is sent repeatedly while the device is powered on.
- A pause of 5 seconds is implemented after each transmission.
- The dot length is set to 100ms±10ms.

#### Getting Started

1. Clone the repository to your local machine.
2. Rename one of the example projects to `PartA`.
3. Replace the example code with the Morse code transmission solution.
4. Compile and upload the code to your microcontroller.

### Part B: Morse Code Transmission on Button Press

#### Features

- The Morse code string is sent only once when `Button1` is pressed down.
- The time interval in milliseconds between the last and the second last press of `Button1` is sent once in Morse code via `LED1` when `Button2` is pressed down.
  - If `Button1` has not been pressed, a single `0` digit is sent.
  - If `Button1` has only been pressed once, the time since boot up until the press is sent.
- The time is sent in decimal notation without leading zeros or decimal points, rounded to full milliseconds.
- During an ongoing Morse code transmission, any button press may be ignored.
- If both buttons are pressed simultaneously, the one pressed first is given priority, and it locks out the other until released.

#### Setup for Part B

1. Copy the project from `PartA` and rename the copy to `PartB`.
2. Extend the code with the additional features for button press functionality.
3. Ensure that timing measurement does not interfere with the Morse code transmission.
4. Consider using SysTick or CCU4 for managing time between button presses.

### Development Environment

- Xilinx Vivado Design Suite or equivalent
- A compatible microcontroller board with LED and button inputs

### Usage

- Power on the device to start the continuous Morse code transmission in `Part A`.
- Press `Button1` to send the Morse code string once in `Part B`.
- Press `Button2` to send the time interval between button presses in Morse code.

## Lab2: Secure Microcontroller Communication Exploit and Patch Project

### Overview

- **Part A**: Exploit a stack-based buffer overflow vulnerability to turn on LEDs by code injection.
- **Part B**: Bypass the MPU and stack protection features to activate a blinking function.
- **Part C**: Replace the homebrew encryption with the `crypto_secretbox_easy()` function from libsodium.

### Prerequisites

- XMC microcontroller board
- Vivado or equivalent development environment
- Precompiled libsodium 1.0.15 library

### Part A: Code Injection Exploit

The program uses a custom crypto algorithm vulnerable to a stack-based buffer overflow. You are tasked with creating an exploit to turn on the LEDs without causing any noticeable malfunction or interruption to the system's operation.

#### Deliverable

- Binary file named `exploit` that contains the necessary code to perform the exploit.
- The exploit should execute upon completion of the vulnerable function.

### Part B: Bypassing MPU and Stack Protection

Despite enabling the MPU and activating stack protection, the system remains vulnerable. Your exploit should demonstrate that these measures are insufficient by triggering the program's blinking function.

#### Deliverable

- Binary file named `exploit` that activates the blinking function without disrupting the normal operation of the application.

### Part C: Implementing Libsodium

After demonstrating the vulnerabilities in the original code, replace the custom encryption with the libsodium library's `crypto_secretbox_easy()` function, using the nonce from the packet parser and a key derived from the "unique chip id."

#### Instructions

1. Replace the homebrew crypto algorithm with the `crypto_secretbox_easy()` function from libsodium.
2. Use the packet parser to obtain the 192-bit nonce required by the function.
3. Construct the 256-bit key by concatenating the 128-bit "unique chip id" to itself.
4. Ensure the application continues to work as expected after the replacement.

## Lab3: The authenticator accepts three optional command line arguments:
- `-t`: Sets the timing side channel per character in milliseconds. Default: 20ms.
- `-l`: Sets the password length. Default: A random number between 10 and 20 characters.
- `-c`: Disables program termination by Ctrl-C. Enabled on the grading system.

### Part A: Modeling an HID Keyboard

#### Prerequisites

- XMC4500 board
- LUFA USB stack
- XMCLib
- Tiny Core Linux machine with the provided authenticator program

#### Instructions

1. Use the `example_hid_project` as a starting point.
2. Modify the project to exploit the timing side channel and intelligently crack the password.
3. After breaching the authenticator, the HID keyboard must:
   - Create a text file named after your matriculation number.
   - Place the text file in the `$HOME` directory.
   - Ensure the text file contains your full name.

#### Usage

To execute the password cracking procedure:
