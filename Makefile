# Windows:
# Use CodeBlocks / MinGW application package adopted GNU make (mingw32-make.exe),
# which copied to "gmake.exe" and location directory is added to command prompt PATH variable.

BOARD_ESP32DEV = esp32:esp32:esp32
BOARD_ESP32S3  = esp32:esp32:XIAO_ESP32S3
BOARD         := $(BOARD_ESP32DEV)

# Replace "USER" , "SKETCH" <nd "INOFILE" data contents with your actual data !!!
# - "USER"      your windows user name
# - "SKETCH"    check actual string from Arduino IDE/CLI build
# - "INOFILE"   get Arduino tool chain to work properly (map file and objdump generated absolute listing)
#
# Here is used syntax, where advanced users can give these data contents from command line,
# but it is out of this scope

USER     := mattihirvonen
SKETCH   := ECD8CD5EEBA8A348321F950ED73BE011
INOFILE  := Multitasking-server.ino

HOMEPATH := \Users\$(USER)

# Example: --build-property "build.extra_flags=\"-DMY_DEFINE=\"hello world\"\""

DFLAGS  := -g
WFLAGS  := -Wall  -Wextra
CFLAGS  := --build-property "build.extra_flags= $(WFLAGS) $(DFLAGS)"
WALL    := --build-property "build.extra_flags= -Wall $(DFLAGS)"
WEXTRA  := --build-property "build.extra_flags= -Wall -Wextra $(DFLAGS)"

IPATHS  = -I \work\BojanJurca\Lightweight-Standard-Template-Library-STL-for-Arduino\src
IPATHS += -I \work\BojanJurca\Thread-safe-ping-Arduino-library-for-ESP32\src
IPATHS += -I \work\BojanJurca\Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library

INCLUDES := --build-property "build.extra_flags= $(IPATHS)"

# Note(s): 
# - TOOLSPATH depend on ESP32 CPU variant (following example work with ESP32 Dev Module)

TOOLSPATH := esp-x32\2511\bin\xtensa-esp32-elf-

NM      = $(HOMEPATH)\AppData\Local\Arduino15\packages\esp32\tools\$(TOOLSPATH)nm.exe
OBJDUMP = $(HOMEPATH)\AppData\Local\Arduino15\packages\esp32\tools\$(TOOLSPATH)objdump.exe

ELF     = $(HOMEPATH)\AppData\Local\arduino\sketches\$(SKETCH)\$(INOFILE).elf
MAP     = $(HOMEPATH)\AppData\Local\arduino\sketches\$(SKETCH)\$(INOFILE).map


all: default


# Arduino's default build disable all compiler's default warninggs with command line option "-w"
default:
	arduino-cli compile -b $(BOARD) -v
#	arduino-cli compile -b $(BOARD) $(DFLAGS) $(WFLAGS) -v
#	arduino-cli compile -b $(BOARD) $(DFLAGS) $(WFLAGS) $(INCLUDES) -v

wall:
	arduino-cli compile -b $(BOARD) $(WALL) -v

wextra:
	arduino-cli compile -b $(BOARD) $(WEXTRA) -v

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
