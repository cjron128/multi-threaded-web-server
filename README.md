# Multi-Threaded Web Server with Long Polling Chat

## Description
This project is a multi-threaded web server written in C. 

It supports:
- Hosting static files (HTML, CSS, JavaScript).
- A real-time chat system using long polling.
- Thread-safe message handling with mutexes and condition variables.

The frontend provides a simple chat interface that interacts with the server via HTTP.

## Features

### Static File Hosting: 

Serve files like index.html, style.css, and script.js from the frontend directory.

### Long-Polling Chat: 

Clients can send and receive messages in real time using HTTP requests.

### Thread-Safe Message Queue: 

Messages are managed using a mutex-protected queue to handle multiple clients concurrently.


## Endpoints

### Static File Hosting

URL: /
Description: Serves the index.html file and other static assets.
Example:
curl http://localhost:8080/

### Chat System

POST /messages

Description: Sends a message to the chat.

Example: curl -X POST -d "Hello, World!" http://localhost:8080/messages

GET /messages

Description: Retrieves the next message from the chat. Blocks until a message is available.

Example: curl -X GET http://localhost:8080/messages

## Build and Run

### Build the Server

Ensure you have gcc installed. Then run: make

### Run the Server

Start the server on a specific port (e.g., 8080): ./web_server 8080
Open the Frontend
Navigate to http://localhost:8080/ in your web browser.


## How It Works

### Frontend Interaction

The frontend uses JavaScript to:

Send messages to the server via POST /messages.
Retrieve new messages via long-polling with GET /messages.

### Server Logic

A new thread is spawned for each client connection.
Messages are stored in a thread-safe queue and distributed to connected clients.

### Multi-Threading

The server uses pthread for concurrency.
Mutexes and condition variables ensure thread-safe message handling.
Future Improvements
Add WebSocket support for more efficient real-time communication.
Implement user authentication.
Add message persistence using a database.
