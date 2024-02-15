#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "/home/ozion/C/headers/timstandard.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define clear printf("\033c")

struct Book
{
    char title[32];
    char author[32];
    int pages;
};

/* global variables */
int numberOfBooks;
struct Book *books;
int storagePointer;
long storageSize;

void printBookContents(int index)
{
    printf("%-32s | %32s | %-32d\n", books[index].title, books[index].author, books[index].pages);
}

/// @brief Writes contents of *books to file "booklist.bin"
void writebooks()
{
    int i = 0;
    int del = remove("booklist.bin");
    if (del)
    {
        logErr("Could not Clear storage file", __LINE__, false);
    }
    storagePointer = open("/home/ozion/codeplanet/C/books/booklist.bin", O_WRONLY | O_CREAT, 00644);
    if (storagePointer < 1)
    {
        logErr("Storage File could not be opened", __LINE__, false);
        return;
    }
    while (i < numberOfBooks)
    {
        write(storagePointer, &books[i], sizeof(struct Book));
        i++;
    }
    close(storagePointer);
}

int getIndexToDelete()
{
    char temp[32];
    printf(ANSI_BOLD "Which book to delete? (0 to cancel)\n" ANSI_RESET);
    for (int i = 0; i < (numberOfBooks); i++)
    {
        printf("%d | %s | %s\n", i + 1, books[i].title, books[i].author);
    }
    printf(">");
    fgets(temp, 31, stdin);
    debugPrint("Finished getting index");
    printf("%d", atoi(temp) - 1);
    return atoi(temp) - 1;
}

void removebook(int index)
{
    if (index != 0)
        memcpy(books, books, index * sizeof(struct Book)); // copy everything BEFORE the index
    if (index != (numberOfBooks - 1))
        memcpy(books, books, (numberOfBooks - index - 1) * sizeof(struct Book));
    numberOfBooks--;
    if (numberOfBooks > 0)
    {
        books = (struct Book *)realloc(books, sizeof(struct Book) * numberOfBooks);
    }
    else if (numberOfBooks == 0)
    {
        free(books);
        debugPrint("Freed books.");
    }
    else
    {
        throwErr("Unknown Error, ending process to avoid data loss", __LINE__);
    }
    writebooks();
}

void getbooks()
{
    storagePointer = open("/home/ozion/codeplanet/C/books/booklist.bin", O_RDONLY | O_CREAT);
    char buffer[500];
    int counter = 0;
    int bytesread;
    if (storagePointer < 1)
    {
        logErr("Storage file failed to open", __LINE__, false);
        return;
    }
    bytesread = read(storagePointer, buffer, sizeof(struct Book));
    if (bytesread < sizeof(struct Book))
    {
        logErr("Too few bytes read. Reconfigure.", __LINE__, true);
        close(storagePointer);
        return;
    }
    books = (struct Book *)malloc(sizeof(struct Book));
    memcpy((struct Book *)&books[counter], buffer, sizeof(struct Book));
    while (read(storagePointer, buffer, sizeof(struct Book)) == sizeof(struct Book))
    {
        counter++;
        books = (struct Book *)realloc(books, sizeof(struct Book) * (counter + 1));
        memcpy((struct Book *)&books[counter], buffer, sizeof(struct Book));
    }
    numberOfBooks = counter + 1;
    close(storagePointer);
}