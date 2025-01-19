#include "server.h"
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGES 10

// Global client queue
int client_sockets[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global message queue
typedef struct
{
    int sender_socket;
    char content[256];
} Message;

Message message_queue[MAX_MESSAGES];
int message_count = 0;
pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_cond = PTHREAD_COND_INITIALIZER;

void add_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == 0)
        {
            client_sockets[i] = client_socket;
            printf("Client with socket descriptor %d connected at index %d\n", client_socket, i);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == client_socket)
        {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void add_message(const char *message)
{
    pthread_mutex_lock(&message_mutex);
    if (message_count < MAX_MESSAGES)
    {
        strncpy(message_queue[message_count].content, message, 256);
        message_count++;
        printf("Message added to queue: %s\n", message);
        pthread_cond_signal(&message_cond);
    }
    else
    {
        printf("Message queue is full. Dropping message: %s\n", message);
    }
    pthread_mutex_unlock(&message_mutex);
}

void get_message(char *buffer)
{
    pthread_mutex_lock(&message_mutex);
    while (message_count == 0)
    {
        pthread_cond_wait(&message_cond, &message_mutex);
    }
    strncpy(buffer, message_queue[0].content, 256);
    for (int i = 1; i < message_count; i++)
    {
        strncpy(message_queue[i - 1].content, message_queue[i].content, 256);
    }
    message_count--;
    printf("Message retrieved: %s\n", buffer);
    pthread_mutex_unlock(&message_mutex);
}

// Map file extensions to MIME types
const char *get_mime_type(const char *path)
{
    if (strstr(path, ".html"))
        return "text/html";
    if (strstr(path, ".css"))
        return "text/css";
    if (strstr(path, ".js"))
        return "application/javascript";
    if (strstr(path, ".png"))
        return "image/png";
    if (strstr(path, ".jpg") || strstr(path, ".jpeg"))
        return "image/jpeg";
    return "application/octet-stream";
}

// Serve static files
void serve_static_file(int client_socket, const char *path)
{
    char file_path[1024] = "frontend";

    if (strcmp(path, "/") == 0)
    {
        strcat(file_path, "/index.html");
    }
    else
    {
        strcat(file_path, path);
    }

    int fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        char response[1024];
        create_http_response(response, "404 Not Found", "text/plain", "404 File Not Found");
        send(client_socket, response, strlen(response), 0);
        return;
    }

    struct stat file_stat;
    fstat(fd, &file_stat);
    size_t file_size = file_stat.st_size;

    char *file_content = malloc(file_size);
    read(fd, file_content, file_size);

    const char *mime_type = get_mime_type(file_path);

    char header[1024];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n\r\n",
             mime_type, file_size);

    send(client_socket, header, strlen(header), 0);
    send(client_socket, file_content, file_size, 0);

    close(fd);
    free(file_content);
}

// Handle client requests
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[1024];

    while (1)
    {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                printf("Client disconnected.\n");
            }
            else
            {
                perror("recv failed");
            }
            break;
        }

        buffer[bytes_received] = '\0';

        HttpRequest request;
        if (parse_http_request(buffer, &request) == 0)
        {
            printf("Client %d requested path: %s\n", client_socket, request.path);

            if (strcmp(request.method, "POST") == 0 && strcmp(request.path, "/messages") == 0)
            {
                char *body = strstr(buffer, "\r\n\r\n");
                if (body)
                {
                    body += 4;
                    printf("Received message: %s\n", body);
                    pthread_mutex_lock(&message_mutex);
                    message_queue[message_count].sender_socket = client_socket;
                    pthread_mutex_unlock(&message_mutex);
                    add_message(body);

                    char response[1024];
                    create_http_response(response, "200 OK", "text/plain", "Message received");
                    send(client_socket, response, strlen(response), 0);
                }
            }
            else if (strcmp(request.method, "GET") == 0 && strcmp(request.path, "/messages") == 0)
            {
                char message[256];
                get_message(message);

                char response[1024];
                create_http_response(response, "200 OK", "text/plain", message);
                send(client_socket, response, strlen(response), 0);
            }
            else
            {
                serve_static_file(client_socket, request.path);
            }
        }
    }

    remove_client(client_socket);
    close(client_socket);
    return NULL;
}

// Start the server
void start_server(int port)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ||
        listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Server setup failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d\n", port);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0)
        {
            perror("Client accept failed");
            continue;
        }

        int *client_sock_ptr = malloc(sizeof(int));
        *client_sock_ptr = client_socket;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client_sock_ptr);
        pthread_detach(thread_id);
    }

    close(server_socket);
}
