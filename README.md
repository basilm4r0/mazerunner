#Micromouse Maze Solver

This is a project for a micromouse robot designed to solve a maze. The micromouse robot is a small autonomous robot capable of navigating through a maze and finding its way to the center. The robot is built using various sensors and a microcontroller that helps it make decisions as it navigates through the maze.
Installation

To use this project, you will need to have the following:

    - Arduino Programming Capable Computer
    - Micromouse robot hardware
    - Motor driver
    - Ultrasonic sensors
    - IR sensors

##Usage

To use the micromouse maze solver, follow these steps:

    1. Build the micromouse robot hardware according to the instructions provided.
    2. Connect the motor driver, ultrasonic sensors, and IR sensors to the microcontroller.
    3. Upload the code to the microcontroller using the Arduino IDE.
    4. Place the micromouse robot at the entrance of the maze and watch as it navigates to the center!

##Features

    - The micromouse robot uses ultrasonic sensors to detect obstacles in its path and IR sensors to detect the walls of the maze.
    - The robot uses an algorithm to navigate through the maze, making decisions on which path to take based on the sensor readings.
    - The code is fully customizable, allowing you to adjust the maze-solving algorithm and sensor readings to fit your specific needs.

##Contributing

If you would like to contribute to this project, please feel free to submit a pull request. We welcome all contributions and feedback!

##Mazerunner project tasks


- [x] Sensor verification
- [x] Interrupt service routines
	- [x] encoder interrupt service routine
	- [x] systick interrupt
- [x] Robot movement
	- [x] PID controller implementation
	- [x] Movement profiles
- [x] Object avoidance
- [x] Maze solving
	- [x] left hand rule
	- [x] finite state machine path optimization
