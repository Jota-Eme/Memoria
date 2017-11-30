CC=g++
CFLAGS=-Wall -std=c++11
OUT=grasp
SOURCES= main.cpp imports.h globals.h instance.cpp instance.h


$(OUT):
	$(CC) $(CFLAGS) -o $(OUT) $(SOURCES)

.PHONY:clean

clean:
	rm -rf *.o $(OUT)