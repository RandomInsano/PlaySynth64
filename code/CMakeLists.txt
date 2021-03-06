SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_FIND_ROOT_PATH  /usr/local/CrossPack-AVR)
SET(CMAKE_C_COMPILER "avr-gcc")
SET(CMAKE_CXX_COMPILER "avr-g++")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER) 
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY) 
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY) 



SET(CSTANDARD "-std=gnu99")
SET(CDEBUG "-gstabs")
SET(CWARN "-Wall -Wextra -Wstrict-prototypes")
SET(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
SET(COPT "-Os")
SET(CMCU "-mmcu=atmega16")
SET(CDEFS "-DF_CPU=8000000")

SET(CFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${CINCS} ${COPT} ${CWARN} ${CSTANDARD} ${CEXTRA}")
SET(CXXFLAGS "${CMCU} ${CDEFS} ${CINCS} ${COPT}")

SET(CMAKE_C_FLAGS ${CFLAGS})
SET(CMAKE_CXX_FLAGS ${CXXFLAGS})


# The actual program goodness
AUX_SOURCE_DIRECTORY(. SOURCES) # Need to re-run cmake on new c file(s)
ADD_EXECUTABLE(main.elf ${SOURCES}) 

ADD_CUSTOM_COMMAND(
	OUTPUT  main.hex
	COMMAND avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	DEPENDS main.elf
	COMMENT "Generating hex file"
	VERBATIM
)

ADD_CUSTOM_TARGET(
	Deployable ALL
	DEPENDS main.hex
)
