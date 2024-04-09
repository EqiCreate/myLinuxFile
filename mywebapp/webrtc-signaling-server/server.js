const WebSocket = require('ws');

// Create a WebSocket server on port 3001 (or any port you prefer)
const wss = new WebSocket.Server({ port: 3001 });

console.log('WebSocket server started on ws://localhost:3001');


wss.on('connection', function connection(ws) {
  console.log('A client connected');

  // Listen for messages from clients
  ws.on('message', function incoming(data) {
    console.log('received: %s', data);
    const message = JSON.parse(data); // Parse to add or modify the message, if necessary
    message.receivedAt = new Date().toISOString(); // Example modification
    const newMessage = JSON.stringify(message); // Serialize back to JSON string
    // Broadcast incoming message to all clients except the sender
    wss.clients.forEach(function each(client) {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(newMessage);  
        console.log('send: %s', newMessage);

      }
    });
  });

  ws.on('close', function close() {
    console.log('A client disconnected');
  });
});
