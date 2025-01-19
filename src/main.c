#include "server.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int port = 8080; // Default port
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    start_server(port);
    return 0;
}
