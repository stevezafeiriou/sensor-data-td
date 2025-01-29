# Sensor Data Visualization with React and Three.js

This project is a 3D visualization of real-time sensor data from an ESP32 device, developed using **React** and **Three.js**. The application features a rotating 3D model with a shiny chrome material, controlled dynamically by sensor data streamed via WebSocket.

## Features

- Real-time sensor data visualization.
- Rotating 3D model rendered with `@react-three/fiber` and `@react-three/drei`.
- Shiny chrome material effect on the 3D model.
- Responsive UI with styled-components.
- Easily configurable WebSocket connection for sensor data.

## Getting Started

Follow these steps to set up and run the application locally.

### Prerequisites

Make sure you have the following installed:

- Node.js (>=16.x)
- npm (>=8.x)

### Installation

1. Clone this repository:

   ```bash
   git clone https://github.com/your-username/your-repo.git
   cd your-repo
   ```

2. Install the required dependencies:

   ```bash
   npm install
   ```

### Configuration

1. Update the WebSocket URL in `App.js` to match your ESP32 server's IP address and port:

   ```javascript
   const ws = new WebSocket("http://<your-nodejs-server-ip>:8080");
   ```

2. Ensure your ESP32 device is configured to send sensor data in JSON format, with `x`, `y`, and `z` fields representing rotation angles in degrees.

### Usage

Run the app in development mode:

```bash
npm start
```

Open [http://localhost:3000](http://localhost:3000) to view it in your browser. The app will automatically reload if you make changes to the code.

### Production Build

To create a production-ready build:

```bash
npm run build
```

The build files will be generated in the `build` directory. You can deploy these files to any static hosting service.

### File Structure

```
src/
├── App.js                # Main application file
├── components/
│   ├── Scene.js          # 3D visualization component
│   ├── SceneElements.js  # Styled-components for the scene container
├── index.js              # Entry point of the application
└── App.css               # Global styles
```

## How It Works

### Real-Time Data

- The app connects to an ESP32 WebSocket server to receive real-time sensor data.
- Sensor data is parsed and used to update the rotation of the 3D model.

### 3D Visualization

- The 3D model is rendered using `@react-three/fiber` and `@react-three/drei`.
- A shiny chrome effect is applied using a `MeshStandardMaterial` with high metalness and low roughness.

### UI

- The application includes a simple card UI displaying sensor data, styled with `styled-components`.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to the creators of [React](https://reactjs.org/) and [Three.js](https://threejs.org/) for providing amazing tools for developers.
- Special thanks to the developers of `@react-three/fiber` and `@react-three/drei` for making 3D rendering in React simple and intuitive.
