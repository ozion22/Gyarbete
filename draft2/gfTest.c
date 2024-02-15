#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char* content;

void getFile(char *filename)    //Puts file on either the binary or char buffer based on suffixes
                                //TODO Learn regex and pattern-match based on suffix
{
        FILE *docPointer;
        char *buffer;
        char *extention = strrchr(filename, '.');
        int readcounter=0;
        buffer = malloc(sizeof(char));
        docPointer = fopen(filename, "r");
        if(docPointer == NULL)
        {
                //throwErr("404 File not Found!\n", __LINE__);
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

int main()
{
	getFile("push.jpg.jpg");
	printf("%s", content);
}
