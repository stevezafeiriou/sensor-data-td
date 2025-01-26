# Sensor Data WebSocket Server

This project implements a WebSocket server for handling real-time sensor data. It listens for incoming connections, receives sensor data in JSON format, and broadcasts it to all connected clients.

## Features

- **WebSocket Support**: Clients can connect to the server using WebSockets.
- **Real-Time Data Broadcasting**: Sensor data received from one client is broadcast to all connected clients.
- **Local IP Detection**: Displays the local IP addresses for easy connection.
- **Error Handling**: Handles invalid JSON messages gracefully.

---

## Prerequisites

Ensure the following software is installed on your system:

1. **Node.js** (v14 or higher)  
   You can download Node.js from [https://nodejs.org/](https://nodejs.org/).

2. **WebSocket Client** (optional for testing)  
   You can use tools like [websocat](https://github.com/vi/websocat), browser-based WebSocket clients, or custom scripts to connect to the server.

---

## Installation

1. Clone this repository:

   ```bash
   git clone <repository_url>
   cd <repository_folder>
   ```

2. Install the required dependencies:
   ```bash
   npm install
   ```

---

## Usage

### Start the Server

Run the server with the following command:

```bash
node server.js
```

### Server Output

When the server starts, it will display the local IP addresses and the listening port:

```
Server running at:
- http://<local_IP_1>:8080
- http://<local_IP_2>:8080
```

Replace `<local_IP_1>` and `<local_IP_2>` with the actual local IP addresses displayed in the output.

### Connecting a Client

Clients can connect to the WebSocket server using the following URL:

```
ws://<server_IP>:8080
```

### Sending Sensor Data

Clients can send JSON-formatted sensor data to the server. Example message format:

```json
{
	"temperature": 22.5,
	"humidity": 65
}
```

The server will broadcast this data to all connected clients.

---

## Code Overview

### Key Functions

1. **`getLocalIP()`**  
   Retrieves the local IPv4 addresses of the machine running the server.

2. **HTTP Server**  
   Serves a basic text response when accessed via a web browser or HTTP client:

   ```
   Sensor Data WebSocket Server
   ```

3. **WebSocket Server**

   - Listens for incoming WebSocket connections.
   - Processes incoming messages:
     - Parses and logs sensor data.
     - Broadcasts data to all connected clients.
   - Handles errors for invalid JSON messages.

4. **Error Logging**  
   Logs the raw message and error details if a message fails to parse.

---

## Development

### Dependencies

- **WebSocket**: Provides WebSocket server functionality.  
  Install with:
  ```bash
  npm install ws
  ```
- **HTTP**: Built-in Node.js module for handling HTTP requests.
- **OS**: Built-in Node.js module for accessing network interfaces.

### Extending Functionality

To extend the server, you can:

- Add custom validation for sensor data.
- Implement authentication for secure client connections.
- Log data to a database or file system.

---

## Troubleshooting

1. **Port Already in Use**  
   If port `8080` is occupied, modify the `server.listen` call in `server.js` to use a different port:

   ```javascript
   server.listen(9090, () => {
       ...
   });
   ```

2. **Invalid JSON Messages**  
   If a client sends malformed JSON, the server logs the raw message and an error:
   ```
   Error processing message: SyntaxError: Unexpected token ...
   Raw message that failed: <message>
   ```

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contributions

Contributions are welcome! Feel free to submit issues, feature requests, or pull requests.

---

## Contact

For questions or support, please contact [steve@saphirelabs.com](mailto:steve@saphirelabs.com).
