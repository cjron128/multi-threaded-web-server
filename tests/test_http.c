#include <stdio.h>
#include "../src/http.h"

int main() {
    // Test parse_http_request
    HttpRequest request;
    const char *raw_request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (parse_http_request(raw_request, &request) == 0) {
        printf("Parsed Request:\n");
        printf("Method: %s\n", request.method);
        printf("Path: %s\n", request.path);
        printf("HTTP Version: %s\n", request.http_version);
    } else {
        printf("Failed to parse request\n");
    }

    // Test create_http_response
    char response[MAX_HEADER_SIZE];
    create_http_response(response, "200 OK", "text/plain", "Hello, World!");
    printf("\nGenerated Response:\n%s\n", response);

    return 0;
}
