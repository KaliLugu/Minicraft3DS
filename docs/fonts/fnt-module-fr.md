# Module `font_fnt` : lecture des polices `.fnt`

Ce module charge et interroge au runtime les fichiers `.fnt` (format FONT v1) produits par `minicraft3ds-font-json2bin`.

- **Fichiers :** `font_fnt.h`, `font_fnt.c`
- **Dépendances :** bibliothèque standard C11 uniquement (`stdio`, `stdlib`, `string`, `stdint`, `stdbool`)
- **Plateformes :** 3DS (ARM11, libctru) et PC little-endian

## Principe

Le format est binaire, packé sur 1 octet et entièrement POD, donc il n'y a pas de désérialisation. Le chargement :

1. lit le fichier entier dans un buffer ;
2. valide l'en-tête et la cohérence du fichier ;
3. fait pointer `header`, `glyphs` et `kerning` directement dans ce buffer, sans aucune copie.

Les recherches de glyphe et de kerning sont des recherches binaires en O(log n) sur les tables triées.

```txt
buffer : [ FontBinaryHeader (64) ][ FontGlyph × glyph_count (20) ][ FontKerningPair × kerning_fontsCount (12) ]
            ↑ font.header             ↑ font.glyphs                    ↑ font.kerning
```

## Démarrage rapide

```c
#include "font_fnt.h"

Font font;
FontResult r = font_load_file(&font, "romfs:/fonts/main_ui.fnt");
if (r != FONT_OK) {
    printf("Erreur police : %s\n", font_result_str(r));
    return;
}

/* Dessiner une chaîne UTF-8 */
const char *text = "Bonjour à tous";
float scale = 16.0f / font.header->reference_size_px;   /* affichage en 16 px */
float cursor_x = 10.0f, cursor_y = 20.0f;
uint32_t prev = 0, cp;

while ((cp = font_utf8_next(&text)) != 0) {
    if (prev)
        cursor_x += font_get_kerning(&font, prev, cp) * scale;

    const FontGlyph *g = font_get_glyph(&font, cp);
    draw_quad(/* position écran */ cursor_x + g->xoffset * scale,
                                  cursor_y + g->yoffset * scale,
              /* taille écran   */ g->w * scale, g->h * scale,
              /* UV atlas       */ g->x, g->y, g->w, g->h);

    cursor_x += g->xadvance * scale;
    prev = cp;
}

font_free(&font);
```

## Types

### `FontBinaryHeader`, `FontGlyph`, `FontKerningPair`

Ce sont les structures du fichier, déclarées sous `#pragma pack(push, 1)`. Leur layout est décrit en détail dans la documentation du format FNT. Des `_Static_assert` garantissent leurs tailles (64, 20 et 12 octets) : si le layout change par erreur, la compilation échoue.

### `Font`

C'est la vue runtime sur un fichier chargé.

| Champ           | Type                       | Description                                                       |
| --------------- | -------------------------- | ----------------------------------------------------------------- |
| `data`          | `void *`                   | Buffer brut du fichier.                                           |
| `size`          | `size_t`                   | Taille du buffer en octets.                                       |
| `owns_data`     | `bool`                     | Si `true`, `font_free()` libère `data` avec `free()`.             |
| `header`        | `const FontBinaryHeader *` | En-tête (pointe au début de `data`).                              |
| `glyphs`        | `const FontGlyph *`        | Table des glyphes, triée par `codepoint`.                         |
| `kerning`       | `const FontKerningPair *`  | Table de kerning, triée par `(first, second)`. Peut être vide.    |
| `default_glyph` | `const FontGlyph *`        | Glyphe de `default_char`, résolu une seule fois au chargement.    |

Tous les pointeurs sont **en lecture seule** et restent valides tant que `data` n'est pas libéré.

### `FontResult`

