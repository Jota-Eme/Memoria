CC=g++
CFLAGS=-Wall -std=c++11
OUT=grasp
SOURCES= main.cpp imports.h instance.cpp instance.h customer.h customer.cpp suplier.cpp suplier.h request.cpp request.h utils.h utils.cpp crossdock.h crossdock.cpp


$(OUT):
	$(CC) $(CFLAGS) -o $(OUT) $(SOURCES)

.PHONY:clean

clean:
	rm -rf *.o $(OUT)