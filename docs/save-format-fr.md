# Format de fichier de sauvegarde

Ce document décrit le format binaire des fichiers de sauvegarde utilisés par Minicraft3DS.

## Généralités

- Le fichier est binaire, pas lisible en texte.
- Les données sont écrites dans l'ordre exact du code de sauvegarde.
- Le fichier commence toujours par un numéro de version de sauvegarde (`SAVE_VERSION`) et se termine par une chaîne de version complète.
- La version complète à la fin du fichier est ajoutée pour assurer la compatibilité entre les anciennes versions qui n'avaient pas cette information et les versions plus récentes.

## Structure du fichier de sauvegarde monde

### En-tête

1. `version` : int (4 octets)
   - Numéro de version de format de sauvegarde.

### Inventaires

2. `nextInv` : sShort (2 octets)
   - Nombre total d'inventaires sauvegardés.
3. Pour chaque inventaire :
   - `lastSlot` : sShort (2 octets)
     - Nombre d'items dans l'inventaire.
   - Pour chaque item :
     - `id` : sShort (2 octets)
     - `countLevel` : sShort (2 octets)
     - Si `id == ITEM_CHEST` :
       - `invIndex` : int (4 octets)
         - Index de l'inventaire stocké dans le coffre.

### Entités par niveau

Pour chaque niveau `0..4` :

4. `amount` : sShort (2 octets)
   - Nombre d'entités persistantes dans ce niveau.
5. Pour chaque entité persistante :
   - `type` : sShort (2 octets)
   - `x` : sShort (2 octets)
   - `y` : sShort (2 octets)
   - Champs additionnels selon le type d'entité :
     - `ENTITY_AIRWIZARD` : `health` (sShort)
     - `ENTITY_ZOMBIE`, `ENTITY_SKELETON`, `ENTITY_KNIGHT`, `ENTITY_SLIME` : `health` (sShort), `lvl` (sByte)
     - `ENTITY_ITEM` : `item.id` (sShort), `item.countLevel` (sShort), `age` (sShort)
     - `ENTITY_FURNITURE` : `itemID` (sShort), `invIndex` (int)
     - `ENTITY_PASSIVE` : `health` (sShort), `mtype` (uByte)
     - `ENTITY_DRAGON` : `health` (sShort)
     - `ENTITY_NPC` : `type` (uByte)

### Données du monde

6. `daytime` : uShort (2 octets)
7. `day` : int (4 octets)
8. `season` : uByte (1 octet)
9. `rain` : bool (1 octet)

### Compas / coordonnées des escaliers

10. `compassData` : uByte[18] (6 × 3 octets)
    - Coordonnées et compte par niveau.

### Carte

11. `map` : uByte[128 * 128 * 5] (80 000 octets)
    - Identifiants des tuiles sur les 5 niveaux de surface.
12. `data` : uByte[128 * 128 * 5] (80 000 octets)
    - Données associées aux tuiles (dégâts, âge des cultures, etc.).

### Version de build

13. `versionBuf` : char[16]
    - Chaîne de version complète du jeu, terminée par `\0` si elle est plus courte que 16 octets.

## Structure du fichier de sauvegarde joueur

La sauvegarde joueur est un fichier binaire distinct et utilise aussi `SAVE_VERSION` au début.

1. `version` : int (4 octets)
2. `score` : int (4 octets)
3. `isSpawned` : bool (1 octet)
4. `hasWonSaved` : bool (1 octet)
5. `health` : sShort (2 octets)
6. `x` : sShort (2 octets)
7. `y` : sShort (2 octets)
8. `level` : sByte (1 octet)

### Inventaire joueur

9. Même structure que pour les inventaires du monde :
   - `lastSlot` + items + éventuel `invIndex` pour coffres.

### Apparence

10. `choosen` : bool (1 octet)
11. `legs` : uByte (1 octet)
12. `body` : uByte (1 octet)
13. `arms` : uByte (1 octet)
14. `head` : uByte (1 octet)
15. `eyes` : uByte (1 octet)
16. `accs` : uByte (1 octet)

### Effets

17. `esize` : int (4 octets)
    - Nombre d'effets sauvegardés (doit être `EFFECTS_MAX`).
18. Pour chaque effet :
    - `level` : uByte (1 octet)
    - `time` : sInt (4 octets)

### Minimap

19. `minimapData` : uByte[128 * 128] (16 384 octets)

### Quêtes

20. `questManager.size` : int (4 octets)
21. Pour chaque ligne de quête :
    - `currentQuest` : int (4 octets)
    - `currentQuestDone` : bool (1 octet)

## Points importants

- Le format est strictement binaire et dépend des tailles de types utilisées dans le code.
- Toute modification de la structure des types ou de l'ordre d'écriture casse la compatibilité des sauvegardes.
- La chaîne de version complète est écrite à la fin du fichier pour faciliter la compatibilité ascendante avec les versions plus récentes.
- Les versions plus anciennes de sauvegarde qui n'avaient pas cette chaîne peuvent toujours être supportées si le loader ne dépend que du numéro de version initial.
- En cas de corruption ou de fichier tronqué, le chargement peut échouer. Il est donc important de vérifier l'intégrité des fichiers avant d'essayer de les ouvrir.
