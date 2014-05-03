sysMsg.so:
	gcc -Wall `pkg-config lxpanel gtk+-2.0 --cflags` -shared -fPIC systemMessage.c -o systemMessage.so `pkg-config --libs lxpanel gtk+-2.0`
clean:
	rm -rf *.so
all: systemMessage.so

install: all
	chmod 644 systemMessage.so
	cp systemMessage.so /usr/lib/*/lxpanel/plugins/

uninstall:
	rm -v /usr/lib/*/lxpanel/plugins/systemMessage.so
