import { Canvas } from "@react-three/fiber";
import { OrbitControls, useGLTF } from "@react-three/drei";
import { SceneContainer } from "./SceneElements";
import { DRACOLoader } from "three-stdlib";
import { Suspense } from "react";
import { useMemo } from "react";
import { MeshStandardMaterial } from "three";

const Model = ({ rotation }) => {
	const { scene } = useGLTF("/model.glb", undefined, (loader) => {
		const dracoLoader = new DRACOLoader();
		dracoLoader.setDecoderPath(
			"https://www.gstatic.com/draco/versioned/decoders/1.5.6/"
		);
		loader.setDRACOLoader(dracoLoader);
	});

	// Create the shiny chrome material
	const chromeMaterial = useMemo(
		() =>
			new MeshStandardMaterial({
				color: 0xffffff, // Base color (white)
				metalness: 1, // Fully metallic
				roughness: 0.45, // Perfectly smooth surface
			}),
		[]
	);

	// Apply material to all child meshes
	scene.traverse((child) => {
		if (child.isMesh) {
			child.material = chromeMaterial;
		}
	});

	return (
		<primitive
			object={scene}
			rotation={[rotation.x, rotation.y, rotation.z]}
			scale={[0.05, 0.05, 0.05]}
			position={[-0.1, -0.5, 0]}
		/>
	);
};

const Scene = ({ rotation }) => {
	return (
		<SceneContainer>
			<Canvas camera={{ position: [2, 2, 5] }}>
				<ambientLight intensity={1} />
				<directionalLight position={[5, 5, 5]} intensity={3} />
				<pointLight position={[10, 10, 10]} intensity={1} />

				{/* import model to the scene */}
				<Suspense fallback={null}>
					<Model rotation={rotation} />
				</Suspense>
				<OrbitControls enableZoom={true} />
			</Canvas>
		</SceneContainer>
	);
};

export default Scene;
