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
    content = malloc((fileSize + 1) * sizeof(char));
    if (content == NULL) {
        printf("Memory allocation failed for content.\n");
        fclose(docPointer);
        return;
    }

    // Read the file into content
    size_t bytesRead = fread(content, sizeof(char), fileSize, docPointer);
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
	getFile(filePath);
	printf("%s\n stlen:%ld", content, strlen(content));
	if(content == NULL)
	{
		printf("content failed\n");
	}
	/*if(httpResponse!=NULL)
	{
		free(httpResponse);
	}*/
	char *extention = getFileExtention(filePath);
	char *responseBuffer = calloc((strlen(content) + 300 + 1), sizeof(char));
	printf("Allocated:%ld, size: %ld",strlen(content) + 300 + 1, sizeof(responseBuffer)*strlen(responseBuffer));
	if (strcmp(extention, ".html") == 0)
	{
		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: %ld\r\nConnection: keep-alive\r\n\r\n", strlen(content));
	}
	else if (strcmp(extention, ".jpg") == 0)
	{
		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: image/jpg\r\nContent-length: %ld\r\nConnection: keep-alive\r\n\r\n%s", strlen(content), content);
	}

	printf("HTTP Response Header: %s\n", responseBuffer); // Print HTTP response header for debugging

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

	printf("HTTP Response: %s\n", httpResponse); // Print the complete HTTP response for debugging
	free(content);
	free(responseBuffer);
}

int main(){
	buildResponse("h.html");
	printf("%s", httpResponse);
}