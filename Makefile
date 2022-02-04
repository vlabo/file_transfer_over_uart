all: server client

server: server.cpp
	g++ server.cpp -g -o server -lpthread

client: client.cpp
	g++ client.cpp -g -o client

device1: server client
	mkdir -p device1
	cp server device1/
	cp client device1/
	rm -f device1/server_pipe
	mkfifo device1/server_pipe

device2: server client
	mkdir -p device2
	cp server device2/
	cp client device2/
	rm -f device2/server_pipe
	mkfifo device2/server_pipe

clean:
	rm -f server client
	rm -fr device1 device2
