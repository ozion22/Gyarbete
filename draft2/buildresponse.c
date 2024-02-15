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
                printf("404 File not Found!\n");
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
	if(strcmp(extention, "html")==0)
	{
		sprintf(responseBuffer,"HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-lenght: %d\r\nConnection: keep-alive\r\n\r\n", strlen(content+1));
	}
	else if(strcmp(extention, "jpg")==0)
	{
		sprintf(responseBuffer, "HTTP/1.1 200 OK\r\nContent-type: image/jpg\r\nContent-lenght: %d\r\nConnection: keep-alive\r\n\r\n%s",strlen(content+1), content);
	}
	
	printf("BUFFER CONTENT\n\n %s\n\n END BUFFER CONTENT\n", responseBuffer);
	httpResponse = malloc(strlen(responseBuffer)*sizeof(char)+strlen(content)*sizeof(char)+1);
	strcat(httpResponse, responseBuffer);
	strcat(httpResponse, content);
	
	
	printf("%d\n",sprintf(httpResponse, "%s", responseBuffer));
	free(content);
	free(responseBuffer);
	
	printf("%s", httpResponse);
}

int main(){
	buildResponse("h.html");
	printf("%s", httpResponse);
}