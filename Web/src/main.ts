import { Config, Flags, NumericParameters, PixelStreaming } from '@epicgames-ps/lib-pixelstreamingfrontend-ue5.7';
import { qualityOrDefault, signallingUrl, streamPreset } from './settings';
import './style.css';

const element = <T extends HTMLElement>(id: string) => document.getElementById(id) as T;
const viewport = element<HTMLDivElement>('viewport');
const play = element<HTMLButtonElement>('play');
const leave = element<HTMLButtonElement>('leave');
const status = element<HTMLParagraphElement>('status');
const empty = element<HTMLDivElement>('empty');
const resume = element<HTMLButtonElement>('resume');
const fullscreen = element<HTMLButtonElement>('fullscreen');
const preferences = element<HTMLDialogElement>('preferences');
const quality = element<HTMLSelectElement>('quality');
try { quality.value = qualityOrDefault(localStorage.getItem('kidzcity.streamQuality')); }
catch { quality.value = 'high'; }
let streaming: PixelStreaming | undefined;
let config: Config | undefined;
let active = false;
let connected = false;
let timeout: ReturnType<typeof setTimeout> | undefined;
const message = (text: string, state = 'idle') => { status.textContent = text; status.dataset.state = state; };
function enableInput(enabled: boolean) {
  config?.setFlagEnabled(Flags.KeyboardInput, enabled);
  config?.setFlagEnabled(Flags.MouseInput, enabled);
}
function disconnect(text = 'Disconnected. Your completed jobs remain saved on the game computer.', state = 'idle') {
  active = false; connected = false;
  clearTimeout(timeout);
  enableInput(false);
  streaming?.disconnect();
  document.exitPointerLock?.();
  play.disabled = false; leave.disabled = true; fullscreen.disabled = true;
  empty.hidden = false; resume.hidden = true;
  message(text, state);
}
function attachVideo(video: HTMLVideoElement) {
  video.addEventListener('playing', () => {
    if (!active) return;
    clearTimeout(timeout); connected = true;
    empty.hidden = true; resume.hidden = true; fullscreen.disabled = false;
    message('Connected. Click the game to take control. Escape releases the mouse.', 'playing');
  });
  video.addEventListener('error', () => {
    if (active) disconnect('The video stream stopped. Check the Unreal game, then reconnect.', 'error');
  });
}
const observed = new WeakSet<HTMLVideoElement>();
const watchVideo = () => viewport.querySelectorAll('video').forEach(video => {
  if (!observed.has(video)) { observed.add(video); attachVideo(video); }
});
new MutationObserver(watchVideo).observe(viewport, { childList: true, subtree: true });

play.addEventListener('click', () => {
  if (active) return;
  try {
    const url = signallingUrl(import.meta.env.VITE_SIGNALLING_URL || 'ws://127.0.0.1:8880', location.protocol);
    const preset = streamPreset(qualityOrDefault(quality.value));
    if (!streaming) {
      config = new Config({ useUrlParams: false, initialSettings: {
        ss: url, AutoConnect: false, AutoPlayVideo: true, StartVideoMuted: false,
        WaitForStreamer: true, UseMic: false, UseCamera: false,
        KeyboardInput: false, MouseInput: false, HoveringMouse: false,
        TouchInput: false, GamepadInput: false, MatchViewportRes: false,
        MaxReconnectAttempts: 0,
      } });
      streaming = new PixelStreaming(config, { videoElementParent: viewport });
      streaming.addEventListener('webRtcConnected', () => {
        if (!active) return;
        message('Stream connected. Start the video if your browser asks.', 'connecting');
        resume.hidden = false;
      });
      streaming.addEventListener('webRtcFailed', () => {
        if (active) disconnect('Could not establish the game stream. Check the local game and signalling server.', 'error');
      });
      streaming.addEventListener('webRtcDisconnected', () => {
        if (active) disconnect('Game disconnected. Restart the local game if needed, then reconnect.', 'error');
      });
      watchVideo();
    }
    config!.setNumericSetting(NumericParameters.WebRTCFPS, preset.fps);
    config!.setNumericSetting(NumericParameters.WebRTCMinBitrate, preset.minBitrate);
    config!.setNumericSetting(NumericParameters.WebRTCMaxBitrate, preset.maxBitrate);
    active = true; play.disabled = true; leave.disabled = false;
    message('Connecting to your local game…', 'connecting');
    timeout = setTimeout(() => {
      if (active && !connected) disconnect('No playable stream arrived. Start signalling and the Unreal game, then try again.', 'error');
    }, 30_000);
    streaming.connect();
  } catch (error) {
    disconnect(error instanceof Error ? error.message : 'Could not start the game connection.', 'error');
  }
});
resume.addEventListener('click', () => streaming?.play());
leave.addEventListener('click', () => disconnect());
document.addEventListener('pointerdown', event => {
  enableInput(active && connected && viewport.contains(event.target as Node) && !preferences.open);
});
window.addEventListener('blur', () => enableInput(false));
document.addEventListener('visibilitychange', () => { if (document.hidden) enableInput(false); });
fullscreen.addEventListener('click', async () => {
  try { await element<HTMLElement>('viewport').parentElement!.requestFullscreen(); }
  catch { message('Full screen is unavailable in this browser. You can keep playing in the page.'); }
});
element<HTMLButtonElement>('settings').addEventListener('click', () => {
  enableInput(false); document.exitPointerLock?.(); preferences.showModal();
});
element<HTMLButtonElement>('close-settings').addEventListener('click', () => {
  try { localStorage.setItem('kidzcity.streamQuality', qualityOrDefault(quality.value)); } catch { /* Session settings still apply. */ }
  preferences.close();
});
window.addEventListener('beforeunload', () => { active = false; streaming?.disconnect(); });
