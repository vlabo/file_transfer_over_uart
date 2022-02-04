#include "common.h"

#include <netdb.h>
#include <netinet/in.h>


void sending(int sockID, Request request)
{
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

int main(int argc, char **args)
{

    hostent *host = gethostbyname(args[1]);
    
    int sockID = socket(AF_INET, SOCK_STREAM, 0);
    // server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5590);
    std::memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    connect(sockID, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  
    if(std::strcmp(args[2], "-m") == 0)
    {
        Request request;

        request.command = Command::MESSAGE;
        std::strcpy(request.data, args[3]);
        sending(sockID, request);
    }
    else if(std::strcmp(args[2], "-f") == 0)
    {
        Request request;
        request.command = Command::FILE;
        std::strcpy(request.data, args[3]);
        sending(sockID, request);
    }


}
