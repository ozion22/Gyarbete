/*
@file This is the standard header :p
@breif Included libraries are: stdio, stdlib, stdbool & string
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Simple ANSI-macros to make colored output easier
#define ANSI_BLACK "\e[0;30m"
#define ANSI_RED "\e[0;31m"
#define ANSI_GREEN "\e[0;32m"
#define ANSI_YELLOW "\e[0;33m"
#define ANSI_BLUE "\e[0;34m"
#define ANSI_MAGENTA "\e[0;35m"
#define ANSI_CYAN "\e[0;36m"
#define ANSI_WHITE "\e[0;37m"
#define ANSI_BOLD "\e[1m"
#define ANSI_RESET "\e[0m"

// Regular background
#define ANSI_BLACKB "\e[40m"
#define ANSI_REDB "\e[41m"
#define ANSI_GREENB "\e[42m"
#define ANSI_YELLOWB "\e[43m"
#define ANSI_BLUEB "\e[44m"
#define ANSI_MAGENTAB "\e[45m"
#define ANSI_CYANB "\e[46m"
#define ANSI_WHITEB "\e[47m"

// Box_drawing
#define BOX_TOPLEFT "╭"
#define BOX_TOPRIGHT "╮"
#define BOX_BOTTOMLEFT "╯"
#define BOX_BOTTOMRIGHT "╰"

/*
U+250x  ─       ━       │       ┃       ┄       ┅       ┆       ┇       ┈       ┉       ┊       ┋       ┌       ┍       ┎       ┏
U+251x  ┐       ┑       ┒       ┓       └       ┕       ┖       ┗       ┘       ┙       ┚       ┛       ├       ┝       ┞       ┟
U+252x  ┠       ┡       ┢       ┣       ┤       ┥       ┦       ┧       ┨       ┩       ┪       ┫       ┬       ┭       ┮       ┯
U+253x  ┰       ┱       ┲       ┳       ┴       ┵       ┶       ┷       ┸       ┹       ┺       ┻       ┼       ┽       ┾       ┿
U+254x  ╀       ╁       ╂       ╃       ╄       ╅       ╆       ╇       ╈       ╉       ╊       ╋       ╌       ╍       ╎       ╏
U+255x  ═       ║       ╒       ╓       ╔       ╕       ╖       ╗       ╘       ╙       ╚       ╛       ╜       ╝       ╞       ╟
U+256x  ╠       ╡       ╢       ╣       ╤       ╥       ╦       ╧       ╨       ╩       ╪       ╫       ╬       ╭       ╮       ╯
U+257x  ╰       ╱       ╲       ╳       ╴       ╵       ╶       ╷       ╸       ╹       ╺       ╻       ╼       ╽       ╾       ╿
Box-drawing Characters :D
*/

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
    fprintf(stderr, ANSI_YELLOW "%s\n", message);
    fprintf(stderr, ANSI_RESET);
}

/// @brief Throws fatal error, exits program
/// @param message The message to display
/// @param line use __LINE__
void throwErr(const char message[], int line)
{
    fprintf(stderr, ANSI_RED "Fatal error\n");
    logErr(message, line, false);
    exit(1);
}

/// @brief Prints Errors to stderr
/// @param message The message
/// @param line Use __LINE__
/// @param supressLog Supress logging
void printErr(const char message[], int line, bool supressLog)
{
    fprintf(stderr, ANSI_RED "%s\n" ANSI_RESET, message);
    if (!supressLog)
    {
        logErr(message, line, true);
    }
}