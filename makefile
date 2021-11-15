MCU=atmega328p
F_CPU=16000000

CC=avr-gcc
OBJCOPY=avr-objcopy

BUILD_DIR=build
CONF_DIR=conf/avrdude.conf
TARGET=${BUILD_DIR}/main

SRCS=$(wildcard src/*.c) $(wildcard src/*/*.c)
INCLUDES := include
INCLUDES += ${addprefix -I,include/**/}



AVRDUDE_PORT=COM3
AVRDUDE_PROGRAMMER=arduino
AVRDUDE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -v -C ${CONF_DIR}

#c11 -Wall
CFLAGS=-std=c11  -Wextra -mmcu=${MCU} -DF_CPU=${F_CPU}  -Os #-O0
CFLAGS+= -I$(INCLUDES)

SERIAL_PORT=${AVRDUDE_PORT}
BAUD_RATE=38400

all: ${TARGET}.hex

${TARGET}.hex: $(TARGET).bin
		${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

$(TARGET).bin: $(BUILD_DIR) ${SRCS}
		${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}

$(BUILD_DIR):
		mkdir ${BUILD_DIR}

flash: ${TARGET}.hex
		avrdude ${AVRDUDE_FLASH} ${AVRDUDE_FLAGS}

flash-test:
		avrdude ${AVRDUDE_FLAGS}

flash-erase:
		avrdude ${AVRDUDE_FLAGS} -e

clean:
		rmdir /s ${BUILD_DIR}

objdump: $(TARGET).bin
		avr-objdump $(TARGET).bin -D
#--visualize-jumps=extended-color | less
size:	$(TARGET).bin
		avr-size -C --mcu=$(MCU) $(TARGET).bin

terminal:
		putty -serial $(SERIAL_PORT) $(BAUD_RATE)
		