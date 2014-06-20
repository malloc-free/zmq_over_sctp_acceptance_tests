CC=gcc
DOC=doxygen
DOC_FILE=Doxyfile
CFLAGS=-c -Wall -D_GNU_SOURCE `pkg-config --cflags glib-2.0`
LDFLAGS=-D_GNU_SOURCE
LIBRARIES=`pkg-config --libs glib-2.0` -lzmq
SOURCES=src/zmq_acceptance.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=acc_test
all: $(SOURCES) $(EXECUTABLE) $(SO_LIB)

executable: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBRARIES) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

check:
	./$(EXECUTABLE)
	
clean:
	rm -rf src/*.o acc_test
	
doxygen:
	$(DOC) $(DOC_FILE)
	
install:
	export LD_LIBRARY_PATH=$(DIR):$$LD_LIBRARY_PATH
