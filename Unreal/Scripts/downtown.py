"""Run in Unreal Editor's Python console. Requires authored assets; never makes proxy geometry."""
import json
from pathlib import Path
import unreal

MANIFEST = Path(__file__).resolve().parents[1] / 'Config' / 'Downtown.manifest.json'


def validate():
    spec = json.loads(MANIFEST.read_text(encoding='utf-8'))
    paths = [spec['gameMode']] + [actor['blueprint'] for actor in spec['actors']]
    missing = sorted({path for path in paths if not unreal.EditorAssetLibrary.load_blueprint_class(path)})
    if missing:
        raise RuntimeError('Author these Blueprint assets first (Docs/EDITOR_SETUP.md):\n' + '\n'.join(missing))
    if len({actor['id'] for actor in spec['actors']}) != len(spec['actors']):
        raise ValueError('Duplicate actor IDs in Downtown manifest.')
    return spec


def assemble():
    spec = validate()  # All prerequisites checked before creating or changing a level.
    if unreal.EditorAssetLibrary.does_asset_exist(spec['map']):
        raise RuntimeError('Map already exists. Edit it in Unreal Editor; this script does not overwrite maps.')
    levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if not levels.new_level(spec['map']):
        raise RuntimeError('Could not create Downtown level.')
    for entry in spec['actors']:
        cls = unreal.EditorAssetLibrary.load_blueprint_class(entry['blueprint'])
        actor = actors.spawn_actor_from_class(cls, unreal.Vector(*entry['position']), unreal.Rotator(0, entry['yaw'], 0))
        if not actor:
            raise RuntimeError('Could not spawn ' + entry['id'])
        actor.set_actor_label(entry['id'])
    player_start = actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(*spec['playerStart']))
    if not player_start:
        raise RuntimeError('Could not create PlayerStart.')
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    world.get_world_settings().set_editor_property('default_game_mode', unreal.EditorAssetLibrary.load_blueprint_class(spec['gameMode']))
    if not levels.save_current_level():
        raise RuntimeError('Could not save Downtown.')
    unreal.log('Downtown assembled from authored assets. Add nav bounds, wire crossings/traffic, validate clearances, then set default maps. See Docs/EDITOR_SETUP.md.')
