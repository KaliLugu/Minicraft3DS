#include "fnt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FontResult font_load_file(Font *font, const char *path)
{
    memset(font, 0, sizeof(*font));

    FILE *f = fopen(path, "rb");
    if (!f) return FONT_ERR_IO;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return FONT_ERR_IO; }
    long len = ftell(f);
    if (len < 0) { fclose(f); return FONT_ERR_IO; }
    rewind(f);

    void *buf = malloc((size_t)len ? (size_t)len : 1);
    if (!buf) { fclose(f); return FONT_ERR_NOMEM; }

    size_t rd = fread(buf, 1, (size_t)len, f);
    fclose(f);
    if (rd != (size_t)len) { free(buf); return FONT_ERR_IO; }

    FontResult r = font_from_memory(font, buf, (size_t)len, true);
    if (r != FONT_OK) free(buf);
    return r;
}

FontResult font_from_memory(Font *font, void *data, size_t size, bool take_ownership)
{
    memset(font, 0, sizeof(*font));

    if (size < sizeof(FontBinaryHeader)) return FONT_ERR_TOO_SMALL;

    const FontBinaryHeader *h = (const FontBinaryHeader *)data;

    if (memcmp(h->magic, FONT_MAGIC, 4) != 0) return FONT_ERR_BAD_MAGIC;
    if (h->version != FONT_FORMAT_VERSION)    return FONT_ERR_BAD_VERSION;

    size_t expected = sizeof(FontBinaryHeader)
                    + (size_t)h->glyph_count        * sizeof(FontGlyph)
                    + (size_t)h->kerning_fontsCount * sizeof(FontKerningPair);
    if (size != expected) return FONT_ERR_BAD_SIZE;

    if ((h->flags & FONT_FLAG_IS_SDF) && h->sdf_spread_px == 0)
        return FONT_ERR_BAD_SDF;

    const uint8_t *base = (const uint8_t *)data;
    const FontGlyph *glyphs =
        (const FontGlyph *)(base + sizeof(FontBinaryHeader));
    const FontKerningPair *kerning =
        (const FontKerningPair *)(base + sizeof(FontBinaryHeader)
                                       + (size_t)h->glyph_count * sizeof(FontGlyph));

    for (uint32_t i = 1; i < h->glyph_count; i++)
        if (glyphs[i - 1].codepoint >= glyphs[i].codepoint)
            return FONT_ERR_NOT_SORTED;

    for (uint32_t i = 1; i < h->kerning_fontsCount; i++) {
        const FontKerningPair *a = &kerning[i - 1], *b = &kerning[i];
        if (a->first > b->first || (a->first == b->first && a->second >= b->second))
            return FONT_ERR_NOT_SORTED;
    }

    font->data      = data;
    font->size      = size;
    font->owns_data = take_ownership;
    font->header    = h;
    font->glyphs    = glyphs;
    font->kerning   = kerning;

    font->default_glyph = font_find_glyph(font, h->default_char);
    if (!font->default_glyph) {
        memset(font, 0, sizeof(*font));
        return FONT_ERR_NO_DEFAULT_CHAR;
    }

    return FONT_OK;
}

void font_free(Font *font)
{
    if (font->owns_data) free(font->data);
    memset(font, 0, sizeof(*font));
}

const FontGlyph *font_find_glyph(const Font *font, uint32_t cp)
{
    uint32_t lo = 0, hi = font->header->glyph_count;   /* [lo, hi) */
    while (lo < hi) {
        uint32_t mid = lo + ((hi - lo) >> 1);
        uint32_t c = font->glyphs[mid].codepoint;
        if (c == cp) return &font->glyphs[mid];
        if (c < cp)  lo = mid + 1;
        else         hi = mid;
    }
    return NULL;
}

const FontGlyph *font_get_glyph(const Font *font, uint32_t cp)
{
    const FontGlyph *g = font_find_glyph(font, cp);
    return g ? g : font->default_glyph;
}

int16_t font_get_kerning(const Font *font, uint32_t first, uint32_t second)
{
    const FontBinaryHeader *h = font->header;
    if (!(h->flags & FONT_FLAG_KERNING_ENABLED) || h->kerning_fontsCount == 0)
        return 0;

    uint64_t key = ((uint64_t)first << 32) | second;

    uint32_t lo = 0, hi = h->kerning_fontsCount;
    while (lo < hi) {
        uint32_t mid = lo + ((hi - lo) >> 1);
        const FontKerningPair *p = &font->kerning[mid];
        uint64_t k = ((uint64_t)p->first << 32) | p->second;
        if (k == key) return p->amount;
        if (k < key)  lo = mid + 1;
        else          hi = mid;
    }
    return 0;
}

uint32_t font_utf8_next(const char **s)
{
    const uint8_t *p = (const uint8_t *)*s;
    uint32_t c = p[0];
    if (c == 0) return 0;

    int extra;
    if      (c < 0x80)           { extra = 0; }
    else if ((c & 0xE0) == 0xC0) { extra = 1; c &= 0x1F; }
    else if ((c & 0xF0) == 0xE0) { extra = 2; c &= 0x0F; }
    else if ((c & 0xF8) == 0xF0) { extra = 3; c &= 0x07; }
    else { *s += 1; return 0xFFFD; } // bad byte

    for (int i = 1; i <= extra; i++) {
        if ((p[i] & 0xC0) != 0x80) { *s += i; return 0xFFFD; }
        c = (c << 6) | (p[i] & 0x3F);
    }
    *s += 1 + extra;
    return c;
}

int font_measure_utf8(const Font *font, const char *text)
{
    int width = 0;
    uint32_t prev = 0, cp;
    while ((cp = font_utf8_next(&text)) != 0) {
        if (prev) width += font_get_kerning(font, prev, cp);
        width += font_get_glyph(font, cp)->xadvance;
        prev = cp;
    }
    return width;
}

const char *font_result_str(FontResult r)
{
    switch (r) {
    case FONT_OK:                 return "ok";
    case FONT_ERR_IO:             return "loading error";
    case FONT_ERR_NOMEM:          return "out of memory";
    case FONT_ERR_TOO_SMALL:      return "file is too small";
    case FONT_ERR_BAD_MAGIC:      return "bad file format";
    case FONT_ERR_BAD_VERSION:    return "Unsupported format version";
    case FONT_ERR_BAD_SIZE:       return "size does not match the header";
    case FONT_ERR_NOT_SORTED:     return "Unsorted tables or duplicates";
    case FONT_ERR_NO_DEFAULT_CHAR:return "default_char missing";
    case FONT_ERR_BAD_SDF:        return "SDF without sdf_spread_px";
    }
    return "unknown";
}