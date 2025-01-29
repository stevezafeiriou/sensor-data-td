import { useState, useEffect } from "react";
import styled from "styled-components";
import Scene from "./components/Scene";
import "./App.css";

const Container = styled.div`
	width: 100vw;
	height: 100vh;
	background: #1a1a1a;
	color: #fff;
	overflow: hidden;
`;

const Card = styled.div`
	width: 300px;
	height: 200px;
	display: flex;
	flex-direction: column;
	justify-content: space-around;
	position: absolute;
	top: 20px;
	left: 20px;
	padding: 10px;
	background-color: #ff56b1;
	border-radius: 16px;
	z-index: 10000;
	h1 {
		font-size: 20px;
		font-weight: 600;
	}

	p,
	a {
		font-size: 16px;
		color: #fff;
	}
`;

function App() {
	const [sensorData, setSensorData] = useState({ x: 0, y: 0, z: 0 });

	useEffect(() => {
		const ws = new WebSocket("http://192.168.237.252:8080"); // Replace with your IP server address

		ws.onmessage = (event) => {
			try {
				const data = JSON.parse(event.data);
				setSensorData({
					x: (data.x * Math.PI) / 180, // Convert degrees to radians
					y: (data.y * Math.PI) / 180,
					z: (data.z * Math.PI) / 180,
				});
			} catch (error) {
				console.error("Error parsing sensor data:", error);
			}
		};

		return () => ws.close();
	}, []);

	return (
		<Container>
			<Card>
				<h1>Sensor Data from ESP32 to React App.</h1>
				<p>X Axis: {sensorData.x}</p>
				<p>Y Axis: {sensorData.y}</p>
				<p>Z Axis: {sensorData.z}</p>
				<p>
					Developed by{" "}
					<a href="https://stevezafeiriou.com" rel="noopener">
						Steve Zafeiriou
					</a>
					.
				</p>
			</Card>
			<Scene rotation={sensorData} />
		</Container>
	);
}

export default App;
