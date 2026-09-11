# The `.mx` map format

An `.mx` file is the JSON map LevelEdit++ saves. It is read by more than one game
(BoxDead and DreamQuest both load it), so this document is the reference each of
them implements against. Every change to the format is additive: a reader written
for an older version still loads a newer file.

```json
{
    "formatVersion": 3,
    "name": "Courtyard",
    "tiles": {
        "Hedge": {
            "filepath": "exports/Courtyard/assets/Hedge.bmp",
            "flags": ["solid"],
            "collision": [4, 8, 24, 20],
            "locations": [[48, 48, 32, 32, 24], [80, 48, 32, 32, 24]]
        },
        "Ground": {
            "filepath": "exports/Courtyard/assets/Ground.bmp",
            "flags": [],
            "locations": [[16, 16, 32, 32, 0]]
        }
    }
}
```

## Top level

| Key | Meaning |
| --- | --- |
| `formatVersion` | The layout this file was written with. Describes the file; it does not gate loading it. |
| `name` | The map's name. |
| `tiles` | One entry per tile type. |

**Any other top-level key belongs to a game.** DreamQuest keeps portals, NPCs and
the like under `"dreamquest"`. The editor never reads or rewrites those keys, and
keeps them intact when a map is opened, edited and saved.

## A tile entry

Each key under `tiles` names a tile type. Everything in the entry is shared by
every placement of that type.

| Key | Meaning |
| --- | --- |
| `filepath` | The image to draw. |
| `flags` | What the tile *means* (version 3). See below. |
| `collision` | A blocking box smaller than the art (version 3). See below. |
| `locations` | Every placement of the tile. |

Keys the editor does not recognise are kept when the map is saved, so a game can
store its own per-tile data in an entry.

### `locations`

Each placement is `[cx, cy, w, h, elevation]`.

- **`cx, cy` is the centre of the tile's quad**, not its top-left corner. The editor
  draws a tile at `cx - w/2, cy - h/2`. A reader that wants corners converts once at
  load time.
- `w, h` are per placement, so one image can be laid at different sizes.
- `elevation` (version 2) is how far the tile stands off the floor, in pixels. `0`
  is flat ground. A raised tile draws its top face lifted by that much, with a side
  face filling the gap down to its footprint. **The footprint does not move**, so
  elevation never changes where a tile blocks. Older maps have four elements; treat
  a missing fifth as `0`.

### `flags`

A list of strings saying what the tile means. The editor does not interpret them:
an author sets them, the map stores them, and each game acts on the ones it knows
and ignores the rest. That is what lets one tileset carry meanings for several games
at once.

Flags are lowercase, with spaces written as underscores (`blocks_shots`, not
`Blocks Shots`). Readers should compare them that way.

Flags a game already acts on:

| Flag | Meaning |
| --- | --- |
| `solid` | Blocks movement. |
| `explosive` | A destructible prop that blows up. BoxDead spawns a barrel in its place instead of drawing the tile. |

Any other flag is valid. Add one in the editor's Inspector and it is saved like the
rest; it does nothing until a game is taught what it means.

#### Reading `flags` - the rule every game must follow

1. **The entry has a `flags` key** (even `[]`): that is the complete, authoritative
   list. An empty list is the author saying the tile means nothing special. Do not
   add anything to it.
2. **The entry has no `flags` key**: the map predates version 3. Infer the flags from
   the tile's name, exactly as games did before flags existed:
   - name contains `barrel`, `drum`, `explosive` or `tnt` → `["explosive"]`
     (checked first, so "Explosive Barrel" is not a wall)
   - otherwise, name contains `wall`, `block`, `rock`, `stone`, `barrier`, `fence`,
     `crate`, `pillar` or `obstacle` → `["solid"]`
   - otherwise → `[]`

   Matching is case-insensitive and on substrings.

Rule 2 is what keeps a map saved before version 3 behaving exactly as it did. The
editor applies the same inference when it opens such a map, shows the result in the
Inspector, and writes it out as an explicit `flags` list on the next save - so after
one save the name no longer decides anything.

### `collision`

An optional `[x, y, w, h]` box that blocks movement instead of the whole tile,
**relative to the top-left corner of the tile's quad** (`cx - w/2, cy - h/2`). A tree
can block at its trunk and not its canopy. Without `collision`, the whole quad blocks.

It only matters for a tile that is `solid`; it says *where* a solid tile blocks, not
*whether* it does.

## Versions

| Version | Added |
| --- | --- |
| 1 | `[x, y, w, h]` locations. |
| 2 | A fifth location element, `elevation`. |
| 3 | Per-tile `flags` and `collision`. |