| Valeur                     | Signification                                                        |
| -------------------------- | -------------------------------------------------------------------- |
| `FONT_OK`                  | Succès.                                                              |
| `FONT_ERR_IO`              | Ouverture ou lecture du fichier impossible.                          |
| `FONT_ERR_NOMEM`           | Échec de `malloc`.                                                   |
| `FONT_ERR_TOO_SMALL`       | Buffer plus petit que l'en-tête (64 octets).                         |
| `FONT_ERR_BAD_MAGIC`       | Les 4 premiers octets ne valent pas `"FONT"`.                        |
| `FONT_ERR_BAD_VERSION`     | `version` différente de `FONT_FORMAT_VERSION`.                       |
| `FONT_ERR_BAD_SIZE`        | La taille ne vaut pas `64 + glyph_count*20 + kerning_fontsCount*12`. |
| `FONT_ERR_NOT_SORTED`      | Tables non triées, ou codepoint / paire de kerning en double.        |
| `FONT_ERR_NO_DEFAULT_CHAR` | `default_char` n'est pas présent dans la table des glyphes.          |
| `FONT_ERR_BAD_SDF`         | `FONT_FLAG_IS_SDF` actif mais `sdf_spread_px == 0`.                  |

## Fonctions

### `font_load_file`

```c
FontResult font_load_file(Font *font, const char *path);
```

Lit tout le fichier `path` dans un buffer alloué avec `malloc`, puis appelle `font_from_memory(font, buf, size, true)`.

- **Paramètres :** `font` reçoit la police chargée ; `path` est un chemin compatible `fopen` (`romfs:/…`, `sdmc:/…` sur 3DS).
- **Retour :** `FONT_OK`, ou un code d'erreur.
- **Mémoire :** le `Font` possède le buffer, donc il faut appeler `font_free()`. En cas d'erreur, le buffer est déjà libéré et `*font` est remis à zéro.

### `font_from_memory`

```c
FontResult font_from_memory(Font *font, void *data, size_t size, bool take_ownership);
```

Valide un buffer déjà en mémoire et initialise `font` pour pointer dedans, **sans copie**. Utile si le fichier vient d'une archive, d'un mod chargé autrement, ou d'un tableau embarqué dans l'exécutable.

Vérifications effectuées, dans cet ordre :

1. `size >= 64` ;
2. `magic == "FONT"`, puis `version == FONT_FORMAT_VERSION` ;
3. taille exacte : `64 + glyph_count*20 + kerning_fontsCount*12` ;
4. si SDF : `sdf_spread_px > 0` ;
5. glyphes strictement croissants par `codepoint` (ce qui détecte aussi les doublons) ;
6. paires de kerning strictement croissantes par `(first, second)` ;
7. présence de `default_char`.

