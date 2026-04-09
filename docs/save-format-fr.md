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

9. **Structure de l'inventaire :**
   - `lastSlot` : sShort (2 octets)
     - Nombre d'items actuellement présents dans les slots d'inventaire.
     - Plage valide : 0 à la taille maximale (généralement 40 slots).

10. **Stockage des items (pour chaque slot de 0 à lastSlot-1) :**
    - `id` : sShort (2 octets)
      - Identifiant du type d'item. Référence la table des types d'items.
      - Une valeur de 0 indique généralement un slot vide.
    - `countLevel` : sShort (2 octets)
      - Encode à la fois le nombre d'items et le niveau de durabilité.
      - **Nombre :** Les bits bas stockent la quantité d'items (1-999) pour les items empilables.
      - **Niveau :** Les bits hauts stockent le niveau de durabilité/enchantement (0-255) pour les outils et armes.
      - Pour les items empilables, les bits hauts ne sont pas utilisés.

11. **Références aux inventaires des coffres :**
    - Si un item a `id == ITEM_CHEST` :
      - `invIndex` : int (4 octets)
        - Index pointant vers un inventaire séparé stocké dans les données du monde.
        - Cela permet aux coffres de posséder leur propre inventaire complet.
        - L'inventaire référencé suit la même structure (lastSlot + items).

12. **Considérations sur la taille de l'inventaire :**
    - La taille totale de l'inventaire (incluant tous les inventaires imbriqués des coffres) peut croître dynamiquement.
    - Chaque coffre ajoute approximativement (2 + 4 × nombre_d_items) octets.
    - La valeur `nextInv` dans les données du monde suit le nombre d'inventaires séparés qui existent.

13. **Stockage des métadonnées des items :**
    - Les niveaux d'enchantement et la durabilité des outils sont codés dans `countLevel`.
    - Les données spécifiques aux items (ex : textures personnalisées pour les armures) peuvent être stockées séparément.
    - Les slots vides sont représentés par des items avec `id == 0`.

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

## Exemple de fichier binaire

Cette section fournit un exemple concret d'un fichier de sauvegarde joueur avec représentation binaire détaillée.

### Exemple de fichier de sauvegarde joueur

Examinons un fichier de sauvegarde joueur minimal avec :
- Version de sauvegarde : 1
- Score : 1500
- Joueur apparu : vrai
- A gagné : faux
- Santé : 20
- Position : (64, 64)
- Niveau : 0 (surface)
- Inventaire : 3 items (10 bois, 5 pierre, 1 épée en fer avec durabilité 100)

### Disposition binaire et valeurs

```
Offset | Type       | Taille | Valeur (Hex)  | Valeur (Déc) | Description
-------|------------|--------|---------------|--------------|-------------
0x0000 | int        | 4      | 01 00 00 00   | 1            | Version de sauvegarde
0x0004 | int        | 4      | DC 05 00 00   | 1500         | Score du joueur
0x0008 | bool       | 1      | 01            | 1            | isSpawned
0x0009 | bool       | 1      | 00            | 0            | hasWonSaved
0x000A | sShort     | 2      | 14 00         | 20           | Santé
0x000C | sShort     | 2      | 40 00         | 64           | Position X
0x000E | sShort     | 2      | 40 00         | 64           | Position Y
0x0010 | sByte      | 1      | 00            | 0            | Niveau
0x0011 | sShort     | 2      | 03 00         | 3            | lastSlot (taille inventaire)
0x0013 | sShort     | 2      | 01 00         | 1            | ID item 1 (bois)
0x0015 | sShort     | 2      | 0A 00         | 10           | Nombre item 1 (10 bois)
0x0017 | sShort     | 2      | 02 00         | 2            | ID item 2 (pierre)
0x0019 | sShort     | 2      | 05 00         | 5            | Nombre item 2 (5 pierre)
0x001B | sShort     | 2      | 03 00         | 3            | ID item 3 (épée en fer)
0x001D | sShort     | 2      | 01 64         | 25601        | countLevel item 3 (1 épée, durabilité 100)
...    | ...        | ...    | ...           | ...          | (apparence, effets, minimap, quêtes suivent)
```

### Dump hexadécimal

```
00000000: 0100 0000 DC05 0000 0100 1400 4000 4000  ..............@@.
00000010: 0003 0001 000A 0002 0005 0003 0064 01...  .............d.
```

### Notes sur l'exemple

- **Ordre des octets :** Toutes les valeurs multi-octets utilisent l'ordre little-endian (octet de poids faible en premier).
- **Encodage des items :** Le `countLevel` de l'épée en fer (0x6401) encode :
  - Nombre : 0x01 = 1 épée
  - Durabilité : 0x64 = 100 (stocké dans l'octet haut)
- **Types de données :**
  - `int` : entier signé 32 bits
  - `sShort` : entier signé 16 bits
  - `sByte` : entier signé 8 bits
  - `bool` : booléen 8 bits (0 = faux, 1 = vrai)
- **Taille du fichier :** Cet exemple minimal fait 30 octets ; les fichiers réels sont beaucoup plus grands à cause de la minimap (16 Ko), des effets et des quêtes.
- **Chaîne de version :** Le fichier se terminerait par une chaîne de version de 16 octets (non montrée dans cet exemple minimal).
