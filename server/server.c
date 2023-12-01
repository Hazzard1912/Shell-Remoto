#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "readstring.h"

#define MAX 100

int main()
{

    // Creamos el socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configuramos el socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1234);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Asociamos el socket a un puerto
    int bind_result =
        bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_result == -1)
    {
        perror("Error en la vinculación");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Ponemos el socket en modo escucha
    if (listen(server_socket, 5) == -1)
    {
        perror("Error en la escucha");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Esperando conexiones...\n");

    // Aceptamos conexiones
    int client_socket = accept(server_socket, NULL, NULL);

    if (client_socket == -1)
    {
        perror("Error al aceptar la conexión");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Conexión establecida\n");

    // bucle principal
    while (1)
    {

        char buffer[MAX];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == -1)
        {
            perror("Error al recibir datos");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';

        printf("Comando recibido: %s\n", buffer);

        // Creamos el vector de cadenas a partir del comando recibido
        char **command_vector = string_to_vector(buffer);

        // Usamos un pipe para redirigir la salida del comando
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }

        // Creamos el proceso hijo que ejecutará el comando
        pid_t pid = fork();
        assert(pid >= 0);

        if (pid == 0)
        {
            // Estamos en el proceso hijo, redirigimos la salida del comando al pipe
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            int status = execvp(command_vector[0], command_vector);

            if (status == -1)
            {
                perror("Error al ejecutar el comando");
                exit(EXIT_FAILURE);
            }

            exit(status);
        }
        else
        {
            // Estamos en el proceso padre, esperamos a que el hijo termine
            int status;
            int _pid;
            _pid = wait(&status);

            // Leemos el pipe
            close(pipefd[1]);
            char command_output[4096];
            ssize_t bytes_read =
                read(pipefd[0], command_output, sizeof(command_output) - 1);

            if (bytes_read == -1)
            {
                perror("Error al leer el pipe");
                close(client_socket);
                close(server_socket);
                exit(EXIT_FAILURE);
            }

            command_output[bytes_read] = '\0';

            printf("Salida del comando:\n%s\n", command_output);

            // Enviamos la salida del comando al cliente
            if (send(client_socket, command_output, strlen(command_output), 0) ==
                -1)
            {
                perror("Error al enviar la salida del comando");
                close(client_socket);
                close(server_socket);
                exit(EXIT_FAILURE);
            }

            close(pipefd[0]);

            if (_pid != pid)
            {
                printf("Terminacion de proceso (%d) no esperado. Se esperaba el "
                       "proceso %d\n",
                       _pid, pid);
                continue;
            }

            if (WIFEXITED(status))
            {
                int _status = WEXITSTATUS(status);
                if (_status != 0)
                {
                    printf("[ERROR] Proceso '%s' termino con status %d\n",
                           command_vector[0], _status);
                }
            }
        }
    }

    // Cerramos los sockets
    close(client_socket);
    close(server_socket);

    return 0;
}