- **`take_ownership`** : `true` si `font_free()` doit faire `free(data)`, `false` si l'appelant garde la main sur le buffer.
- **En cas d'erreur :** `*font` est remis à zéro et `data` **n'est jamais libéré** par cette fonction.
- **Alignement :** `data` doit être aligné sur 4 octets (c'est garanti par `malloc`). Fais attention si tu pointes à un offset arbitraire dans un plus gros buffer.
- **Coût :** O(n + k) à cause de la vérification du tri. Si tu fais confiance au convertisseur, tu peux mettre les étapes 5 et 6 sous `#ifdef DEBUG`.

### `font_free`

```c
void font_free(Font *font);
```

Libère `data` si `owns_data` vaut `true`, puis remet `*font` à zéro. L'appeler deux fois de suite, ou sur un `Font` à zéro, ne pose pas de problème.

### `font_find_glyph`

```c
const FontGlyph *font_find_glyph(const Font *font, uint32_t codepoint);
```

Fait une recherche binaire du glyphe correspondant à `codepoint`.

- **Retour :** un pointeur vers le glyphe, ou `NULL` s'il est absent.
- **Coût :** O(log n). La boucle est écrite à la main plutôt qu'avec `bsearch`, ce qui évite un appel par pointeur de fonction à chaque comparaison.

### `font_get_glyph`

```c
const FontGlyph *font_get_glyph(const Font *font, uint32_t codepoint);
```

Fait la même chose que `font_find_glyph`, mais renvoie `font->default_glyph` si le codepoint est absent. **Elle ne renvoie jamais `NULL`** sur une police chargée avec succès. C'est la fonction à utiliser pour le rendu.

### `font_get_kerning`

```c
int16_t font_get_kerning(const Font *font, uint32_t first, uint32_t second);
```

Renvoie l'ajustement en pixels (à `reference_size_px`) à ajouter à l'avance quand `second` suit `first`.

- Renvoie `0` si `FONT_FLAG_KERNING_ENABLED` n'est pas actif, si la table est vide, ou si la paire est absente.
- Une valeur négative rapproche les glyphes.
- La recherche binaire utilise une clé composite 64 bits, `(first << 32) | second`, équivalente au tri `(first, second)`.

### `font_utf8_next`

```c
uint32_t font_utf8_next(const char **s);
```

Décode le prochain codepoint UTF-8 de `*s` et avance le pointeur.

- **Retour :** le codepoint, ou `0` en fin de chaîne (`'\0'`).
- **Séquence invalide :** renvoie `0xFFFD` et avance d'au moins un octet, donc la boucle ne peut pas tourner à l'infini. `0xFFFD` étant en général absent de l'atlas, `font_get_glyph` tombera sur `default_char`.
- Ne vérifie pas les encodages trop longs ni les surrogates, ce qui suffit pour des textes de jeu.

### `font_measure_utf8`

```c
int font_measure_utf8(const Font *font, const char *text);
```

Renvoie la largeur en pixels, **à `reference_size_px`**, d'une chaîne UTF-8 : somme des `xadvance`, plus le kerning entre caractères consécutifs.

Pour une autre taille d'affichage, multiplie le résultat par `taille_voulue / reference_size_px`. Exemple pour centrer un texte :

```c
float scale = 12.0f / font.header->reference_size_px;
float w = font_measure_utf8(&font, "Changelog") * scale;
float x = (400.0f - w) / 2.0f;   /* écran du haut de la 3DS : 400 px */
```

Ne gère ni les retours à la ligne ni le wrapping : `'\n'` est traité comme un glyphe normal.

### `font_result_str`

```c
const char *font_result_str(FontResult r);
```

Renvoie une description lisible du code d'erreur, en ASCII, pour les logs et l'UI de debug.

## Accès aux métadonnées de l'en-tête

Les champs de l'en-tête se lisent directement :

```c
bool is_sdf   = font.header->flags & FONT_FLAG_IS_SDF;
uint16_t lh   = font.header->line_height;        /* saut de ligne      */
uint16_t base = font.header->baseline_offset;    /* ligne de base      */
uint16_t aw   = font.header->atlas_width;        /* pour normaliser UV */
uint16_t ah   = font.header->atlas_height;
uint32_t id   = font.header->font_id_hash;       /* FNV-1a du nom      */
```

Coordonnées UV normalisées d'un glyphe :

```c
float u0 = (float)g->x / aw,          v0 = (float)g->y / ah;
float u1 = (float)(g->x + g->w) / aw, v1 = (float)(g->y + g->h) / ah;
```

## Remarques

- **Thread-safety :** après le chargement, toutes les fonctions de lecture sont en lecture seule et peuvent être appelées depuis plusieurs threads. Le chargement et `font_free` ne doivent pas être concurrents avec une lecture.
- **Endianness :** le format suppose une plateforme little-endian. C'est le cas de la 3DS et des PC x86/ARM courants. Aucune conversion n'est faite.
- **Accès packés :** à cause de `#pragma pack(1)`, le compilateur peut générer des lectures octet par octet sur ARM. C'est négligeable ici. Si un profilage montre le contraire, on peut retirer le pack de `FontGlyph` et `FontKerningPair`, dont le padding est déjà explicite.
- **Polices de mods :** comme `font_from_memory` revalide tout, un fichier corrompu ou malveillant est rejeté proprement au lieu de provoquer une lecture hors du buffer.
- **Test :** `test_font.c` génère un `.fnt` minimal et vérifie le chargement, les recherches, le kerning, la mesure UTF-8 et le rejet d'un fichier corrompu.

  ```bash
  gcc -std=c11 -Wall -Wextra -O2 font_fnt.c test_font.c -o test_font && ./test_font
  ```
