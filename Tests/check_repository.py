"""Static wiring checks only. This is not an Unreal compile or asset validation."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
versions = json.loads((root / 'Infrastructure/versions.json').read_text())
project = json.loads((root / 'Unreal/KidzCity.uproject').read_text())
package = json.loads((root / 'Web/package.json').read_text())
assert project['EngineAssociation'] == versions['unreal']
assert any(p['Name'] == versions['pixelStreamingPlugin'] and p['Enabled'] for p in project['Plugins'])
if versions['frontendPackage'] in package['dependencies']:
    # Unreal/Pixel Streaming route.
    assert package['dependencies'][versions['frontendPackage']] == versions['frontendVersion']
else:
    # Browser-native route. Babylon is intentionally a different runtime from Pixel Streaming.
    assert package['dependencies'].get('@babylonjs/core', '').startswith('^8.')
    assert package['dependencies'].get('@babylonjs/loaders', '').startswith('^8.')
    assert '@dimforge/rapier3d-compat' in package['dependencies']
assert len(versions['infrastructureCommit']) == 40
for path in (root / 'Unreal/Source').rglob('*.h'):
    includes = [line for line in path.read_text().splitlines() if line.startswith('#include')]
    generated = [line for line in includes if '.generated.h' in line]
    if generated:
        assert generated == [includes[-1]], f'Generated header must be last include: {path}'
        assert f'"{path.stem}.generated.h"' in generated[0]
manifest = json.loads((root / 'Unreal/Config/Downtown.manifest.json').read_text())
assert len({a['id'] for a in manifest['actors']}) == len(manifest['actors'])
assert all(len(a['position']) == 3 for a in manifest['actors'])
for location in ('Hospital', 'FireStation', 'Restaurant', 'Supermarket', 'Bank'):
    assert any(a['id'] == location for a in manifest['actors'])
for path in root.rglob('*.json'):
    if not any(part in ('node_modules', 'ThirdParty', '.git', 'dist') for part in path.parts):
        json.loads(path.read_text())
print('Repository wiring checks passed; Unreal compilation and assets remain separate gates.')
