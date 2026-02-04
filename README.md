# Multifunctions-server Build Demo Using PlatformIO
This test application can build using
- PlatformIO
- Arduino IDE
- Arduino CLI

Test appllication uses following ZIP libraries which PlatformIO fetch "automagic" during build process into project from Github repositories
- https://github.com/BojanJurca/Thread-safe-ping-Arduino-library-for-ESP32.git
  https://github.com/BojanJurca/Lightweight-Standard-Template-Library-STL-for-Arduino.git
  https://github.com/BojanJurca/Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library.git

Repository contain some extra tool files
- _**Makefile**_  contain examples to
  -- build using Arduino CLI (**all**, default)
  -- Arduino build  "project cache" delete to force clean rebuild (**fullclean**)
  -- copy Arduino tool chain generated project's symbol table "map" file to project's main directory
  -- generate mixed source code/assembly language absolute listing of build result application (**objdump**)
  -- generate symbol table map using own command line options (**nm**)
- _**cppcheck.cppcheck**_  CppCheck project file to use external CppCheck tool to inspect project sources. Note: If include directories are properly configured in project file, then CppCheck analyse also library code/header files
