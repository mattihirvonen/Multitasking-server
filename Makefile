# Windows:
# Use CodeBlocks / MinGW application package adopted GNU make (mingw32-make.exe),
# which copied to "gmake.exe" and location directory is added to command prompt PATH variable.

BOARD_ESP32DEV = esp32:esp32:esp32
BOARD_ESP32S3  = esp32:esp32:XIAO_ESP32S3
BOARD         := $(BOARD_ESP32DEV)

USER     := mattihirvonen
HOMEPATH := \Users\$(USER)

SKETCH   := ECD8CD5EEBA8A348321F950ED73BE011
INOFILE  := Multitasking-server.ino

# Example: --build-property "build.extra_flags=\"-DMY_DEFINE=\"hello world\"\""

DFLAGS := -g
WFLAGS := -Wall -Wextra
CFLAGS := --build-property "build.extra_flags= $(WFLAGS) $(DFLAGS)"

IPATHS  = -I \work\BojanJurca\Lightweight-Standard-Template-Library-STL-for-Arduino\src
IPATHS += -I \work\BojanJurca\Thread-safe-ping-Arduino-library-for-ESP32\src
IPATHS += -I \work\BojanJurca\Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library

INCLUDES := --build-property "build.extra_flags= $(IPATHS)"

# Note: TOOLSPATH depend on ESP32 CPU variant !!!

TOOLSPATH := esp-x32\2511\bin\xtensa-esp32-elf-

NM      = $(HOMEPATH)\AppData\Local\Arduino15\packages\esp32\tools\$(TOOLSPATH)nm.exe
OBJDUMP = $(HOMEPATH)\AppData\Local\Arduino15\packages\esp32\tools\$(TOOLSPATH)objdump.exe

ELF     = $(HOMEPATH)\AppData\Local\arduino\sketches\$(SKETCH)\$(INOFILE).elf
MAP     = $(HOMEPATH)\AppData\Local\arduino\sketches\$(SKETCH)\$(INOFILE).map


all: build


build:
	arduino-cli compile -b $(BOARD) $(CFLAGS) -v
#	arduino-cli compile -b $(BOARD) $(DFLAGS) $(WFLAGS) -v
#	arduino-cli compile -b $(BOARD) $(DFLAGS) $(WFLAGS) $(INCLUDES) -v

help:
	arduino-cli help

boards:
	arduino-cli board listall

objdump:
	$(OBJDUMP) -x -l -S $(ELF)  >objdump.lst

nm:
	$(NM) -l $(ELF)  >nm.lst

map:
	copy $(MAP) .

#-------------------------------------------------------------------------------

# This do not equal normal project clean for rebuild application sorces from scratch !!!
# Arduino's "cache clean" clean only applications previously built libraries cache NOT application itself
#
libclean:
	arduino-cli cache clean

fullclean: libclean
	del /S /Q  $(HOMEPATH)\AppData\Local\arduino\sketches\$(SKETCH)\*
