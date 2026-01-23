# comment les textures sotn attribuées aux tiles

## architecture générale

1. **image source** : icons.png (512*512 pixels) - contient toutes les textures
2. **Tableaux de textures** : Les textures sotn découpéees en tableaux :
    - tile8[(512/8)*(512/8)] = 64\*64=4096 textures de 8\*8px
    - tile16[(512/16)*(512/16)] = 32\*32=1024 textures de 16\*16px
    - tile32 et tile64 pour d'autres tailles
3. Système d'index : chaque texture est accédée par index = xTile + yTile * (512/size)

## Comment ajouter une nouvelle tile

.1 Définir l'ID de la tile -> [TilesData.h](../source/data/TilesData.h)

```c
#define TILE_YOUR_TILE 10  // la valeur est l'id unique de la tile
````

.2 Enregistrer le nom -> [TilesData.c](../source/data/TilesData.c)

```c
void tilesDataInit() {
    // ... autres registrations
    _tileRegister(TILE_YOUR_TILE, "Your Tile Name");
}
```

.3 Ajouter la logique de rendu -> [RenderTiles.c](../source/render/RenderTiles.c) fonction renderTile() :

```c
case TILE_YOUR_TILE:
    // Exemple simple (une seul texture 16*16 au pixel 20.7)
    renderTile(TILE_WOOD_DOOR_CLOSED, 0, level, x, y);
    renderTile16(x, y, 20, 7, 0);
    break;

// Ou avec tiling connecté (comme GRASS/ROCK)
case TILE_YOUR_TILE:
    checkSurrTiles8(level, x >> 4, y >> 4, TILE_YOUR_TILE);
    // utilisez renderConnectedTile4 ou renderConnectedTile8 selon le besoin (4-way / 8-way)
    // Ex: renderConnectedTile8(x, y, 22, 5); // colonne 22, ligne 5 (indices en 16px)
    break;
