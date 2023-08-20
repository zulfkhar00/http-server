#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *handle_connection(void *);             // method for reacting to incoming requests
struct sockaddr_in server_addr, client_addr; // structure to save IP address and port
int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    socklen_t sin_len = sizeof(client_addr);
    pthread_t tid;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Bind socket to port
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // Initialize/fill the server_address to 0
    server_address.sin_family = AF_INET;                // address family
    server_address.sin_port = htons(80);                // port
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed..:");
        return -1;
    }

    // Listen for connections
    if (listen(server_fd, 10) == -1)
    {
        perror("Error listening for connections");
        close(server_fd);
        exit(1);
    }

    printf("Server started on port 80...\n");

    while (1)
    {
        // Accept new connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_len);
        if (client_fd == -1)
        {
            perror("Error accepting connection");
            continue;
        }

        // Create new thread to handle request
        if (pthread_create(&tid, NULL, handle_connection, (void *)&client_fd) != 0)
        {
            perror("Error creating thread");
            close(client_fd);
            continue;
        }

        // Detach thread so it can run independently
        if (pthread_detach(tid) != 0)
        {
            perror("Error detaching thread");
            close(client_fd);
            continue;
        }
    }

    // Close server socket
    close(server_fd);

    return 0;
}

void *handle_connection(void *arg)
{
    int client_socket = *((int *)arg);
    // free(arg);

    char buffer[1024]; // for input packet reading
    memset(buffer, 0, 1024);
    ssize_t recv_size = recv(client_socket, buffer, 1024, 0);
    if (recv_size <= 0)
    {
        fprintf(stderr, "Failed to receive HTTP request from client\n");
        close(client_socket);
        return NULL;
    }

    char method[10], path[1024], protocol[10];
    sscanf(buffer, "%s %s %s", method, path, protocol); // parsing the method, path, protocol from input

    char filename[100];
    char *extension;
    sscanf(path, "/%s", filename);      // parsing filename
    extension = strrchr(filename, '.'); // find the last occurrence of "."
    if (extension != NULL)
    {
        extension++; // move the pointer one position to the right to exclude the "."
    }
    else
    {
        extension = "html"; // if it is root path '/'
    }

    if (strcmp(method, "GET") != 0) // if it is any other methods (e.g. POST)
    {
        printf("%s \"%s %s %s\" [501 Not Implemented]\n", inet_ntoa(client_addr.sin_addr), method, path, protocol);
        char *error_message = "HTTP/1.0 501 Not Implemented\r\n\r\n";
        send(client_socket, error_message, strlen(error_message), 0);
        close(client_socket);
        printf("exiting with error\n");
        return NULL;
    }

    // determine content type
    char *content_type;
    if (strcmp(path, "/") == 0 || strcmp(extension, "html") == 0)
    {
        content_type = "text/html";
    }
    else if (strcmp(extension, "css") == 0)
    {
        content_type = "text/css";
    }
    else if (strcmp(extension, "js") == 0)
    {
        content_type = "application/javascript";
    }
    else if (strcmp(extension, "png") == 0)
    {
        content_type = "image/png";
    }
    else
    {
        printf("%s \"%s %s %s\" [404 Not Found]\n", inet_ntoa(client_addr.sin_addr), method, path, protocol);
        char *error_message = "HTTP/1.0 404 Not Found\r\n\r\n";
        send(client_socket, error_message, strlen(error_message), 0);
        close(client_socket);
        return NULL;
    }

    // realising the file path in server
    char file_path[100];
    if (strcmp(filename, "") == 0)
    {
        sprintf(file_path, "files/index.html");
    }
    else
    {
        sprintf(file_path, "files/%s", filename);
    }

    FILE *file = fopen(file_path, "r");
    if (file == NULL) // if file doesn't exist
    {
        printf("%s \"%s %s %s\" [404 Not Found]\n", inet_ntoa(client_addr.sin_addr), method, path, protocol);
        char *error_message = "HTTP/1.0 404 Not Found\r\n\r\n";
        send(client_socket, error_message, strlen(error_message), 0);
        close(client_socket);
        return NULL;
    }

    // send response
    char header[1024];
    sprintf(header, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
    send(client_socket, header, strlen(header), 0);

    // send file
    int file_size;
    while ((file_size = fread(buffer, 1, 1024, file)) > 0)
    {
        send(client_socket, buffer, file_size, 0);
    }
    printf("%s \"%s %s %s\" [200 OK]\n", inet_ntoa(client_addr.sin_addr), method, path, protocol);

    fclose(file);
    close(client_socket);
    return NULL;
}