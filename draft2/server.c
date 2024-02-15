#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFERSIZE 10000

#define HTTP_OK "200"
#define HTTP_NOTFOUND "404"
#define HTTP_VERSION "HTTP/1.1"
#define HTTP_GET 1

char *content;			//The buffer used for text-type data, mostly html 
/*
int *binBuffer; 		//The buffer used for binary data, such as images
						//NOTE: Use ONLY for images, html cannot be sent in binary.
*/
char *httpResponse;		//Buffer holding the http response header, append before sending data over http

/// @brief Logs errors, prints a user message to stderr
/// @param message The message
/// @param line The line of the error, use __LINE__
/// @param supress Supress user message, or just log
void logErr(const char message[], int line, bool supress)
{
	FILE *fptr;
	fptr = fopen("errout.txt", "a");
	fprintf(fptr, "%s\t At line: %d\n", message, line);
	fclose(fptr);
	if (!supress)
	{
		fprintf(stderr, "Error at line %d, logged\n", line);
	}
}

/// @brief Prints a debug message to stderr
/// @param message The message
void debugPrint(const char message[])
{
	fprintf(stderr, "%s\n", message);
}

/// @brief Throws fatal error, exits program
/// @param message The message to display
/// @param line use __LINE__
void throwErr(const char message[], int line)
{
	fprintf(stderr, "Fatal error\n");
	logErr(message, line, false);
	printf("Errno: %s\n", strerror(errno));
	exit(1);
}

/// @brief Prints Errors to stderr
/// @param message The message
/// @param line Use __LINE__
/// @param supressLog Supress logging
void printErr(const char message[], int line, bool supressLog)
{
	fprintf(stderr, "%s\n", message);
	if (!supressLog)
	{
		logErr(message, line, true);
	}
}

void printServerIP(int localFileDesc)
{

	struct sockaddr_in addr;
    	socklen_t addr_len = sizeof(addr);
    	if (getsockname(localFileDesc, (struct sockaddr *)&addr, &addr_len) == 0) 
	{
		printf("Server IP address: %s\n", inet_ntoa(addr.sin_addr));
    	}
	else {
        	perror("Error getting server IP address");
    	}
}


char *getFileExtention(char *pathToFile)
{
	return strrchr(pathToFile, '.');
}

char *getPath(char *request)
{
	return strrchr(request, ' ');
}

void getFile(char *filename)    //Puts file on either the binary or char buffer based on suffixes
                                //TODO Learn regex and pattern-match based on suffix
{
        FILE *docPointer;
        char *buffer;
        char *extention = strrchr(filename, '.');
        int readcounter=0;
		buffer = calloc(BUFFERSIZE, sizeof(char));
        docPointer = fopen(filename, "r");
        if(docPointer == NULL)
        {
                throwErr("404 File not Found!\n", __LINE__);
        }
        while(fread(buffer, sizeof(char),1, docPointer) > 0)
        {
                if(readcounter==0)
                {
                        content = malloc(strlen(buffer)*sizeof(char));
                        strcpy(content, buffer);
                        readcounter++;
                        continue;
                }
                content = realloc(content,strlen(content)*sizeof(char)+1+strlen(buffer)*sizeof(char));
                strcat(content, buffer);
                readcounter++;
        }
        free(buffer);
        //TODO Make dynamic, supporting binary encodings and switching buffer according
}
void buildResponse(char *filePath)
{
	getFile(filePath);
	printf("%s", content);
	if(content == NULL)
	{
		throwErr("content failed", __LINE__);
	}
	/*if(httpResponse!=NULL)
	{
		free(httpResponse);
	}*/
	char *extention = getFileExtention(filePath);
	char *buffer = malloc((strlen(content) + 300 + 1) * sizeof(char));
	if(strcmp(extention, ".html")==0)
	{
		sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-lenght: %ld\r\nConnection: keep-alive\r\n\r\n%s", strlen(content+1), content);
		printf("%s", buffer);
	}
	else if(strcmp(extention, ".jpg")==0)
	{
		sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-type: image/jpg\r\nContent-lenght: %ld\r\nConnection: keep-alive\r\n\r\n%s",strlen(content+1), content);
		printf("%s", buffer);
	}
	httpResponse = malloc(strlen(buffer)*sizeof(char));
	sprintf(httpResponse, "%s", buffer);
	free(content);
	free(buffer);
}



/*	WARNING: DEPRECATED
void buildHeader(char *filetype, char *filename, char *requestType)
{
	switch(requestType)
	{
		case "GET":
			struct GETResponse Builtheader;
			Builtheader.content-type = filetype;
			Builtheader.content-lenght = sizeof(content);
			break;

	}
}
*/





int main(int argc, char *argv[])
{
	switch (argc)	//Bars unintenden usage
	{
	case 1:
		printf("Too few arguments!\n");
		printf("Usage: ./server <filename/path to html doc in txt format>\n");
		exit(1);
		break;
	case 2:
		getFile(argv[1]);
		break;
	default:
		printf("Usage: ./server <filename/path to html doc in txt format>\n");
		exit(1);
	}

	int localFileDesc;
	// Server adress
	struct sockaddr_in localAdress;
	// Create socket
	if ((localFileDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		throwErr("Socket start Fail.", __LINE__);
	}
	localAdress.sin_family = AF_INET;
	localAdress.sin_addr.s_addr = INADDR_ANY;
	localAdress.sin_port = htons(PORT);
	if (bind(localFileDesc, (struct sockaddr *)&localAdress, sizeof(localAdress)) < 0)
	{
		throwErr("Bind failed", __LINE__);
	}
	if(listen(localFileDesc, 10) < 0)
	{
		throwErr("Listen failed.", __LINE__);
	}
	debugPrint("Server listening, socket + bind success.");
	printServerIP(localFileDesc);	//Prints the server IP
	while(1)
	{
		int incomingFileDesc,n;
		char *receivedRequest;				//Sting containing received
		receivedRequest = malloc(BUFFERSIZE);
		struct sockaddr_in incomingAdress;
		int incomingLenght=sizeof(incomingAdress);
		printf("Waiting for connection... %d\n", n);
		incomingFileDesc=accept(localFileDesc, (struct sockaddr *) &incomingAdress, (socklen_t *)&incomingLenght);	//Accepts incoming connections
		if(incomingFileDesc < 0)
		{
			printf("Acc. Fail");
			continue;
		}
		else
		{
			printf("Connection ACC.\n");
		}
		read(incomingFileDesc, receivedRequest, BUFFERSIZE-1);	//Reads the data received into the receivedRequest buffer
		char *path;
		char *token = strtok(receivedRequest, " \t\n");
		while (token != NULL) {
            if (strcmp(token, "GET") == 0) {
                // The next token after "GET" is the path
                token = strtok(NULL, " \t\n");
                if (token != NULL) {
                    path = token;
                    break;
                }
            }
            token = strtok(NULL, " \t\n");
        }
		printf("Path: %s", path);
		printf("\e[0;37m""%s"  "\e[0m", receivedRequest);	//Prints out the incoming request
		send(incomingFileDesc, httpResponse, strlen(httpResponse), 0); 	//TODO Refactor
								     	//Currently brute-forces the html doc
		n++;
	}
	return 0;
}
