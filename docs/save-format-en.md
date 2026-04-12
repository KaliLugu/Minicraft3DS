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
   - Total number of saved inventories (including chest inventories).
3. For each inventory (0 to nextInv-1):
   - `lastSlot` : sShort (2 bytes)
     - Number of items in this inventory.
   - For each item (0 to lastSlot-1):
     - `nameLen` : size_t (4 bytes on 32-bit, 8 on 64-bit)
       - Length of the item name in bytes.
     - `name` : char[nameLen]
       - Internal name of the item (e.g., "ITEM_WOOD", "TOOL_SWORD").
     - `countLevel` : sShort (2 bytes)
       - Quantity and level (durability/enchantment).
     - If the item name is "ITEM_CHEST":
       - `invIndex` : int (4 bytes)
         - Index pointing to the associated chest inventory (reference to another inventory in the list).

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
     - `ENTITY_ITEM`: `nameLen` (size_t), `name` (char[nameLen]), `countLevel` (sShort), `age` (sShort)
     - `ENTITY_FURNITURE`: `nameLen` (size_t), `name` (char[nameLen]), `invIndex` (int)
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

9. **Inventory Structure:**
   - `lastSlot` : sShort (2 bytes)
     - Number of items currently in the main inventory slots.
     - Valid range: 0 to max inventory size (typically 40 slots).

10. **Item Storage (for each slot from 0 to lastSlot-1):**
    - `nameLen` : size_t (4 bytes)
      - Length of the item name.
    - `name` : char[nameLen]
      - Internal name of the item.
    - `countLevel` : sShort (2 bytes)
      - Quantity and durability/enchantment level.
    - If the item name is "ITEM_CHEST":
      - `invIndex` : int (4 bytes)
        - Index of the chest's inventory.

11. **Inventory Size Considerations:**
    - The total inventory size (including all nested chest inventories) can grow dynamically.
    - Each chest adds approximately (2 + 4 × number_of_items) bytes.
    - The `nextInv` value in world data tracks how many separate inventories exist.

12. **Item Metadata Storage:**
    - Enchantment levels and tool durability are encoded within `countLevel`.
    - Item-specific data (e.g., custom textures for armor) may be stored separately.
    - Empty slots are represented as items with `id == 0`.

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

## Binary File Example

This section provides a concrete example of a player save file with detailed binary representation.

### Example Player Save File

Let's examine a minimal player save file with:
- Save version: 1
- Score: 1500
- Player spawned: true
- Has won: false
- Health: 20
- Position: (64, 64)
- Level: 0 (surface)
- Inventory: 3 items (10 wood, 5 stone, 1 iron sword with durability 100)

### Binary Layout and Values

```
Offset | Type       | Size | Value (Hex)              | Value (Dec) | Description
-------|------------|------|---------------------------|-------------|-------------
0x0000 | int        | 4    | 01 00 00 00               | 1           | Save version
0x0004 | int        | 4    | DC 05 00 00               | 1500        | Player score
0x0008 | bool       | 1    | 01                        | 1           | isSpawned
0x0009 | bool       | 1    | 00                        | 0           | hasWonSaved
0x000A | sShort     | 2    | 14 00                     | 20          | Health
0x000C | sShort     | 2    | 40 00                     | 64          | X position
0x000E | sShort     | 2    | 40 00                     | 64          | Y position
0x0010 | sByte      | 1    | 00                        | 0           | Level
0x0011 | sShort     | 2    | 03 00                     | 3           | lastSlot (3 items)
0x0013 | size_t     | 4    | 09 00 00 00               | 9           | nameLen item 1
0x0017 | char[9]    | 9    | 49 54 45 4D 5F 57 4F 4F 44     | -           | name "ITEM_WOOD"
0x0020 | sShort     | 2    | 0A 00                     | 10          | countLevel item 1 (10 wood)
0x0022 | size_t     | 4    | 0A 00 00 00               | 10          | nameLen item 2
0x0026 | char[10]   | 10   | 49 54 45 4D 5F 53 54 4F 4E 45   | -           | name "ITEM_STONE"
0x0030 | sShort     | 2    | 05 00                     | 5           | countLevel item 2 (5 stone)
0x0032 | size_t     | 4    | 0C 00 00 00               | 12          | nameLen item 3
0x0036 | char[12]   | 12   | 54 4F 4F 4C 5F 53 57 4F 52 44   | -           | name "TOOL_SWORD"
0x0042 | sShort     | 2    | 01 64                     | 25601       | countLevel item 3 (1 sword, durability 100)
...    | ...        | ...  | ...                       | ...         | (appearance, effects, minimap, quests continue)
```

### Hexadecimal Dump (truncated for readability)

```
01 00 00 00 DC 05 00 00 01 00 14 00 40 00 40 00 00 03 00 
09 00 00 00 49 54 45 4D 5F 57 4F 4F 44 0A 00 
0A 00 00 00 49 54 45 4D 5F 53 54 4F 4E 45 05 00 
0C 00 00 00 54 4F 4F 4C 5F 53 57 4F 52 44 01 64 
[appearance: 01 00 00 00 00 00 00] 
[effects: 00 00 00 00] 
[minimap: 128*128 bytes of data] 
[quests: 00 00 00 00]
```
00000010: 0003 0001 000A 0002 0005 0003 0064 01...  .............d.
```

### Notes on the Example

- **Byte Order:** All multi-byte values use little-endian byte order (least significant byte first).
- **Item Encoding:** The iron sword's `countLevel` (0x6401) encodes:
  - Count: 0x01 = 1 sword
  - Durability: 0x64 = 100 (stored in upper byte)
- **Data Types:**
  - `int`: 32-bit signed integer
  - `sShort`: 16-bit signed integer  
  - `sByte`: 8-bit signed integer
  - `bool`: 8-bit boolean (0 = false, 1 = true)
- **File Size:** This minimal example is 30 bytes; real files are much larger due to minimap (16KB), effects, and quests.
- **Version String:** The file would end with a 16-byte version string (not shown in this minimal example).
