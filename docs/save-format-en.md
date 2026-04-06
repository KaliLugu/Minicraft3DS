# Save File Format

This document describes the binary save file format used by Minicraft3DS.

## Overview

- Save files are binary, not text.
- Data is written in the exact order defined by the save code.
- Each save file begins with a save format version number (`SAVE_VERSION`) and ends with a full version string.
- The full version string at the end of the file is included to ensure compatibility between older versions that did not store it and newer versions that do.

## World Save File Structure

### Header

1. `version` : int (4 bytes)
   - Save format version number.

### Inventory Data

2. `nextInv` : sShort (2 bytes)
   - Number of saved inventories.
3. For each inventory:
   - `lastSlot` : sShort (2 bytes)
     - Number of items in the inventory.
   - For each item:
     - `id` : sShort (2 bytes)
     - `countLevel` : sShort (2 bytes)
     - If `id == ITEM_CHEST`:
       - `invIndex` : int (4 bytes)
         - Index of the inventory stored in the chest.

### Entity Data by Level

For each level `0..4`:

4. `amount` : sShort (2 bytes)
   - Number of persistent entities in that level.
5. For each persistent entity:
   - `type` : sShort (2 bytes)
   - `x` : sShort (2 bytes)
   - `y` : sShort (2 bytes)
   - Additional fields depending on entity type:
     - `ENTITY_AIRWIZARD`: `health` (sShort)
     - `ENTITY_ZOMBIE`, `ENTITY_SKELETON`, `ENTITY_KNIGHT`, `ENTITY_SLIME`: `health` (sShort), `lvl` (sByte)
     - `ENTITY_ITEM`: `item.id` (sShort), `item.countLevel` (sShort), `age` (sShort)
     - `ENTITY_FURNITURE`: `itemID` (sShort), `invIndex` (int)
     - `ENTITY_PASSIVE`: `health` (sShort), `mtype` (uByte)
     - `ENTITY_DRAGON`: `health` (sShort)
     - `ENTITY_NPC`: `type` (uByte)

### World State

6. `daytime` : uShort (2 bytes)
7. `day` : int (4 bytes)
8. `season` : uByte (1 byte)
9. `rain` : bool (1 byte)

### Compass Data

10. `compassData` : uByte[18] (6 × 3 bytes)
    - Stair coordinates and level counts.

### Map Data

11. `map` : uByte[128 * 128 * 5] (80 KB)
    - Tile IDs for the 5 surface levels.
12. `data` : uByte[128 * 128 * 5] (80 KB)
    - Tile metadata (damage, crop age, saplings, etc.).

### Build Version

13. `versionBuf` : char[16]
    - Full game version string, padded or null-terminated.

## Player Save File Structure

The player save file is a separate binary file that also starts with `SAVE_VERSION`.

1. `version` : int (4 bytes)
2. `score` : int (4 bytes)
3. `isSpawned` : bool (1 byte)
4. `hasWonSaved` : bool (1 byte)
5. `health` : sShort (2 bytes)
6. `x` : sShort (2 bytes)
7. `y` : sShort (2 bytes)
8. `level` : sByte (1 byte)

### Player Inventory

9. Same inventory structure as world inventories:
   - `lastSlot` + items + optional `invIndex` for chests.

### Sprite Appearance

10. `choosen` : bool (1 byte)
11. `legs` : uByte (1 byte)
12. `body` : uByte (1 byte)
13. `arms` : uByte (1 byte)
14. `head` : uByte (1 byte)
15. `eyes` : uByte (1 byte)
16. `accs` : uByte (1 byte)

### Effects

17. `esize` : int (4 bytes)
    - Number of saved effects (`EFFECTS_MAX`).
18. For each effect:
    - `level` : uByte (1 byte)
    - `time` : sInt (4 bytes)

### Minimap

19. `minimapData` : uByte[128 * 128] (16 KB)

### Quests

20. `questManager.size` : int (4 bytes)
21. For each questline:
    - `currentQuest` : int (4 bytes)
    - `currentQuestDone` : bool (1 byte)

## Important Notes

- The save format depends on the exact binary sizes and order of fields in code.
- Changing field order or type sizes breaks compatibility.
- The final version string is written to support compatibility with newer versions while preserving older saves.
- Older saves without the trailing version string should still be usable if the loader relies on the initial save version number.
- A truncated or corrupted file may fail to load.
