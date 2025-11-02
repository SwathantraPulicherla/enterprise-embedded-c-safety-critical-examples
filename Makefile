CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = temperature_monitor
SOURCES = main.c sensor.c utils.c embedded_hardware.c communication_protocols.c device_drivers.c safety_critical.c
HEADERS = sensor.h utils.h embedded_hardware.h communication_protocols.h device_drivers.h safety_critical.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean