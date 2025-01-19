#ifndef HTTP_H
#define HTTP_H

#define MAX_HEADER_SIZE 8192
#define MAX_METHOD_LENGTH 10
#define MAX_PATH_LENGTH 1024
#define MAX_HTTP_VERSION_LENGTH 10

// Structure to represent an HTTP request
typedef struct {
    char method[MAX_METHOD_LENGTH];
    char path[MAX_PATH_LENGTH];
    char http_version[MAX_HTTP_VERSION_LENGTH];
} HttpRequest;

// Parse an HTTP request string into an HttpRequest struct
int parse_http_request(const char *request_string, HttpRequest *request);

// Create an HTTP response string
void create_http_response(char *buffer, const char *status_code, const char *content_type, const char *body);



#endif
