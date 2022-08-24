CC	:=g++
CFLAGS	:=-g -Wall -Wno-parentheses
LD	:=g++
LIB	:=

SOURCES	:=$(wildcard *.c)
OBJECTS	:=$(subst .c,.o,$(SOURCES))
TARGET	:=sh3

all: $(OBJECTS) $(LIB)
	$(LD) -o $(TARGET) $(OBJECTS) $(LIB)

%.o: %.c
	@echo CC $<
	@$(CC) $(CFLAGS) -g -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)
run: all
	./$(TARGET)
