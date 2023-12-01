#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readstring.h"

/**
 * Lee una cadena de caracteres desde el teclado.
 * @param size Tamaño del buffer representado en cadena.
 * @param str Cadena buffer donde se almacenará la entrada.
 * @return tamaño de la cadena leída.
 */

int read_from_keyboard(int size, char *str)
{
  if (fgets(str, size, stdin) == NULL)
  {
    return -1;
  }

  size_t length = strlen(str);
  if (length > 0 && str[length - 1] == '\n')
  {
    str[length - 1] = '\0';
  }
  return length;
}

char **string_to_vector(char *str)
{
  int i;
  char *token;
  char *delim = " ";
  char **res;

  res = (char **)malloc(sizeof(char *));
  assert(res != NULL);
  i = 0;
  token = strtok(str, delim);
  while (token != NULL)
  {
    int cad_size;
    char **res_temp;
    cad_size = strlen(token) + 1;
    // Copiar el token en la posicion i del vector
    res[i] = strdup(token);
    // Buscamos la proxima cadena
    token = strtok(NULL, delim);
    i++;

    res_temp = realloc(res, (i + 1) * sizeof(*res));
    assert(res_temp != NULL);
    res = res_temp;
  }

  res[i] = NULL;
  return res;
}