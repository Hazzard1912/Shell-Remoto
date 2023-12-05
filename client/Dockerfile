# Imagen base de C
FROM gcc:latest

# Directorio de trabajo
WORKDIR /client

# Copia el archivo client.c al directorio de trabajo
COPY client.c readstring.c readstring.h /client/

# Compila el archivo client.c
RUN gcc -c readstring.c
RUN gcc -o client client.c readstring.o
