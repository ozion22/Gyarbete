
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

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

char *getHtml(char *filename)
{
	int buffer=1048576;
	char *rawText;
	rawText = malloc(buffer);
	FILE *docPointer;
	docPointer = fopen(filename, "r");
	if(docPointer == NULL)
	{
		throwErr("HTML file not Found!\n", __LINE__);
	}
	fgets(rawText, sizeof(rawText), docPointer);
	return rawText;
}

int main(int argc, char *argv[])
{
	char *html;
	switch (argc)
	{
	case 1:
		printf("Too few arguments!\n");
		printf("Usage: ./server <filename/path to html doc in txt format>\n");
		exit(1);
		break;
	case 2:
		html=getHtml(argv[1]);
		break;
	default:
		printf("Usage: ./server <filename/path to html doc in txt format>\n");
		exit(1);
	}

	printf("It worked? :O\n");
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

	while(1)
	{
		int incomingFileDesc,n;
		struct sockaddr_in incomingAdress;
		int incomingLenght=sizeof(incomingAdress);
		printf("Waiting for connection... %d\n", n);
		incomingFileDesc=accept(localFileDesc, (struct sockaddr *) &incomingAdress, (socklen_t *)&incomingLenght);
		if(incomingFileDesc < 0)
		{
			printf("Acc. Fail");
		}
		send(incomingFileDesc, html, strlen(html), 0);
		n++;
	}
	return 0;
}
