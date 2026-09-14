import { createBrowserGame } from './browser-game';
import './style.css';
const canvas = document.querySelector<HTMLCanvasElement>('#game-canvas');
const status = document.querySelector<HTMLElement>('#status');
const play = document.querySelector<HTMLButtonElement>('#play');
const loading = document.querySelector<HTMLElement>('#loading');
if (!canvas || !status || !play || !loading) throw new Error('KidzCity benchmark markup is incomplete.');
document.querySelector<HTMLButtonElement>('#settings')?.addEventListener('click', () => document.querySelector<HTMLDialogElement>('#preferences')?.showModal());
document.querySelector<HTMLButtonElement>('#close-settings')?.addEventListener('click', () => document.querySelector<HTMLDialogElement>('#preferences')?.close());
play.addEventListener('click', async () => {
  play.disabled = true; loading.hidden = false; status.textContent = 'Loading the WebGPU city benchmark…';
  try { await createBrowserGame(canvas); loading.hidden = true; status.textContent = 'Ready. Click the city, then use WASD to explore. Shift runs.'; canvas.focus(); }
  catch (error) { loading.hidden = true; play.disabled = false; status.textContent = error instanceof Error ? error.message : 'Could not start the browser benchmark.'; }
});
