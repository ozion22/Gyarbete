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

char *getPath(char *request)
{
	char *front = strstr(request, "GET "); //Finds the pointer pointing to the first occurance of "GET "
	printf("Front: %s\n", front);
	if(front == NULL)
	{
		printf("Failed to find GET, defaulting to h.html\n");
		return "h.html"; //Failure condition
	} 
	char *back = strstr(front, " ");
	printf("Back: %s\n", back);
	if(back== NULL)
	{
		printf("Failed to find trailing whitespace, defaulting to h.html\n");
		return "h.html";
	}
	size_t pathLenght = strlen(front);
	char *path = malloc(pathLenght + 1);
	if(path == NULL)
	{
		//throwErr("Malloc failed on path", __LINE__);
	}
	printf("Front: %s\n", front);
	printf("Pathlength: %ld\n", pathLenght);
	strncpy(path, front+4, strlen(back+1));
	path[pathLenght] = '\0';
	printf("Path: %s\n", path);
	return path; 
	
}

int main()
{
	char *request = "GET h.html";
	printf("%s", getPath(request));
	return 0;
}