```

.4 Mettre a jour `MAX_TILE_ID_USED` -> [TilesData.h](../source/data/TilesData.h)

```c
#define MAX_TILE_ID_USED 39 // adapté au nombre de tile
```

.5 ajouter les pixels dans [icon.png](../resources/icons.png)

- Les coordonnées `renderTile16(x, y, **xTile**, **yTile**, 0)` correspondent à la grille

- pour une texture 16*16 aux pixels (320, 112) : `xTile = 320/16 = 20`, `yTile = 112/16 = 7`

## Textures connectées (tiling connecté) — détails et layout

Si vous utilisez des textures connectées (tiled textures) comme l'herbe, l'eau, la roche, etc. ; il y a quelques concepts importants à connaître :

- Les fonctions disponibles
    - `checkSurrTiles4(level, xt, yt, id)` : Remplit les flags `tu`, `td`, `tl`, `tr` (haut/bas/gauche/droite) en vérifiant les voisins sur la grille 4-way (4 directions).

    - `checkSurrTiles8(level, xt, yt, id)` : Remplit les mêmes flags ainsi que `tul`, `tur`, `tdl`, `tdr` (diagonales) pour la grille 8-way.

    - `renderConnectedTile4(x, y, xTile, yTile)` : Utilise les flags `tl`, `tr`, `tu`, `td` et découpe la tuile 16x16 en 4 sous-tiles 8x8 en choisissant la variante appropriée.

    - `renderConnectedTile8(x, y, xTile, yTile)` : Comme ci-dessus mais gère aussi les diagonales (8 connexions) et peut prendre en compte des coin/angle spécifique.

> Important: Always call `checkSurrTiles4` or `checkSurrTiles8` **before** `renderConnectedTile*` so the flags are set. The function `renderTile()` already does so for many built-in tiles.

### Comment calculer `xTile` et `yTile`

- `xTile` et `yTile` sont des indices sur une grille 16x16 px (pour `renderConnectedTile*` et `renderTile16`).
- Pour une texture positionnée à la coordonnée pixel (px, py) dans `icons.png`, calculez :
    - `xTile = px / 16`
    - `yTile = py / 16`
    - Ex. la texture qui commence à pixel (320, 112) → `xTile = 320/16 = 20`, `yTile = 112/16 = 7`.

### Ce que fait la fonction en interne

- `renderConnectedTile4()` et `renderConnectedTile8()` convertissent l'indice 16px en index 8px :
    - `xTile_8 = xTile << 1` (multiplié par 2)
    - `yTile_8 = yTile << 1`
    - Les 4 sous-tiles (8x8) occupant la tuile 16x16 sont donc piochées à partir de cette grille 8px.
- Les offsets pour chaque quadrant sont déterminés par les flags (tl, tr, tu, td) :
    - l = (tl ? 2 : 0), r = (tr ? 2 : 0), u = (tu ? 4 : 0), d = (td ? 4 : 0)
    - pour `renderConnectedTile8`, si une diagonale est présente (par ex. `tl && tu && tul`) on ajoute `+2` de plus pour sélectionner le coin (arrondi) adapté.

### Mise en page (layout) requise dans `icons.png` pour `renderConnectedTile8`

Le layout attendu par `renderConnectedTile8()` est une petite grille de 8 colonnes (8px) × 2 lignes (8px) pour les variations de quadrant, **expliquée ci‑dessous** :

Notation : `X` = base 16px tile start; les indices sont en `8px grid index` après `xTile<<1`.

8px-grid index:  xTile_8 -> xTile_8 + 7
Row 0 (yTile_8):  TL0, TR0, TL_left, TR_right, TL_up, TR_up, TL_leftup, TR_rightup
Row 1 (yTile_8+1): BL0, BR0, BL_left, BR_right, BL_down, BR_down, BL_leftdown, BR_rightdown

ASCII simplifié (xTile_8 = X, yTile_8 = Y):

        X+0 (TL base)   X+1 (TR base)   X+2 (TL left)   X+3 (TR right)   X+4 (TL up)      X+5 (TR up)      X+6 (TL left+up)  X+7 (TR right+up)
        X+0 (BL base)   X+1 (BR base)   X+2 (BL left)   X+3 (BR right)   X+4 (BL down)    X+5 (BR down)    X+6 (BL left+down) X+7 (BR right+down)

Explication :
- Si un voisin gauche existe (`tl`), la colonne `+2` (ou `+2` combinée) est utilisée pour la variante du quadrant gauche.
- Si un voisin haut existe (`tu`), la colonne `+4` est utilisée pour la variante "haut". Ces valeurs s'additionnent (l + u = 2 + 4 = 6) pour les combinaisons.
- Une variante de coin (diagonale) ajoute une petite correction supplémentaire via `+2` (voir la condition du code: `(tl && tu && tul) ? 2 : 0`).

### Exemple concret — WATER (utilisé dans le code)

- Appel dans code : `renderConnectedTile8(x, y, 16, 9);`
- Position 16px : `xTile = 16`, `yTile = 9`
    - Origine pixel dans `icons.png` : `(16*16, 9*16)` = `(256 px, 144 px)`.
    - 8px grid : `xTile_8 = 32`, `yTile_8 = 18` (donc `xTile_8*8 = 256 px`, `yTile_8*8 = 144 px`).
- Si le tile n'a aucun voisin, la top-left 8px est à `(xTile_8 * 8, yTile_8 * 8)` = `(256, 144)` ; top-right à `(256+8, 144)` etc.
- Si par ex. `tl == true` (left neighbor), la top-left 8px utilisée devient la colonne `xTile_8 + 2` → pixel `(256 + 16, 144)`.

### Tile complet(e) quand entouré(e)
- `renderConnectedTile4()` utilise une tuile 16x16 complète située au `xTile + 3` (colonne 3 relative au `xTile`) si entourée des 4 voisins (tl, tr, tu, td).
- `renderConnectedTile8()` utilise une tuile 16x16 complète située au `xTile + 4` si entourée des 8 voisins.

### Détails supplémentaires et astuces
- Les fonctions `renderTile8/16` attendent `xTile,yTile` en indices de la grille correspondante (8px/16px). Pour `renderConnectedTile*`, fournissez les indices 16px (`xTile`, `yTile`).
- `renderConnectedTile4()` est pour 4-way (haut/gauche/droite/bas) ; `renderConnectedTile8()` s'occupe aussi des coins/diagonales.
- L'appel type (dans `renderTile()`):
```c
checkSurrTiles8(level, x >> 4, y >> 4, TILE_YOUR_TILE);
renderConnectedTile8(x, y, xTile, yTile);
```
- Si vous avez un comportement étrange (mauvaise quad sélectionnée), vérifiez :
    - que la palette 8px est bien ordonnée comme dans le diagramme ci‑dessus (8 colonnes × 2 lignes),
    - que `xTile`/`yTile` sont bien les indices 16px (ne pas donner d'index 8px par erreur),
    - que `checkSurrTiles4/8` est appelé avant `renderConnectedTile*` et que vous n'avez pas modifié les flags globales entre les deux appels.

Si vous voulez, je peux ajouter un schéma exemple pour un tileset concret (par ex. WATER à `16,9`) en explicitant chaque quadrant en pixels dans `icons.png`.

✅ Tip: Pour tester, vous pouvez créer une texture test dans `icons.png` où les 8 variantes de 8x8 sont visuellement distinctes et vérifier dans le jeu que chaque combinaison de voisins affiche la bonne variante.

