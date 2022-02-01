#include "common.h"

#include <thread>

#include <netdb.h>
#include <netinet/in.h>


void read_message(int sockID)
{
	int len = 0;
	int r_size = recv(sockID, &len, sizeof(len), 0);
	if(r_size < sizeof(len)) {
		return;
	}

	char *data = new char[len];

	recv(sockID, data, len, 0);

	std::cout << "Message: " << data << std::endl;

	delete [] data;
}

void read_file(int sockID)
{
	int len = 0;
	int r_size = recv(sockID, &len, sizeof(len), 0);
	if(r_size < sizeof(len)) {
		return;
	}

	char *filename = new char[len];

	recv(sockID, filename, len, 0);

	std::ofstream file(filename, std::ios::binary);

	int file_length = 0;
	recv(sockID, &file_length, sizeof(file_length), 0);

	uint8_t *content = new uint8_t[file_length];
	recv(sockID, content, file_length, 0);

	file.write((const char*)content, file_length);

	std::cout << "File received: " << filename << std::endl;

	delete [] filename;
	delete [] content;
}

void revice(int sockID)
{
	while(true)
	{
		int command = 0;
		int r_size = recv(sockID, &command, sizeof(command), 0);
		if(r_size < sizeof(command)) {
			continue;
		}

		if((Command)command == Command::MESSAGE)
		{
			read_message(sockID);
		}
		else if((Command)command == Command::FILE)
		{
			read_file(sockID);
		}
	}
}

void sending(int sockID)
{
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

                send(sockID, &request.command, sizeof(request.command), 0);
				int len = std::strlen(request.data) + 1;
                send(sockID, &len, sizeof(len), 0);
				send(sockID, request.data, len, 0);

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

					send(sockID, &request.command, sizeof(request.command), 0);
					int len = std::strlen(request.data) + 1;
					send(sockID, &len, sizeof(len), 0);
					send(sockID, request.data, len, 0);

					send(sockID, &length, sizeof(length), 0);
					send(sockID, buffer, length, 0);

					delete [] buffer;
				}
			}
		}
	}

}

int main(int argc, char** args)
{
	if(argc < 2)
	{
		return -1;
	}

    int sockID = 0;

    // server
    if(std::strcmp(args[1], "-l") == 0)
    {
        socklen_t addr_size;

        int listener = socket(PF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5588);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        bind(listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

        if(listen(listener, 1) != 0)
        {
            return -1;
        }
        std::cout << "listining" << std::endl;
        sockaddr_in cliAddr;
        socklen_t len = sizeof(cliAddr);
        sockID = accept(listener, (struct sockaddr *) &cliAddr, &len);

        std::cout << "Connection accepted" << std::endl;
    }

    // client
    if(std::strcmp(args[1], "-c") == 0)
    {
        hostent *host = gethostbyname(args[2]);

        sockID = socket(AF_INET, SOCK_STREAM, 0);
        // server address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(5588);
        std::memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

        connect(sockID, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    }
    std::thread r(revice, sockID);
    std::thread s(sending, sockID);

    r.join();
    s.join();
}
