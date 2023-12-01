#ifndef __READSTRING_H__
#define __READSTRING_H__

#include <stdio.h>
#include <string.h>

#define MAX_TOKENS 20
#define TOKEN_LEN 80

int read_from_keyboard(int, char *);

char **string_to_vector(char *);

#endif