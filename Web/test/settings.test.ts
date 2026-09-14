import test from 'node:test';
import assert from 'node:assert/strict';
import { qualityOrDefault, signallingUrl, streamPreset } from '../src/settings.ts';
test('saved settings reject unexpected values', () => {
  assert.equal(qualityOrDefault('garbage'), 'high');
  assert.equal(qualityOrDefault(null), 'high');
  assert.equal(streamPreset('mobile').fps, 30);
  assert.equal(streamPreset('high').maxBitrate, 20_000, 'Epic bitrate settings are in kbps');
  assert.ok(streamPreset('balanced').maxBitrate < streamPreset('high').maxBitrate);
});
test('URL validation prevents mixed content and embedded secrets', () => {
  assert.equal(signallingUrl('ws://127.0.0.1:8880', 'http:'), 'ws://127.0.0.1:8880/');
  assert.equal(signallingUrl('wss://stream.example.test', 'https:'), 'wss://stream.example.test/');
  for (const url of ['https://example.test', 'file:///tmp/test', 'ws://user:pass@localhost', 'ws://localhost/#secret'])
    assert.throws(() => signallingUrl(url, 'http:'));
  assert.throws(() => signallingUrl('ws://localhost', 'https:'));
});
