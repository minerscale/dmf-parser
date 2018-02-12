TARGET = out/libdmf.so
LIBS = 
CC = gcc
CFLAGS = -Wall -Iinc -fPIC -O3
CHMOD = chmod

.PHONY: default all clean

default: $(TARGET)
all: default

HEADERS = $(wildcard *.h)
HEADERS += $(wildcard inc/*.h)

SRC_C =  $(wildcard *.c)
SRC_C += $(wildcard src/*.c)

OBJECTS = $(addprefix out/, $(SRC_C:.c=.o))

pre-build:
	mkdir -p out/
	mkdir -p out/src

out/%.o: %.c $(HEADERS) pre-build
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -shared $(CFLAGS) $(LIBS) -o $@
	$(CHMOD) +x $@

clean:
	-rm -f out/*.o
	-rm -f out/src/*.o
	-rm -f $(TARGET)
