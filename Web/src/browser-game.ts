import {
  ArcRotateCamera, Color3, Color4, DirectionalLight, DynamicTexture, Engine, HemisphericLight,
  KeyboardEventTypes, Mesh, MeshBuilder, PBRMaterial, Scene, ShadowGenerator, StandardMaterial,
  Vector3, WebGPUEngine
} from '@babylonjs/core';
import '@babylonjs/loaders';
import RAPIER from '@dimforge/rapier3d-compat';

export type BrowserGame = { engine: Engine | WebGPUEngine; scene: Scene; dispose: () => void };

const mat = (scene: Scene, name: string, colour: Color3, metallic = 0, roughness = 0.8) => {
  const material = new PBRMaterial(name, scene); material.albedoColor = colour; material.metallic = metallic; material.roughness = roughness; return material;
};
const label = (scene: Scene, text: string, position: Vector3, colour = '#e8d2a4') => {
  const texture = new DynamicTexture(`label-${text}`, { width: 1024, height: 256 }, scene, true);
  texture.hasAlpha = true; texture.drawText(text, null, 150, 'bold 96px Arial', colour, 'transparent', true, true);
  const plane = MeshBuilder.CreatePlane(`sign-${text}`, { width: 4, height: 1 }, scene); plane.position = position;
  const material = new StandardMaterial(`signmat-${text}`, scene); material.diffuseTexture = texture; material.emissiveColor = Color3.FromHexString('#6f563a'); material.opacityTexture = texture; plane.material = material; return plane;
};
const box = (scene: Scene, name: string, position: Vector3, dimensions: { width: number; depth: number; height: number }, material: PBRMaterial) => {
  const mesh = MeshBuilder.CreateBox(name, dimensions, scene); mesh.position = position; mesh.material = material; return mesh;
};

