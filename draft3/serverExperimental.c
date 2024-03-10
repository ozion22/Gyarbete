#include <pthread.h>
#include "server.c"
// ...

void *handle_client(void *arg)
{
    int incomingFileDesc = *(int *)arg;
    char *receivedRequest;
    receivedRequest = malloc(BUFFERSIZE); // Allocate memory for receivedRequest

    read(incomingFileDesc, receivedRequest, BUFFERSIZE - 1); // Read the request into receivedRequest

    printf("\033[34m%s\033[0m", receivedRequest);
    char *path = getPath(receivedRequest, "index.html");

    printf("Path: %s\n", path);
    if ((path == NULL) || (access(path, F_OK) != 0))
    {
        printf("Path was empty, or fnf\n");
        buildResponse("h.html", incomingFileDesc);
    }
    else
    {
        printf("Built on proper path\n");
        buildResponse(path, incomingFileDesc);
    }
    send(incomingFileDesc, httpResponse, strlen(httpResponse), 0);
    free(receivedRequest); // Free memory allocated for receivedRequest
    free(httpResponse);

    close(incomingFileDesc);

    return NULL;
}

// ...

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
    while (1)
    {
        int incomingFileDesc;
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

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, &incomingFileDesc);
        pthread_detach(thread);
    }

    return 0;
}