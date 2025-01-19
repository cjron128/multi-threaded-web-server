const messageInput = document.getElementById('messageInput');
const sendButton = document.getElementById('sendButton');
const messagesDiv = document.getElementById('messages');

// Send a message to the server
sendButton.addEventListener('click', () => {
    const message = messageInput.value.trim();
    if (message) {
        fetch('http://localhost:8080/messages', {
            method: 'POST',
            headers: {
                'Content-Type': 'text/plain',
            },
            body: message,
        }).then((response) => {
            if (response.ok) {
                messageInput.value = '';
            }
        });
    }
});

// Long polling to fetch new messages
function fetchMessages() {
    fetch('http://localhost:8080/messages', {
        method: 'GET',
    })
        .then((response) => response.text())
        .then((data) => {
            const messageElement = document.createElement('p');
            messageElement.textContent = data;
            messagesDiv.appendChild(messageElement);
            messagesDiv.scrollTop = messagesDiv.scrollHeight; // Scroll to bottom
            fetchMessages(); // Continue polling
        })
        .catch((error) => {
            console.error('Error fetching messages:', error);
            setTimeout(fetchMessages, 3000); // Retry after 3 seconds
        });
}

// Start fetching messages
fetchMessages();
