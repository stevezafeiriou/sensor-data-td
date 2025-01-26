const WebSocket = require("ws");
const http = require("http");
const os = require("os");

function getLocalIP() {
	const interfaces = os.networkInterfaces();
	const addresses = [];

	Object.keys(interfaces).forEach((interfaceName) => {
		interfaces[interfaceName].forEach((interface) => {
			if (interface.family === "IPv4" && !interface.internal) {
				addresses.push(interface.address);
			}
		});
	});

	return addresses.length > 0 ? addresses : ["127.0.0.1"];
}

const server = http.createServer((req, res) => {
	res.writeHead(200, { "Content-Type": "text/plain" });
	res.end("Sensor Data WebSocket Server\n");
});

const wss = new WebSocket.Server({ server });

server.listen(8080, () => {
	const ips = getLocalIP();
	console.log("Server running at:");
	ips.forEach((ip) => console.log(`- http://${ip}:8080`));
});

wss.on("connection", (ws) => {
	console.log("New client connected");

	ws.on("message", (message) => {
		try {
			const sensorData = JSON.parse(message);
			console.log("Sensor Data:", sensorData); // Add this line

			wss.clients.forEach((client) => {
				if (client.readyState === WebSocket.OPEN) {
					client.send(JSON.stringify(sensorData));
				}
			});
		} catch (error) {
			console.error("Error processing message:", error);
			console.log("Raw message that failed:", message); // Add this line
		}
	});

	ws.on("close", () => {
		console.log("Client disconnected");
	});
});
