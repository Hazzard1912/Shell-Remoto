# Imagen base de C
FROM gcc:latest

# Directorio de trabajo
WORKDIR /server

# Copia el archivo server.c al directorio de trabajo
COPY server.c readstring.c readstring.h /server/

# Compila el archivo server.c
RUN gcc -c readstring.c
RUN gcc -o server server.c readstring.o

# Expone el puerto 12345
EXPOSE 12345

# Ejecuta el servidor
CMD ["./server"]
