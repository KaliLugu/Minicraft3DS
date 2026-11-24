#ifndef FONT_FNT_H
#define FONT_FNT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FONT_MAGIC                 "FONT"
#define FONT_FORMAT_VERSION        1
#define FONT_FLAG_IS_SDF           (1u << 0)
#define FONT_FLAG_KERNING_ENABLED  (1u << 1)

#pragma pack(push, 1)

typedef struct {
    char     magic[4];            /* 0x00 "FONT" (without \0)   */
    uint16_t version;             /* 0x04                       */
    uint16_t flags;               /* 0x06                       */
    uint16_t glyph_count;         /* 0x08                       */
    uint16_t kerning_fontsCount;  /* 0x0A                       */
    uint16_t atlas_width;         /* 0x0C                       */
    uint16_t atlas_height;        /* 0x0E                       */
    uint16_t reference_size_px;   /* 0x10                       */
    uint16_t sdf_spread_px;       /* 0x12                       */
    uint16_t line_height;         /* 0x14                       */
    uint16_t baseline_offset;     /* 0x16                       */
    uint32_t default_char;        /* 0x18                       */
    uint32_t font_id_hash;        /* 0x1C                       */
    uint8_t  reserved[32];        /* 0x20                       */
} FontBinaryHeader;

typedef struct {
    uint32_t codepoint;
    uint16_t x, y, w, h;
    int16_t  xoffset, yoffset;
    uint16_t xadvance;
    uint16_t _pad;
} FontGlyph;

typedef struct {
    uint32_t first;
    uint32_t second;
    int16_t  amount;
    uint16_t _pad;
} FontKerningPair;

#pragma pack(pop)

_Static_assert(sizeof(FontBinaryHeader) == 64, "FontBinaryHeader 64bytes");
_Static_assert(sizeof(FontGlyph)        == 20, "FontGlyph 20bytes");
_Static_assert(sizeof(FontKerningPair)  == 12, "FontKerningPair 12bytes");

typedef struct {
    void                  *data;      /* buffer brut (possédé si owns_data) */
    size_t                 size;
    bool                   owns_data;
    const FontBinaryHeader *header;
    const FontGlyph        *glyphs;
    const FontKerningPair  *kerning;
    const FontGlyph        *default_glyph;
} Font;

typedef enum {
    FONT_OK = 0,
    FONT_ERR_IO,
    FONT_ERR_NOMEM,
    FONT_ERR_TOO_SMALL,
    FONT_ERR_BAD_MAGIC,
    FONT_ERR_BAD_VERSION,
    FONT_ERR_BAD_SIZE,
    FONT_ERR_NOT_SORTED,
    FONT_ERR_NO_DEFAULT_CHAR,
    FONT_ERR_BAD_SDF,
} FontResult;

FontResult font_load_file(Font *font, const char *path);

FontResult font_from_memory(Font *font, void *data, size_t size, bool take_ownership);

void font_free(Font *font);

const FontGlyph *font_find_glyph(const Font *font, uint32_t codepoint);

const FontGlyph *font_get_glyph(const Font *font, uint32_t codepoint);

int16_t font_get_kerning(const Font *font, uint32_t first, uint32_t second);

int font_measure_utf8(const Font *font, const char *text);

uint32_t font_utf8_next(const char **s);

const char *font_result_str(FontResult r);

#endif