# Multifunctions-server Build Demo Using PlatformIO
**_READ THIS DOCUMENT AT LEAST THREE TIMES BEFORE DOING ANYTHING ELSE_**

This test repository/application was build WITHOUT ANY modifications using
- PlatformIO
- Arduino IDE
- Arduino CLI

Test appllication uses following ZIP libraries which PlatformIO fetch "automagic" during build process into project from Github repositories
- https://github.com/BojanJurca/Thread-safe-ping-Arduino-library-for-ESP32.git
- https://github.com/BojanJurca/Lightweight-Standard-Template-Library-STL-for-Arduino.git
- https://github.com/BojanJurca/Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library.git

Project's application workhorse is from ZIP library examples copied (and renamd) file
- src/A_minimal_FTP_server.cpp

## STATUS:
- 2026-02-09 (morning): Projects's Arduino and PlatformIO builds from scratch have gone broken after update ZIP libraries from Github 

## DO NOT !!!
- Do not change or modify project's top directory file **_Multitasking-server.ino_** to contain functions
  - setup()
  - loop()

## PlatformIO
PlatformIO users expected to be familiar to modify project's configuration file **platformio.ini**

## Extra tools
Repository contain some extra tool files
- _**Makefile**_  contain examples to
  - build using Arduino CLI (**all**, default)
  - Arduino build "project cache" delete to force clean rebuild (**fullclean**)
  - copy Arduino tool chain generated project's symbol table "map" file to project's main directory (**map**)
  - generate mixed source code/assembly language absolute listing of build result application (**objdump**)
  - generate symbol table map using own command line options (**nm**)
- _**cppcheck.cppcheck**_  CppCheck project file to use external CppCheck tool to inspect project sources. Note: If include directories are properly configured in project file, then CppCheck analyse also library code/header files

## Makefile
Makefile contain now only Windows "Command Prompt" directory path examples. Linux users can modidy paths him/her self. Ideas can find from my **hdrcopy** project.

## Force Clean Build with Arduino IDE/CLI
Arduino build use "internal compile cache", which is difficult to clean for new fresh build from scratch.
There are two methods
- IDE/CLI method
  - switch build target board type
  - execute new verify (do not care ebout possible errors)
  - switch back to original target board
  - execute new verify
- "brutal" cache delete under the hood
  - check sketch directory name (32 hex digit) from Arduino build
  - modify Makefile USER and SKETCH definitions
  - execute from command line **make fullclean**

## CppCheck
Arduino library header and source files for CppCheck analyse's include directories/files can collect using my companion project
- https://github.com/mattihirvonen/hdrcopy 
