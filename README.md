
-------------------
# About
Project Description: This repository contains the files required for the implementation of 
a digital lock as described in the Project Overview document and the relevant documentation
for proper use. This readme will serve as a guiding tour of the repository and detaul necessary things
for minimil use.

Contribitor List: Cristian Pompey

--------------------
# Features

The basic features of this implementation will allow the user to unlock and lock the device.
Futher functionality can be added in the future to make it such that particular functions or 
data are only available when the device is unlocked.
--------------------
# Required Materials
- Nucleo L4R5ZI
- 4x4 Matrix keypad
- 1602A 16x2 LCD with PC8574T expander
- x4 Light emitting diodes
- STM-NUCLEO-L552ZE-Q

--------------------
# Resources and References

- https://os.mbed.com/docs/mbed-os/v6.15/introduction/index.html
- CSE-321-LCD display library
--------------------
# Getting Started

## Basic instructions for getting software set up

First you will need to clone the repository.

`
git clone https://github.com/CPompey1/CSE-321-DigitalLock.git
`

Next, build the circuit. This procedure will be detailed extensively in the project overview document.

Assuming MBED studio has been installed and setup, the user will open the project folder that has been 
cloned, connect their host computer to the NUCLEO via USB-to-USBC connection, and run the program.

Upon booting the program, lcd of the digital lock will display "Hello Cruel Worl Please wait....",
while in maintains a temporary 5 second state of no processing. Once this message is seen, it can be 
cassumed that the program is working correctly if no other changes to the source folder has been made.
The device will then be ready to input the 4 digit passcode, or change the password based on the #### inptu.

--------------------
# CSE321_project2_cipompey_main.cpp:
This will contain the main implementation and runloop of the program, all global variable declerations,
and helper functions. The specifics will be detailed in the comments of the source file.
 

	
	
