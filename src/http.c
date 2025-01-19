#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parse an HTTP request string into an HttpRequest struct
int parse_http_request(const char *request_string, HttpRequest *request) {
    if (!request_string || !request) {
        return -1; // Invalid input
    }

    // Split the request string by spaces to extract method, path, and version
    char method[MAX_METHOD_LENGTH], path[MAX_PATH_LENGTH], http_version[MAX_HTTP_VERSION_LENGTH];
    if (sscanf(request_string, "%s %s %s", method, path, http_version) != 3) {
        return -1; // Failed to parse
    }

    // Copy parsed values into the HttpRequest struct
    strncpy(request->method, method, MAX_METHOD_LENGTH - 1);
    request->method[MAX_METHOD_LENGTH - 1] = '\0';
    strncpy(request->path, path, MAX_PATH_LENGTH - 1);
    request->path[MAX_PATH_LENGTH - 1] = '\0';
    strncpy(request->http_version, http_version, MAX_HTTP_VERSION_LENGTH - 1);
    request->http_version[MAX_HTTP_VERSION_LENGTH - 1] = '\0';

    return 0; // Success
}

// Create an HTTP response string
void create_http_response(char *response, const char *status, const char *content_type, const char *body)
{
    snprintf(response, 1024,
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",
             status, content_type, strlen(body), body);
}

