CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = temperature_monitor
SOURCES = main.c src/sensor.c src/utils.c src/embedded_hardware.c src/communication_protocols.c src/device_drivers.c src/safety_critical.c
HEADERS = src/sensor.h src/utils.h src/embedded_hardware.h src/communication_protocols.h src/device_drivers.h src/safety_critical.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean