#include "common.h"
#include <rc/uart.h>

#include <thread>


void read_message(int bus)
{
	int len = 0;
	int r_size = rc_uart_read_bytes(bus,(uint8_t*) &len, sizeof(len));
	if(r_size < sizeof(len)) {
		return;
	}

	char *data = new char[len];

	rc_uart_read_bytes(bus,(uint8_t*) data, len);

	std::cout << "Message: " << data << std::endl;

	delete [] data;
}

void read_file(int bus)
{
	int len = 0;
	int r_size = rc_uart_read_bytes(bus,(uint8_t*) &len, sizeof(len));
	if(r_size < sizeof(len)) {
		return;
	}

	char *filename = new char[len];

	rc_uart_read_bytes(bus,(uint8_t*) filename, len);

	std::ofstream file(filename, std::ios::binary);

	int file_length = 0;
	rc_uart_read_bytes(bus, (uint8_t*) &file_length, sizeof(file_length));

	uint8_t *content = new uint8_t[file_length];
	rc_uart_read_bytes(bus, content, file_length);

	file.write((const char*)content, file_length);

	std::cout << "File recived: " << filename << std::endl;

	delete [] filename;
	delete [] content;
}

void revice(int bus)
{
	while(true)
	{
		int command = 0;
		int r_size = rc_uart_read_bytes(bus, (uint8_t*)&command, sizeof(command));
		if(r_size < sizeof(command)) {
			continue;
		}

		if((Command)command == Command::MESSAGE)
		{
			read_message(bus);
		}
		else if((Command)command == Command::FILE)
		{
			read_file(bus);
		}
	}
}


int main(int argc, char** args)
{
	if(argc != 2)
	{
		return -1;
	}

	int bus = std::atoi(args[1]);

	rc_uart_init(bus, 9600, 1, 0, 1, 0);

	std::thread t(revice, bus);
	
	while(true)
	{
		std::ifstream pipe("server_pipe");

		if(pipe.is_open())
		{
			Request request;
			pipe >> request;
			

			if(request.command == Command::MESSAGE)
			{
				std::cout << "Message: " << request.data << std::endl;

				rc_uart_write(bus, (uint8_t*)&request.command, sizeof(request.command));
				int len = std::strlen(request.data) + 1;
				rc_uart_write(bus, (uint8_t*)&len, sizeof(len));
				rc_uart_write(bus, (uint8_t*)request.data, len);

			}
			else if(request.command == Command::FILE)
			{
				std::cout << "File: " << request.data << std::endl;
				std::ifstream file(request.data);

				if(file.is_open())
				{
					file.seekg(0, file.end);
					int length = file.tellg();
					file.seekg(0, file.beg);

					char * buffer = new char[length];
					file.read(buffer, length);

					rc_uart_write(bus, (uint8_t*)&request.command, sizeof(request.command));
					int len = std::strlen(request.data) + 1;
					rc_uart_write(bus, (uint8_t*)&len, sizeof(len));
					rc_uart_write(bus, (uint8_t*)request.data, len);

					rc_uart_write(bus, (uint8_t*)&length, sizeof(length));
					rc_uart_write(bus, (uint8_t*)buffer, length);

					delete [] buffer;
				}
			}
		}
	}

	rc_uart_close(bus);
}
