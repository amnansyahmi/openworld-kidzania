# Browser native benchmark

This branch evaluates running KidzCity entirely in the browser. The runtime is Babylon.js 8 with WebGPU where available and WebGL fallback, plus Rapier WASM for the future physics layer. Vite produces static files suitable for Vercel; no Unreal process, signalling server or GPU streaming host is needed.

Run locally with `npm install --prefix Web` and `npm run dev --prefix Web`, then click **Play game**. The benchmark is an actual Babylon scene and render loop, not a video or static mockup. It includes PBR materials, a lit street, sidewalks, road markings, glass, four buildings, readable signs, a third-person capsule placeholder, smooth follow camera, keyboard movement, running, moving background cars, shadows, a sun cycle and full-screen canvas.

The capsule and primitive buildings are measurable placeholders only. Replace them with authored glTF assets before calling it a finished game. The next pass should import a real character, animations and modular street/building assets, add Rapier colliders, a proper camera controller, an enterable hospital room and the Doctor interaction system.

Babylon.js gives us a complete browser runtime with WebGPU and WebGL support, PBR materials, shadows, glTF loading and browser-oriented tooling. Rapier supplies WASM physics. TypeScript keeps gameplay systems reviewable. This is a stronger browser-only foundation than building every engine service directly on raw Three.js.

The target is a compact realistic-stylised downtown with desktop 1080p/60 as the first benchmark. Browser memory, GPU differences, asset download size, WebGPU support and mobile thermals remain constraints. Use compressed GLB/KTX2 assets, LODs, instancing, selective lighting and a small active simulation radius.

For Vercel, set the project root to `Web`, build command to `npm run build`, and output directory to `dist`. Later assets in `public/assets` must use relative paths. No secret is required for this benchmark.
