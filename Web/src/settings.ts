export type Quality = 'high' | 'balanced' | 'mobile';
export function qualityOrDefault(value: string | null): Quality {
  return value === 'balanced' || value === 'mobile' ? value : 'high';
}
export function streamPreset(quality: Quality) {
  return {
    // Epic's frontend settings use kilobits/second, not bits/second.
    high: { fps: 60, minBitrate: 2_000, maxBitrate: 20_000 },
    balanced: { fps: 60, minBitrate: 1_000, maxBitrate: 12_000 },
    mobile: { fps: 30, minBitrate: 500, maxBitrate: 6_000 },
  }[quality];
}
export function signallingUrl(value: string, pageProtocol: string): string {
  const url = new URL(value);
  if (!['ws:', 'wss:'].includes(url.protocol) || url.username || url.password || url.hash) {
    throw new Error('Use a WebSocket signalling URL without embedded credentials or fragments.');
  }
  if (pageProtocol === 'https:' && url.protocol !== 'wss:') {
    throw new Error('An HTTPS launcher requires secure WebSocket signalling (wss).');
  }
  return url.toString();
}