export async function createBrowserGame(canvas: HTMLCanvasElement): Promise<BrowserGame> {
  await RAPIER.init();
  let engine: Engine | WebGPUEngine;
  if (await WebGPUEngine.IsSupportedAsync) {
    try { engine = new WebGPUEngine(canvas, { adaptToDeviceRatio: true, antialias: true }); await (engine as WebGPUEngine).initAsync(); }
    catch { engine = new Engine(canvas, true, { preserveDrawingBuffer: false, stencil: true }, true); }
  } else engine = new Engine(canvas, true, { preserveDrawingBuffer: false, stencil: true }, true);
  const scene = new Scene(engine); scene.clearColor = new Color4(0.03, 0.07, 0.08, 1);
  const camera = new ArcRotateCamera('follow-camera', -Math.PI / 2, Math.PI / 3.1, 16, new Vector3(0, 1.5, 0), scene);
  camera.lowerRadiusLimit = 7; camera.upperRadiusLimit = 24; camera.wheelPrecision = 80; camera.attachControl(canvas, true); camera.inertia = 0.75; camera.panningSensibility = 0;
  const hemi = new HemisphericLight('sky-fill', new Vector3(0, 1, 0), scene); hemi.intensity = 0.6; hemi.diffuse = Color3.FromHexString('#d9e5eb'); hemi.groundColor = Color3.FromHexString('#3e5a51');
  const sun = new DirectionalLight('sun', new Vector3(-0.4, -1, 0.3), scene); sun.position = new Vector3(30, 50, -20); sun.intensity = 2.2;
  const shadows = new ShadowGenerator(2048, sun); shadows.useBlurExponentialShadowMap = true; shadows.blurKernel = 24;
  const road = mat(scene, 'warm asphalt', Color3.FromHexString('#1c2a2c'), 0, 0.92); const sidewalk = mat(scene, 'sidewalk stone', Color3.FromHexString('#aaa99a'), 0, 0.78); const cream = mat(scene, 'hospital facade', Color3.FromHexString('#d7d0be'), 0, 0.7); const terracotta = mat(scene, 'restaurant facade', Color3.FromHexString('#a9503c'), 0, 0.68); const glass = mat(scene, 'reflective glass', Color3.FromHexString('#3d7780'), 0.55, 0.16); const white = mat(scene, 'road markings', Color3.FromHexString('#e3d9bc'), 0, 0.5);
  box(scene, 'road', new Vector3(0, -0.12, 0), { width: 110, depth: 12, height: 0.2 }, road); box(scene, 'cross-street', new Vector3(0, -0.1, 0), { width: 12, depth: 82, height: 0.2 }, road);
  for (const z of [-10, 10]) box(scene, `sidewalk-${z}`, new Vector3(0, 0, z), { width: 110, depth: 5, height: 0.4 }, sidewalk);
  for (const x of [-58, 58]) box(scene, `sidewalk-x-${x}`, new Vector3(x, 0, 0), { width: 5, depth: 82, height: 0.4 }, sidewalk);
  for (let x = -45; x <= 45; x += 9) box(scene, `mark-${x}`, new Vector3(x, 0.02, 0), { width: 4, depth: 0.18, height: 0.03 }, white);
  const buildings = [{ name: 'HOSPITAL', pos: new Vector3(-30, 7, -23), size: [25, 28, 14] as const, material: cream }, { name: 'RESTAURANT', pos: new Vector3(30, 6, -23), size: [25, 28, 12] as const, material: terracotta }, { name: 'BANK', pos: new Vector3(-30, 7, 23), size: [25, 28, 14] as const, material: glass }, { name: 'SUPERMARKET', pos: new Vector3(30, 6, 23), size: [25, 28, 12] as const, material: cream }];
  for (const building of buildings) { const b = box(scene, building.name, building.pos, { width: building.size[0], depth: building.size[1], height: building.size[2] }, building.material); b.receiveShadows = true; label(scene, building.name, building.pos.add(new Vector3(0, building.size[2] / 2 + 0.4, building.size[1] / 2 + 0.1))); const roof = box(scene, `${building.name}-roof`, building.pos.add(new Vector3(0, building.size[2] / 2 + 0.15, 0)), { width: building.size[0] + 0.4, depth: building.size[1] + 0.4, height: 0.3 }, glass); roof.receiveShadows = true; }
  const hospitalDoor = box(scene, 'hospital-entry', new Vector3(-30, 1.3, -9), { width: 4, depth: 0.25, height: 2.6 }, glass); label(scene, 'ENTER', new Vector3(-30, 3.1, -9.2), '#ffffff');
  const player = MeshBuilder.CreateCapsule('player', { height: 2.2, radius: 0.42 }, scene); player.position = new Vector3(0, 1.1, 4); player.material = mat(scene, 'player jacket', Color3.FromHexString('#e3a95e'), 0, 0.75); shadows.addShadowCaster(player); player.receiveShadows = true;
  const traffic: Mesh[] = []; for (const [x, z, colour] of [[-35, 0, '#d9644d'], [12, 0, '#5a9ab1'], [42, 0, '#e5bd5f']] as const) { const car = box(scene, 'traffic-car', new Vector3(x, 0.8, z + 2.2), { width: 2.6, depth: 4.8, height: 1.1 }, mat(scene, `car-${x}`, Color3.FromHexString(colour), 0.15, 0.42)); traffic.push(car); shadows.addShadowCaster(car); }
  const input = new Set<string>(); scene.onKeyboardObservable.add(info => { if (info.type === KeyboardEventTypes.KEYDOWN) input.add(info.event.key.toLowerCase()); else input.delete(info.event.key.toLowerCase()); });
  let last = performance.now(); let time = 9; const render = () => { const now = performance.now(); const dt = Math.min((now - last) / 1000, 0.05); last = now; const forward = (input.has('w') ? 1 : 0) - (input.has('s') ? 1 : 0); const side = (input.has('d') ? 1 : 0) - (input.has('a') ? 1 : 0); const speed = input.has('shift') ? 8 : 4.5; const direction = new Vector3(side, 0, forward); if (direction.lengthSquared() > 0) { direction.normalize(); player.position.addInPlace(direction.scale(speed * dt)); player.rotation.y = Math.atan2(direction.x, direction.z); } camera.target = Vector3.Lerp(camera.target, player.position.add(new Vector3(0, 0.6, 0)), Math.min(1, dt * 8)); for (const car of traffic) { car.position.x += dt * (car.position.z > 0 ? 6 : -5); if (car.position.x > 58) car.position.x = -58; if (car.position.x < -58) car.position.x = 58; } time = (time + dt * 24 / 240) % 24; const daylight = Math.max(0.05, Math.sin((time - 6) / 24 * Math.PI * 2) * 1.4); sun.intensity = 0.3 + daylight * 1.8; hemi.intensity = 0.28 + daylight * 0.35; scene.render(); }; engine.runRenderLoop(render); const resize = () => engine.resize(); window.addEventListener('resize', resize); return { engine, scene, dispose: () => { window.removeEventListener('resize', resize); engine.dispose(); } };
}
