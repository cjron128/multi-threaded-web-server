CC = gcc
CFLAGS = -Wall -pthread
SRC = src/main.c src/server.c src/http.c
OBJ = $(SRC:.c=.o)
EXEC = web_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test_http: tests/test_http.c src/http.c
	$(CC) $(CFLAGS) -o test_http tests/test_http.c src/http.c

clean:
	rm -f $(OBJ) $(EXEC) test_http
