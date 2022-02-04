#include "common.h"

#include <thread>
#include <vector>

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

    int command = 0;
    int r_size = recv(sockID, &command, sizeof(command), 0);
    if(r_size < sizeof(command))
    {
        return;
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


int main(int argc, char** args)
{
    socklen_t addr_size;

    int listener = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5590);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if(listen(listener, 10) != 0)
    {
        return -1;
    }

    std::vector<std::thread> threads;
    while(true)
    {
        std::cout << "listining" << std::endl;
        sockaddr_in cliAddr;
        socklen_t len = sizeof(cliAddr);
        int sockID = accept(listener, (struct sockaddr *) &cliAddr, &len);

        std::cout << "Connection accepted" << std::endl;

        threads.emplace_back(revice, sockID);
    }
}
