CC=g++

CFLAGS= -g -std=c++17 -Wall 

INCLUDES= -I../dragonfly/include

LDFLAGS= -L../dragonfly/lib -ldragonfly -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

SOURCES= chat.cpp \
         EventNetwork.cpp \
         Input.cpp \
         NameEntry.cpp \
         NetworkManager.cpp \
         Sentry.cpp \
         yakclient.cpp \
         yakserver.cpp

EXECUTABLE=game

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) core dragon.log dragonfly.log