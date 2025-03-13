# Multi-Threaded Port Scanner (Windows)
A simple multi-threaded port scanner for Windows, written in C++. The program uses the Windows Sockets API (WinSock) and multithreading to efficiently scan open TCP ports on a given IP address.

# Features
Multi-Threading: Uses multiple threads for faster scanning.
Port Identification: Detects known services based on port numbers.
User-Friendly: Simple input system to specify the target IP.
# Requirements
Windows OS
Microsoft Visual Studio or another C++ compiler with WinSock2 support
# Installation & Usage
Compile:
Ensure that ws2_32.lib is linked.
Compile the code with a C++ compiler.
Run:
Start the program and enter the target IP address.
The program scans all 65,535 ports and displays open ports with their corresponding services.
⚠ Note: Scanning networks without permission may have legal consequences. Use this tool responsibly!
