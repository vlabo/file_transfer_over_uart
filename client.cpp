#include "common.h"

int main(int argc, char **args)
{
	if(argc != 3) {
		return -1;
	}

	std::ofstream pipe("server_pipe");

	if(pipe.is_open()) 
	{
		if(std::strcmp(args[1], "-m") == 0)
		{
			Request request;

			request.command = Command::MESSAGE;
			std::strcpy(request.data, args[2]);
			pipe << request;
		} 
		else if(std::strcmp(args[1], "-f") == 0)
		{
			Request request;
			request.command = Command::FILE;
			std::strcpy(request.data, args[2]);
			pipe << request;
		}
	}

}
