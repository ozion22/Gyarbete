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

/// @brief Prints the ServerIP to the console
/// @param localFileDesc The file descriptor of the server
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




void getFile(char *filename) {
    FILE *docPointer;
    char *buffer;
    char *extension = strrchr(filename, '.');
    int readCounter = 0;

    docPointer = fopen(filename, "r");
    if (docPointer == NULL) {
        printf("404 File not Found!\n");
        return;
    }

    // Get the file size
    fseek(docPointer, 0L, SEEK_END);
    long fileSize = ftell(docPointer);
    fseek(docPointer, 0L, SEEK_SET);

    // Allocate memory for content
	if (content != NULL) {
        free(content);
        content = NULL;
    }
    content = malloc((fileSize + 1) * sizeof(unsigned char));
    if (content == NULL) {
        printf("Memory allocation failed for content.\n");
        fclose(docPointer);
        return;
    }

    // Read the file into content
    size_t bytesRead = fread(content, sizeof(unsigned char), fileSize, docPointer);
    if (bytesRead != fileSize) {
        printf("Error reading file.\n");
        fclose(docPointer);
        free(content);
        return;
    }

    // Null-terminate the content string
    content[fileSize] = '\0';

    fclose(docPointer);
}

void buildResponse(char *filePath)
{
	if(filePath == NULL)
	{
		return; 
	}
	getFile(filePath);
	//printf("%s\n stlen:%ld", content, strlen(content));
	if(content == NULL)
	{
		printf("content failed\n");
	}
	unsigned char *extention = getFileExtention(filePath);
	unsigned char *responseBuffer = calloc((strlen(content) + 300 + 1), sizeof(unsigned char));
	printf("Allocated:%ld, size: %ld",strlen(content) + 300 + 1, sizeof(responseBuffer)*strlen(responseBuffer));
	if (strcmp(extention, ".html") == 0)
	{
		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: %ld\r\nConnection: keep-alive\r\n\r\n", strlen(content));
	}
	else if (strcmp(extention, ".jpg") == 0)
	{
		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: image/jpg\r\nContent-length: %ld\r\nConnection: keep-alive\r\n\r\n%s", strlen(content), content);
	}

	//printf("HTTP Response Header: %s\n", responseBuffer); // Print HTTP response header for debugging

	// Allocate memory for httpResponse
	httpResponse = malloc((strlen(responseBuffer) + strlen(content) + 1) * sizeof(unsigned char));

	// Check if memory allocation succeeded
	if (httpResponse == NULL)
	{
		printf("Memory allocation failed for httpResponse.\n");
		exit(1);
	}

	// Copy responseBuffer to httpResponse
	strcpy(httpResponse, responseBuffer);

	// Concatenate content to httpResponse
	strcat(httpResponse, content);

	//printf("HTTP Response: %s\n", httpResponse); // Print the complete HTTP response for debugging
	free(responseBuffer);
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


char *getPath(char *request)
{
	printf("Received reques:\n%s", request);
	char *front = strstr(request, "GET "); //Finds the pointer pointing to the first occurance of "GET "
	if(front == NULL)
	{
		printf("Failed to find GET, defaulting to h.html\n");
		return "h.html"; //Failure condition
	} 
	char *back = strstr(front, " ");
	if(back== NULL)
	{
		printf("Failed to find trailing whitespace, defaulting to h.html\n");
		return "h.html";
	}
	size_t pathLenght = back-front;
	char *path = malloc(pathLenght + 1);
	if(path == NULL)
	{
		throwErr("Malloc failed on path", __LINE__);
	}
	strncpy(path, front, pathLenght);
	path[pathLenght] = '\0';
	
	return path; 
	
}



int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        printf("Too few arguments!\n");
        printf("Usage: ./server <filename/path to html doc in txt format>\n");
        exit(1);
        break;
    case 2:
        break;
    default:
        printf("Usage: ./server <filename/path to html doc in txt format>\n");
        exit(1);
    }

    int localFileDesc;
    struct sockaddr_in localAdress;
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
    if (listen(localFileDesc, 10) < 0)
    {
        throwErr("Listen failed.", __LINE__);
    }
    debugPrint("Server listening, socket + bind success.");
    printServerIP(localFileDesc);

    while (1)
    {
        int incomingFileDesc, n;
        char *receivedRequest;
        receivedRequest = malloc(BUFFERSIZE);
        struct sockaddr_in incomingAdress;
        int incomingLenght = sizeof(incomingAdress);
        printf("Waiting for connection... %d\n", n);
        incomingFileDesc = accept(localFileDesc, (struct sockaddr *)&incomingAdress, (socklen_t *)&incomingLenght);
        if (incomingFileDesc < 0)
        {
            printf("Acc. Fail");
            continue;
        }
        else
        {
            printf("Connection ACC.\n");
        }
        read(incomingFileDesc, receivedRequest, BUFFERSIZE - 1);
		/*
        char *path=getPath(receivedRequest);
        
        printf("Path: %s\n", path);
        if ((path == NULL) || (access(path, F_OK)!=0))
        {
            buildResponse("h.html");
        }
        else
        {
			printf("Built on proper path\n");
            buildResponse(path);
        }
		*/
		buildResponse("h.html");
        send(incomingFileDesc, httpResponse, strlen(httpResponse), 0);
        //free(receivedRequest); // Free memory allocated for receivedRequest
		free(httpResponse);
		buildResponse("push.jpg");
		send(incomingFileDesc, httpResponse, strlen(httpResponse), 0);
		//free(receivedRequest); // Free memory allocated for receivedRequest
		free(httpResponse);
        n++;
    }

    return 0;
}
