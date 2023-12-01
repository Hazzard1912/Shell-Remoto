#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "readstring.h"

#define MAX 100
#define MAX_OUTPUT 4096

int main(void)
{

  // Creamos el socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1)
  {
    perror("Error al crear el socket");
    exit(EXIT_FAILURE);
  }

  // Configuramos el socket
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(1234);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Conectamos al servidor
  int connect_result = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_result == -1)
  {
    perror("Error al conectar");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  printf("Conexión establecida...\n");

  printf("---------- Bienvenido a Minishell Remoto ----------\n");

  printf("Minishell inicializado. Ingrese un comando\n");
  // Recibimos el comando del usuario
  char command[MAX];

  while (1)
  {
    printf("> ");

    // Validar error al leer el comando
    if (read_from_keyboard(MAX, command) == -1)
    {
      printf("Error al leer el comando\n");
      exit(EXIT_FAILURE);
    }

    // Validar cadena vacia
    if (strlen(command) == 0)
    {
      printf("\?");
      continue;
    }

    // Validar comando de salida
    if (strcmp(command, "salida") == 0)
    {

      printf("Gracias por utilizar Minishell Remoto!\n");
      printf("Saliendo...\n");

      close(client_socket);
      break;
    }

    printf("Enviando comando: %s al servidor...\n", command);

    // creamos proceso hijo para enviar el comando al servidor
    pid_t pid = fork();
    assert(pid >= 0);

    if (pid == 0)
    {
      // Enviamos el comando al servidor
      if (send(client_socket, command, strlen(command), 0) == -1)
      {
        perror("Error al enviar el comando");
        close(client_socket);
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
    else
    {
      // Esperamos a que el hijo termine
      int status;
      int _pid;
      _pid = wait(&status);

      if (_pid != pid)
      {
        printf("Terminacion de proceso (%d) no esperado. Se esperaba el proceso %d\n", _pid, pid);
        continue;
      }

      if (WIFEXITED(status))
      {
        int _status = WEXITSTATUS(status);
        if (_status != 0)
        {
          printf("[ERROR] Proceso '%s' termino con status %d\n", command, _status);
        }
      }

      // Recibimos la salida del comando
      char command_output[MAX_OUTPUT];
      ssize_t bytes_read = recv(client_socket, command_output, MAX_OUTPUT - 1, 0);
      if (bytes_read == -1)
      {
        perror("Error al recibir la salida del comando");
        close(client_socket);
        exit(EXIT_FAILURE);
      }

      command_output[bytes_read] = '\0';

      printf("Respuesta del servidor:\n%s\n", command_output);
    }
  }

  close(client_socket);

  return 0;
}