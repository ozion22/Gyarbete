#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#define PORT 8080
#define BUFFERSIZE 10000

#define HTTP_OK "200"
#define HTTP_NOTFOUND "404"
#define HTTP_VERSION "HTTP/1.1"
#define HTTP_GET 1
void send404(int socket);
char *content;		// The buffer used for text-type data, mostly html
char *httpResponse; // Buffer holding the http response header, append before sending data over http

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
	else
	{
		perror("Error getting server IP address");
	}
}

/// @brief Returns a pointer to the file extention, including '.'
/// @param pathToFile The path to the file
/// @return char* The file extention
char *getFileExtention(char *pathToFile)
{
	return strrchr(pathToFile, '.');
}

/// @brief Gets the file requested, puts on content buffer (char *content)
/// @param filename The name of the file reuqested
/// @param filedesc Filedescriptor of the requestee
/// @return Returns 0 on success, 1 on failure
int getFile(char *filename, int filedesc)
{
	FILE *docPointer;
	int readCounter = 0;

	// Open the file in binary mode for non-text files
	char *extention = getFileExtention(filename);
	docPointer = fopen(filename, "rb"); // Open file in binary modea
	if (docPointer == NULL)
	{
		printf("404 File not Found!\n");
		send404(filedesc);
		return 1;
	}

	// Get the file size
	fseek(docPointer, 0L, SEEK_END);
	long fileSize = ftell(docPointer);
	fseek(docPointer, 0L, SEEK_SET);

	// Allocate memory for content
	if (content != NULL)
	{
		content = NULL;
	}
	content = malloc((fileSize + 1) * sizeof(char));
	if (content == NULL)
	{
		printf("Memory allocation failed for content.\n");
		fclose(docPointer);
		return 1;
	}

	// Read the file into content
	size_t bytesRead = fread(content, sizeof(char), fileSize, docPointer);
	if (bytesRead != fileSize)
	{
		printf("Error reading file.\n");
		fclose(docPointer);
		free(content);
		return 1;
	}

	content[fileSize] = '\0'; // Null-terminate the content

	fclose(docPointer);
	return 0;
}

/// @brief Builds the response to the request, puts on httpResponse buffer
void buildResponse(char *filePath, int filedesc)
{
	if (filePath == NULL)
	{
		return;
	}

	if (getFile(filePath, filedesc) != 0)
	{
		return;
	}

	if (content == NULL)
	{
		printf("content failed\n");
	}

	char *extention = getFileExtention(filePath);
	char *responseBuffer = calloc((strlen(content) + 300 + 1), sizeof(char));
	if (strcmp(extention, ".html") == 0)
	{
		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: %ld\r\nConnection: keep-alive\r\n\r\n", strlen(content));
		// Allocate memory for httpResponse
		httpResponse = malloc((strlen(responseBuffer) + strlen(content) + 1) * sizeof(char));

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
	}
	else if (strcmp(extention, ".jpg") == 0)
	{
		// Get the actual size of the image file
		struct stat st;
		stat(filePath, &st);
		long fileSize = st.st_size;

		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: image/jpg\r\nContent-length: %ld\r\nConnection: keep-alive\r\n\r\n", fileSize);
		// Allocate memory for httpResponse
		httpResponse = malloc((strlen(responseBuffer) + fileSize + 1) * sizeof(char));

		// Check if memory allocation succeeded
		if (httpResponse == NULL)
		{
			printf("Memory allocation failed for httpResponse.\n");
			exit(1);
		}

		// Copy responseBuffer to httpResponse
		memcpy(httpResponse, responseBuffer, strlen(responseBuffer));

		// Copy image content to httpResponse
		memcpy(httpResponse + strlen(responseBuffer), content, fileSize);
	}

	free(responseBuffer);
}

char *getPath(char *request, char *defaultPath)
{
	// Check if request is NULL
	if (request == NULL)
	{
		printf("Request is NULL, defaulting to %s\n", defaultPath);
		return defaultPath;
	}

	// Find the start of the method
	char *methodStart = strstr(request, "GET ");
	if (methodStart == NULL)
	{
		printf("Failed to find GET, defaulting to %s\n", defaultPath);
		//methodStart = strstr()
		return defaultPath;
	}

	// Find the start of the path
	char *pathStart = methodStart + 4; // Skip "GET "

	// If the path starts with '/', skip it
	if (*pathStart == '/')
	{
		pathStart++;
	}

	// Find the end of the path
	char *pathEnd = strstr(pathStart, " HTTP/1.1");
	if (pathEnd == NULL)
	{
		printf("Failed to find end of line, defaulting to %s\n", defaultPath);
		return defaultPath;
	}

	// Calculate the length of the path
	size_t pathLength = pathEnd - pathStart;

	// Allocate memory for the path
	char *path = malloc(pathLength + 1);
	if (path == NULL)
	{
		printf("Malloc failed on path, defaulting to %s\n", defaultPath);
		return defaultPath;
	}

	// Copy the path
	strncpy(path, pathStart, pathLength);

	// Null-terminate the path
	path[pathLength] = '\0';

	return path;
}

void send404(int socket)
{
	char *notFoundResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
	ssize_t bytes_sent = send(socket, notFoundResponse, strlen(notFoundResponse), 0);
	if (bytes_sent < 0)
	{
		printErr("Failed to send 404 response", __LINE__, false);
	}
}

/// @brief Starts the server, returns the file descriptor
int startServer()
{
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
	return localFileDesc;
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

	int localFileDesc = startServer();
	printServerIP(localFileDesc);
	while (1)
	{
		int incomingFileDesc;
		char *receivedRequest;
		receivedRequest = malloc(BUFFERSIZE); // Allocate memory for receivedRequest
		struct sockaddr_in incomingAdress;
		int incomingLenght = sizeof(incomingAdress);
		printf("Waiting for connection\n");
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
		read(incomingFileDesc, receivedRequest, BUFFERSIZE - 1); // Read the request into receivedRequest

		printf("\033[34m%s\033[0m", receivedRequest);
		char *path = getPath(receivedRequest, argv[1]);

		printf("Path: %s\n", path);
		if ((path == NULL) || (access(path, F_OK) != 0))
		{
			printf("Path was empty, or fnf\n");
			buildResponse("h.html", localFileDesc);
			//send404(incomingFileDesc);
			continue;
		}
		else
		{
			printf("Built on proper path\n");
			buildResponse(path, localFileDesc);
		}
		send(incomingFileDesc, httpResponse, strlen(httpResponse), 0);
		free(receivedRequest); // Free memory allocated for receivedRequest
		free(httpResponse);
	}

	return 0;
}
