CSOPESY Machine Project - Process Scheduler Simulator
======================================================

Description:
------------
This project simulates a process scheduler with multiple CPUs. It implements various scheduling algorithms and provides a command-line interface for interacting with the system.

Features:
---------
1. Multiple CPU simulation
2. Process creation and management
3. Various scheduling algorithms (FCFS, RR)
4. Real-time process monitoring
5. System utilization reporting
6. Configurable simulation parameters

Setup:
------
1. Ensure you have a C++ compiler that supports C++11 or later.
2. Clone the repository or extract the project files.
3. Compile the project using your preferred C++ compiler or IDE.
4. Run the compiled executable.

Configuration:
--------------
Edit the 'config.txt' file to adjust simulation parameters:
- num-cpu: Number of CPU cores
- scheduler: Scheduling algorithm ("fcfs" or "rr")
- quantum-cycles: Time quantum for Round Robin (if applicable)
- batch-process-freq: Frequency of automatic process creation
- min-ins: Minimum number of instructions per process
- max-ins: Maximum number of instructions per process
- delay-per-exec: Delay between instruction executions

Usage:
------
Main commands:
- screen -s [process_name]: Create and switch to a new process
- screen -r [process_name]: Switch to an existing process
- screen -ls: List all active processes
- scheduler-test: Start automatic process creation
- scheduler-stop: Stop automatic process creation
- report-util: Generate system utilization report
- marquee: Switch to marquee display mode
- exit: Exit the current console or the program

Process-specific commands:
- process-smi: Show detailed process metrics
- exit: Return to the main console

Initial Setup and Commands:
--------------------------
1. Launch the program
2. Type 'initialize' to start the system
   - This loads configuration from config.txt
   - Sets up CPUs and scheduler
   - Initializes the marquee display
3. After initialization, you can use all other commands

Basic Command Flow:
root:\> initialize    # First command to start the system
root:\> screen -ls    # Check system status
root:\> scheduler-test    # Begin automatic processing

Notes:
------
- The system starts in manual mode. Use 'scheduler-test' to begin automatic process creation.
- Use 'screen' commands to manage and monitor individual processes.
- The 'report-util' command provides an overview of system status and process states.

Contributors:
-------------
DELA CRUZ, ANGELO RICHTER 
JAVELLANA, MAC ANDRE 
KUA, MIGUEL CARLO  
SANG, NATHAN IMMANUEL 

Version:
--------
1.0.0

Last Updated:
-------------
NOV 4, 2024
