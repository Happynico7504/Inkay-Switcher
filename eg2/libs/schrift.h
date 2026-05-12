/*/ 
 * This file is part of libschrift.
/*/
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "eg2/libs/typedefs.h"
#define SFT_DOWNWARD_Y 0x01
typedef ul32 SFT_UChar;
typedef uf32 SFT_Glyph;
struct SFT_Font {
    const u8 *memory;
    uf32 size;
    int source;
    ul16 unitsPerEm;
    sl16 locaFormat;
    ul16 numLongHmtx;
};
struct SFT {
    SFT_Font *font;
    f64 xScale;
    f64 yScale;
    f64 xOffset;
    f64 yOffset;
    int flags;
};
struct SFT_LMetrics {
    f64 ascender;
    f64 descender;
    f64 lineGap;
};
struct SFT_GMetrics {
    f64 advanceWidth;
    f64 leftSideBearing;
    int yOffset;
    int minWidth;
    int minHeight;
};
struct SFT_Kerning {
    f64 xShift;
    f64 yShift;
};
struct SFT_Image {
    void *pixels;
    int width;
    int height;
};
const char *sft_version(void);
SFT_Font *sft_loadmem(const void *mem, usize size);
void sft_freefont(SFT_Font *font);
int sft_lmetrics(const SFT *sft, SFT_LMetrics *metrics);
int sft_lookup(const SFT *sft, SFT_UChar codepoint, SFT_Glyph *glyph);
int sft_gmetrics(const SFT *sft, SFT_Glyph glyph, SFT_GMetrics *metrics);
int sft_kerning(const SFT *sft, SFT_Glyph leftGlyph, SFT_Glyph rightGlyph, SFT_Kerning *kerning);
int sft_render(const SFT *sft, SFT_Glyph glyph, SFT_Image image);
#ifdef SCHRIFT_C
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#define SCHRIFT_VERSION "0.10.2"
#define FILE_MAGIC_ONE             0x00010000
#define FILE_MAGIC_TWO             0x74727565
#define HORIZONTAL_KERNING         0x01
#define MINIMUM_KERNING            0x02
#define CROSS_STREAM_KERNING       0x04
#define OVERRIDE_KERNING           0x08
#define POINT_IS_ON_CURVE          0x01
#define X_CHANGE_IS_SMALL          0x02
#define Y_CHANGE_IS_SMALL          0x04
#define REPEAT_FLAG                0x08
#define X_CHANGE_IS_ZERO           0x10
#define X_CHANGE_IS_POSITIVE       0x10
#define Y_CHANGE_IS_ZERO           0x20
#define Y_CHANGE_IS_POSITIVE       0x20
#define OFFSETS_ARE_LARGE          0x001
#define ACTUAL_XY_OFFSETS          0x002
#define GOT_A_SINGLE_SCALE         0x008
#define THERE_ARE_MORE_COMPONENTS  0x020
#define GOT_AN_X_AND_Y_SCALE       0x040
#define GOT_A_SCALE_MATRIX         0x080
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SIGN(x)   (((x) > 0) - ((x) < 0))
#define STACK_ALLOC(var, type, thresh, count) type var##_stack_[thresh]; var = ((count) <= (thresh)) ? var##_stack_ : (type*)calloc((count), sizeof(type));
#define STACK_FREE(var) if (var != var##_stack_) free(var);
enum { SrcMapping, SrcUser };
struct Point { f64 x, y; };
struct Line  { ul16 beg, end; };
struct Curve { ul16 beg, end, ctrl; };
struct Cell  { f64 area, cover; };
struct Outline {
    Point *points;
    Curve *curves;
    Line *lines;
    ul16 numPoints;
    ul16 capPoints;
    ul16 numCurves;
    ul16 capCurves;
    ul16 numLines;
    ul16 capLines;
};
struct Raster {
    Cell *cells;
    int width;
    int height;
};
void *reallocarray(void *optr, usize nmemb, usize size);
static inline int fast_floor(f64 x);
static inline int fast_ceil(f64 x);
static int init_font(SFT_Font *font);
static Point midpoint(Point a, Point b);
static void transform_points(uint numPts, Point *points, f64 trf[6]);
static void clip_points(uint numPts, Point *points, int width, int height);
static int init_outline(Outline *outl);
static void free_outline(Outline *outl);
static int grow_points(Outline *outl);
static int grow_curves(Outline *outl);
static int grow_lines(Outline *outl);
static inline int is_safe_offset(SFT_Font *font, uf32 offset, uf32 margin);
static void *csearch(const void *key, const void *base, usize nmemb, usize size, int (*compar)(const void *, const void *));
static int cmpu16(const void *a, const void *b);
static int cmpu32(const void *a, const void *b);
static inline ul8 getu8(SFT_Font *font, uf32 offset);
static inline sl8 geti8(SFT_Font *font, uf32 offset);
static inline ul16 getu16(SFT_Font *font, uf32 offset);
static inline sl16 geti16(SFT_Font *font, uf32 offset);
static inline ul32 getu32(SFT_Font *font, uf32 offset);
static int gettable(SFT_Font *font, const char tag[4], uf32 *offset);
static int cmap_fmt4(SFT_Font *font, uf32 table, SFT_UChar charCode, uf32 *glyph);
static int cmap_fmt6(SFT_Font *font, uf32 table, SFT_UChar charCode, uf32 *glyph);
static int glyph_id(SFT_Font *font, SFT_UChar charCode, uf32 *glyph);
static int hor_metrics(SFT_Font *font, uf32 glyph, int *advanceWidth, int *leftSideBearing);
static int glyph_bbox(const SFT *sft, uf32 outline, int box[4]);
static int outline_offset(SFT_Font *font, uf32 glyph, uf32 *offset);
static int simple_flags(SFT_Font *font, uf32 *offset, uf16 numPts, u8 *flags);
static int simple_points(SFT_Font *font, uf32 offset, uf16 numPts, u8 *flags, Point *points);
static int decode_contour(u8 *flags, uf16 basePoint, uf16 count, Outline *outl);
static int simple_outline(SFT_Font *font, uf32 offset, uint numContours, Outline *outl);
static int compound_outline(SFT_Font *font, uf32 offset, int recDepth, Outline *outl);
static int decode_outline(SFT_Font *font, uf32 offset, int recDepth, Outline *outl);
static int is_flat(Outline *outl, Curve curve);
static int tesselate_curve(Curve curve, Outline *outl);
static int tesselate_curves(Outline *outl);
static void draw_line(Raster buf, Point origin, Point goal);
static void draw_lines(Outline *outl, Raster buf);
static void post_process(Raster buf, u8 *image);
static int render_outline(Outline *outl, f64 transform[6], SFT_Image image);
const char *sft_version(void) { return SCHRIFT_VERSION; }
SFT_Font *sft_loadmem(const void *mem, usize size) {
    SFT_Font *font;
    if (size > UINT32_MAX) return NULL;
    if (!(font = (SFT_Font*)calloc(1, sizeof *font))) return NULL;
    font->memory = (const u8*)mem;
    font->size   = (uf32) size;
    font->source = SrcUser;
    if (init_font(font) < 0) {
        sft_freefont(font);
        return NULL;
    }
    return font;
}
void sft_freefont(SFT_Font *font) {
    if (!font) return;
    free(font);
}
int sft_lmetrics(const SFT *sft, SFT_LMetrics *metrics) {
    f64 factor;
    uf32 hhea;
    memset(metrics, 0, sizeof *metrics);
    if (gettable(sft->font, "hhea", &hhea) < 0) return -1;
    if (!is_safe_offset(sft->font, hhea, 36)) return -1;
    factor = sft->yScale / sft->font->unitsPerEm;
    metrics->ascender  = geti16(sft->font, hhea + 4) * factor;
    metrics->descender = geti16(sft->font, hhea + 6) * factor;
    metrics->lineGap   = geti16(sft->font, hhea + 8) * factor;
    return 0;
}
int sft_lookup(const SFT *sft, SFT_UChar codepoint, SFT_Glyph *glyph) { return glyph_id(sft->font, codepoint, glyph); }
int sft_gmetrics(const SFT *sft, SFT_Glyph glyph, SFT_GMetrics *metrics) {
    int adv, lsb;
    f64 xScale = sft->xScale / sft->font->unitsPerEm;
    uf32 outline;
    int bbox[4];
    memset(metrics, 0, sizeof *metrics);
    if (hor_metrics(sft->font, glyph, &adv, &lsb) < 0) return -1;
    metrics->advanceWidth    = adv * xScale;
    metrics->leftSideBearing = lsb * xScale + sft->xOffset;
    if (outline_offset(sft->font, glyph, &outline) < 0) return -1;
    if (!outline) return 0;
    if (glyph_bbox(sft, outline, bbox) < 0) return -1;
    metrics->minWidth  = bbox[2] - bbox[0] + 1;
    metrics->minHeight = bbox[3] - bbox[1] + 1;
    metrics->yOffset   = sft->flags & SFT_DOWNWARD_Y ? -bbox[3] : bbox[1];
    return 0;
}
int sft_kerning(const SFT *sft, SFT_Glyph leftGlyph, SFT_Glyph rightGlyph, SFT_Kerning *kerning) {
    void *match;
    uf32 offset;
    uint numTables, numPairs, length, format, flags;
    int value;
    u8 key[4];
    memset(kerning, 0, sizeof *kerning);
    if (gettable(sft->font, "kern", &offset) < 0) return 0;
    if (!is_safe_offset(sft->font, offset, 4)) return -1;
    if (getu16(sft->font, offset) != 0) return 0;
    numTables = getu16(sft->font, offset + 2);
    offset += 4;
    while (numTables > 0) {
        if (!is_safe_offset(sft->font, offset, 6)) return -1;
        length = getu16(sft->font, offset + 2);
        format = getu8 (sft->font, offset + 4);
        flags  = getu8 (sft->font, offset + 5);
        offset += 6;
        if (format == 0 && (flags & HORIZONTAL_KERNING) && !(flags & MINIMUM_KERNING)) {
            if (!is_safe_offset(sft->font, offset, 8)) return -1;
            numPairs = getu16(sft->font, offset);
            offset += 8;
            key[0] = (leftGlyph  >> 8) & 0xFF;
            key[1] =  leftGlyph  & 0xFF;
            key[2] = (rightGlyph >> 8) & 0xFF;
            key[3] =  rightGlyph & 0xFF;
            if ((match = bsearch(key, sft->font->memory + offset, numPairs, 6, cmpu32)) != NULL) {
                value = geti16(sft->font, (uf32) ((u8 *) match - sft->font->memory + 4));
                if (flags & CROSS_STREAM_KERNING) kerning->yShift += value;
                else kerning->xShift += value;
            }
        }
        offset += length;
        --numTables;
    }
    kerning->xShift = kerning->xShift / sft->font->unitsPerEm * sft->xScale;
    kerning->yShift = kerning->yShift / sft->font->unitsPerEm * sft->yScale;
    return 0;
}
int sft_render(const SFT *sft, SFT_Glyph glyph, SFT_Image image) {
    uf32 outline;
    f64 transform[6];
    int bbox[4];
    Outline outl;
    if (outline_offset(sft->font, glyph, &outline) < 0) return -1;
    if (!outline) return 0;
    if (glyph_bbox(sft, outline, bbox) < 0) return -1;
    transform[0] = sft->xScale / sft->font->unitsPerEm;
    transform[1] = 0.0;
    transform[2] = 0.0;
    transform[4] = sft->xOffset - bbox[0];
    if (sft->flags & SFT_DOWNWARD_Y) {
        transform[3] = -sft->yScale / sft->font->unitsPerEm;
        transform[5] = bbox[3] - sft->yOffset;
    } else {
        transform[3] = +sft->yScale / sft->font->unitsPerEm;
        transform[5] = sft->yOffset - bbox[1];
    }
    memset(&outl, 0, sizeof outl);
    if (init_outline(&outl) < 0) goto failure;
    if (decode_outline(sft->font, outline, 0, &outl) < 0) goto failure;
    if (render_outline(&outl, transform, image) < 0) goto failure;
    free_outline(&outl);
    return 0;
    failure:
        free_outline(&outl);
        return -1;
}
#define MUL_NO_OVERFLOW	((usize)1 << (sizeof(usize) * 4))
void *reallocarray(void *optr, usize nmemb, usize size) {
    if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
        nmemb > 0 && SIZE_MAX / nmemb < size) {
        errno = ENOMEM;
        return NULL;
    }
    return realloc(optr, size * nmemb);
}
static inline int fast_floor(f64 x) {
    int i = (int) x;
    return i - (i > x);
}
static inline int fast_ceil(f64 x) {
    int i = (int) x;
    return i + (i < x);
}
static int init_font(SFT_Font *font) {
    uf32 scalerType, head, hhea;
    if (!is_safe_offset(font, 0, 12)) return -1;
    scalerType = getu32(font, 0);
    if (scalerType != FILE_MAGIC_ONE && scalerType != FILE_MAGIC_TWO) return -1;
    if (gettable(font, "head", &head) < 0) return -1;
    if (!is_safe_offset(font, head, 54)) return -1;
    font->unitsPerEm = getu16(font, head + 18);
    font->locaFormat = geti16(font, head + 50);
    if (gettable(font, "hhea", &hhea) < 0) return -1;
    if (!is_safe_offset(font, hhea, 36)) return -1;
    font->numLongHmtx = getu16(font, hhea + 34);
    return 0;
}
static Point midpoint(Point a, Point b) { return (Point) {0.5 * (a.x + b.x), 0.5 * (a.y + b.y)}; }
static void transform_points(uint numPts, Point *points, f64 trf[6]) {
    Point pt;
    uint i;
    for (i = 0; i < numPts; ++i) {
        pt = points[i];
        points[i] = (Point) {pt.x * trf[0] + pt.y * trf[2] + trf[4], pt.x * trf[1] + pt.y * trf[3] + trf[5]};
    }
}
static void clip_points(uint numPts, Point *points, int width, int height) {
    Point pt;
    uint i;
    for (i = 0; i < numPts; ++i) {
        pt = points[i];
        if (pt.x < 0.0) points[i].x = 0.0;
        if (pt.x >= width) points[i].x = nextafter(width, 0.0);
        if (pt.y < 0.0) points[i].y = 0.0;
        if (pt.y >= height) points[i].y = nextafter(height, 0.0);
    }
}
static int init_outline(Outline *outl) {
    outl->numPoints = 0;
    outl->capPoints = 64;
    if (!(outl->points = (Point*)malloc(outl->capPoints * sizeof *outl->points))) return -1;
    outl->numCurves = 0;
    outl->capCurves = 64;
    if (!(outl->curves = (Curve*)malloc(outl->capCurves * sizeof *outl->curves))) return -1;
    outl->numLines = 0;
    outl->capLines = 64;
    if (!(outl->lines = (Line*)malloc(outl->capLines * sizeof *outl->lines))) return -1;
    return 0;
}
static void free_outline(Outline *outl) {
    free(outl->points);
    free(outl->curves);
    free(outl->lines);
}
static int grow_points(Outline *outl) {
    void *mem;
    uf16 cap;
    assert(outl->capPoints);
    if (outl->capPoints > UINT16_MAX / 2) return -1;
    cap = (uf16) (2U * outl->capPoints);
    if (!(mem = reallocarray(outl->points, cap, sizeof *outl->points))) return -1;
    outl->capPoints = (ul16) cap;
    outl->points    = (Point*)mem;
    return 0;
}
static int grow_curves(Outline *outl) {
    void *mem;
    uf16 cap;
    assert(outl->capCurves);
    if (outl->capCurves > UINT16_MAX / 2) return -1;
    cap = (uf16) (2U * outl->capCurves);
    if (!(mem = reallocarray(outl->curves, cap, sizeof *outl->curves))) return -1;
    outl->capCurves = (ul16) cap;
    outl->curves    = (Curve*)mem;
    return 0;
}
static int grow_lines(Outline *outl) {
    void *mem;
    uf16 cap;
    assert(outl->capLines);
    if (outl->capLines > UINT16_MAX / 2) return -1;
    cap = (uf16) (2U * outl->capLines);
    if (!(mem = reallocarray(outl->lines, cap, sizeof *outl->lines))) return -1;
    outl->capLines = (ul16) cap;
    outl->lines    = (Line*)mem;
    return 0;
}
static inline int is_safe_offset(SFT_Font *font, uf32 offset, uf32 margin) {
    if (offset > font->size) return 0;
    if (font->size - offset < margin) return 0;
    return 1;
}
static void *csearch(const void *key, const void *base, usize nmemb, usize size, int (*compar)(const void *, const void *)) {
    const u8 *bytes = (const u8*)base, *sample;
    usize low = 0, high = nmemb - 1, mid;
    if (!nmemb) return NULL;
    while (low != high) {
        mid = low + (high - low) / 2;
        sample = bytes + mid * size;
        if (compar(key, sample) > 0) low = mid + 1;
        else high = mid;
    }
    return (u8 *) bytes + low * size;
}
static int cmpu16(const void *a, const void *b) { return memcmp(a, b, 2); }
static int cmpu32(const void *a, const void *b) { return memcmp(a, b, 4); }
static inline ul8 getu8(SFT_Font *font, uf32 offset) {
    assert(offset + 1 <= font->size);
    return *(font->memory + offset);
}
static inline sl8 geti8(SFT_Font *font, uf32 offset) { return (sl8) getu8(font, offset); }
static inline ul16 getu16(SFT_Font *font, uf32 offset) {
    assert(offset + 2 <= font->size);
    const u8 *base = font->memory + offset;
    ul16 b1 = base[0], b0 = base[1];
    return (ul16) (b1 << 8 | b0);
}
static inline s16 geti16(SFT_Font *font, uf32 offset) { return (sl16) getu16(font, offset); }
static inline u32 getu32(SFT_Font *font, uf32 offset) {
    assert(offset + 4 <= font->size);
    const u8 *base = font->memory + offset;
    ul32 b3 = base[0], b2 = base[1], b1 = base[2], b0 = base[3];
    return (ul32) (b3 << 24 | b2 << 16 | b1 << 8 | b0);
}
static int gettable(SFT_Font *font, const char tag[4], uf32 *offset) {
    void *match;
    uint numTables;
    numTables = getu16(font, 4);
    if (!is_safe_offset(font, 12, (uf32) numTables * 16)) return -1;
    if (!(match = bsearch(tag, font->memory + 12, numTables, 16, cmpu32))) return -1;
    *offset = getu32(font, (uf32) ((u8 *) match - font->memory + 8));
    return 0;
}
static int cmap_fmt4(SFT_Font *font, uf32 table, SFT_UChar charCode, SFT_Glyph *glyph) {
    const u8 *segPtr;
    uf32 segIdxX2;
    uf32 endCodes, startCodes, idDeltas, idRangeOffsets, idOffset;
    uf16 segCountX2, idRangeOffset, startCode, shortCode, idDelta, id;
    u8 key[2] = { (u8) (charCode >> 8), (u8) charCode };
    if (charCode > 0xFFFF) {
        *glyph = 0;
        return 0;
    }
    shortCode = (uf16) charCode;
    if (!is_safe_offset(font, table, 8)) return -1;
    segCountX2 = getu16(font, table);
    if ((segCountX2 & 1) || !segCountX2) return -1;
    endCodes       = table + 8;
    startCodes     = endCodes + segCountX2 + 2;
    idDeltas       = startCodes + segCountX2;
    idRangeOffsets = idDeltas + segCountX2;
    if (!is_safe_offset(font, idRangeOffsets, segCountX2)) return -1;
    segPtr = (const u8*)csearch(key, font->memory + endCodes, segCountX2 / 2, 2, cmpu16);
    segIdxX2 = (uf32) (segPtr - (font->memory + endCodes));
    if ((startCode = getu16(font, startCodes + segIdxX2)) > shortCode) return 0;
    idDelta = getu16(font, idDeltas + segIdxX2);
    if (!(idRangeOffset = getu16(font, idRangeOffsets + segIdxX2))) {
        *glyph = (shortCode + idDelta) & 0xFFFF;
        return 0;
    }
    idOffset = idRangeOffsets + segIdxX2 + idRangeOffset + 2U * (uint) (shortCode - startCode);
    if (!is_safe_offset(font, idOffset, 2)) return -1;
    id = getu16(font, idOffset);
    *glyph = id ? (id + idDelta) & 0xFFFF : 0;
    return 0;
}
static int cmap_fmt6(SFT_Font *font, uf32 table, SFT_UChar charCode, SFT_Glyph *glyph) {
    uint firstCode, entryCount;
    if (charCode > 0xFFFF) {
        *glyph = 0;
        return 0;
    }
    if (!is_safe_offset(font, table, 4)) return -1;
    firstCode  = getu16(font, table);
    entryCount = getu16(font, table + 2);
    if (!is_safe_offset(font, table, 4 + 2 * entryCount)) return -1;
    if (charCode < firstCode) return -1;
    charCode -= firstCode;
    if (!(charCode < entryCount)) return -1;
    *glyph = getu16(font, table + 4 + 2 * charCode);
    return 0;
}
static int cmap_fmt12_13(SFT_Font *font, uf32 table, SFT_UChar charCode, SFT_Glyph *glyph, int which) {
    u32 len, numEntries;
    uf32 i;
    *glyph = 0;
    if (!is_safe_offset(font, table, 16)) return -1;
    len = getu32(font, table + 4);
    if (len < 16) return -1;
    if (!is_safe_offset(font, table, len)) return -1;
    numEntries = getu32(font, table + 12);
    for (i = 0; i < numEntries; ++i) {
        u32 firstCode, lastCode, glyphOffset;
        firstCode = getu32(font, table + (i * 12) + 16);
        lastCode = getu32(font, table + (i * 12) + 16 + 4);
        if (charCode < firstCode || charCode > lastCode) continue;
        glyphOffset = getu32(font, table + (i * 12) + 16 + 8);
        if (which == 12) *glyph = (charCode-firstCode) + glyphOffset;
        else *glyph = glyphOffset;
        return 0;
    }
    return 0;
}
static int glyph_id(SFT_Font *font, SFT_UChar charCode, SFT_Glyph *glyph) {
    uf32 cmap, entry, table;
    uint idx, numEntries;
    int type, format;
    *glyph = 0;
    if (gettable(font, "cmap", &cmap) < 0) return -1;
    if (!is_safe_offset(font, cmap, 4)) return -1;
    numEntries = getu16(font, cmap + 2);
    if (!is_safe_offset(font, cmap, 4 + numEntries * 8)) return -1;
    for (idx = 0; idx < numEntries; ++idx) {
        entry = cmap + 4 + idx * 8;
        type = getu16(font, entry) * 0100 + getu16(font, entry + 2);
        if (type == 0004 || type == 0312) {
            table = cmap + getu32(font, entry + 4);
            if (!is_safe_offset(font, table, 8)) return -1;
            format = getu16(font, table);
            switch (format) {
                case 12: return cmap_fmt12_13(font, table, charCode, glyph, 12);
                default: return -1;
            }
        }
    }
    for (idx = 0; idx < numEntries; ++idx) {
        entry = cmap + 4 + idx * 8;
        type = getu16(font, entry) * 0100 + getu16(font, entry + 2);
        if (type == 0003 || type == 0301) {
            table = cmap + getu32(font, entry + 4);
            if (!is_safe_offset(font, table, 6)) return -1;
            switch (getu16(font, table)) {
                case 4: return cmap_fmt4(font, table + 6, charCode, glyph);
                case 6: return cmap_fmt6(font, table + 6, charCode, glyph);
                default: return -1;
            }
        }
    }
    return -1;
}
static int hor_metrics(SFT_Font *font, SFT_Glyph glyph, int *advanceWidth, int *leftSideBearing) {
    uf32 hmtx, offset, boundary;
    if (gettable(font, "hmtx", &hmtx) < 0) return -1;
    if (glyph < font->numLongHmtx) {
        offset = hmtx + 4 * glyph;
        if (!is_safe_offset(font, offset, 4)) return -1;
        *advanceWidth = getu16(font, offset);
        *leftSideBearing = geti16(font, offset + 2);
        return 0;
    } else {
        boundary = hmtx + 4U * (uf32) font->numLongHmtx;
        if (boundary < 4) return -1;
        offset = boundary - 4;
        if (!is_safe_offset(font, offset, 4)) return -1;
        *advanceWidth = getu16(font, offset);
        offset = boundary + 2 * (glyph - font->numLongHmtx);
        if (!is_safe_offset(font, offset, 2)) return -1;
        *leftSideBearing = geti16(font, offset);
        return 0;
    }
}
static int glyph_bbox(const SFT *sft, uf32 outline, int box[4]) {
    f64 xScale, yScale;
    if (!is_safe_offset(sft->font, outline, 10)) return -1;
    box[0] = geti16(sft->font, outline + 2);
    box[1] = geti16(sft->font, outline + 4);
    box[2] = geti16(sft->font, outline + 6);
    box[3] = geti16(sft->font, outline + 8);
    if (box[2] <= box[0] || box[3] <= box[1]) return -1;
    xScale = sft->xScale / sft->font->unitsPerEm;
    yScale = sft->yScale / sft->font->unitsPerEm;
    box[0] = (int) floor(box[0] * xScale + sft->xOffset);
    box[1] = (int) floor(box[1] * yScale + sft->yOffset);
    box[2] = (int) ceil (box[2] * xScale + sft->xOffset);
    box[3] = (int) ceil (box[3] * yScale + sft->yOffset);
    return 0;
}
static int outline_offset(SFT_Font *font, SFT_Glyph glyph, uf32 *offset) {
    uf32 loca, glyf;
    uf32 base, offset_this, offset_next;
    if (gettable(font, "loca", &loca) < 0) return -1;
    if (gettable(font, "glyf", &glyf) < 0) return -1;
    if (font->locaFormat == 0) {
        base = loca + 2 * glyph;
        if (!is_safe_offset(font, base, 4)) return -1;
        offset_this = 2U * (uf32) getu16(font, base);
        offset_next = 2U * (uf32) getu16(font, base + 2);
    } else {
        base = loca + 4 * glyph;
        if (!is_safe_offset(font, base, 8)) return -1;
        offset_this = getu32(font, base);
        offset_next = getu32(font, base + 4);
    }
    *offset = offset_this == offset_next ? 0 : glyf + offset_this;
    return 0;
}
static int simple_flags(SFT_Font *font, uf32 *offset, uf16 numPts, u8 *flags) {
    uf32 off = *offset;
    uf16 i;
    u8 value = 0, repeat = 0;
    for (i = 0; i < numPts; ++i) {
        if (repeat) --repeat;
        else {
            if (!is_safe_offset(font, off, 1)) return -1;
            value = getu8(font, off++);
            if (value & REPEAT_FLAG) {
                if (!is_safe_offset(font, off, 1)) return -1;
                repeat = getu8(font, off++);
            }
        }
        flags[i] = value;
    }
    *offset = off;
    return 0;
}
static int simple_points(SFT_Font *font, uf32 offset, uf16 numPts, u8 *flags, Point *points) {
    long accum, value, bit;
    uf16 i;
    accum = 0L;
    for (i = 0; i < numPts; ++i) {
        if (flags[i] & X_CHANGE_IS_SMALL) {
            if (!is_safe_offset(font, offset, 1)) return -1;
            value = (long) getu8(font, offset++);
            bit = !!(flags[i] & X_CHANGE_IS_POSITIVE);
            accum -= (value ^ -bit) + bit;
        } else if (!(flags[i] & X_CHANGE_IS_ZERO)) {
            if (!is_safe_offset(font, offset, 2)) return -1;
            accum += geti16(font, offset);
            offset += 2;
        }
        points[i].x = (f64) accum;
    }
    accum = 0L;
    for (i = 0; i < numPts; ++i) {
        if (flags[i] & Y_CHANGE_IS_SMALL) {
            if (!is_safe_offset(font, offset, 1)) return -1;
            value = (long) getu8(font, offset++);
            bit = !!(flags[i] & Y_CHANGE_IS_POSITIVE);
            accum -= (value ^ -bit) + bit;
        } else if (!(flags[i] & Y_CHANGE_IS_ZERO)) {
            if (!is_safe_offset(font, offset, 2)) return -1;
            accum += geti16(font, offset);
            offset += 2;
        }
        points[i].y = (f64) accum;
    }
    return 0;
}
static int decode_contour(u8 *flags, uf16 basePoint, uf16 count, Outline *outl) {
    uf16 i;
    ul16 looseEnd, beg, ctrl, center, cur;
    uint gotCtrl;
    if (count < 2) return 0;
    assert(basePoint <= UINT16_MAX - count);
    if (flags[0] & POINT_IS_ON_CURVE) {
        looseEnd = (ul16) basePoint++;
        ++flags;
        --count;
    } else if (flags[count - 1] & POINT_IS_ON_CURVE) looseEnd = (ul16) (basePoint + --count);
    else {
        if (outl->numPoints >= outl->capPoints && grow_points(outl) < 0) return -1;
        looseEnd = outl->numPoints;
        outl->points[outl->numPoints++] = midpoint(outl->points[basePoint], outl->points[basePoint + count - 1]);
    }
    beg = looseEnd;
    gotCtrl = 0;
    for (i = 0; i < count; ++i) {
        cur = (ul16) (basePoint + i);
        if (flags[i] & POINT_IS_ON_CURVE) {
            if (gotCtrl) {
                if (outl->numCurves >= outl->capCurves && grow_curves(outl) < 0) return -1;
                outl->curves[outl->numCurves++] = (Curve) { beg, cur, ctrl };
            } else {
                if (outl->numLines >= outl->capLines && grow_lines(outl) < 0) return -1;
                outl->lines[outl->numLines++] = (Line) { beg, cur };
            }
            beg = cur;
            gotCtrl = 0;
        } else {
            if (gotCtrl) {
                center = outl->numPoints;
                if (outl->numPoints >= outl->capPoints && grow_points(outl) < 0) return -1;
                outl->points[center] = midpoint(outl->points[ctrl], outl->points[cur]);
                ++outl->numPoints;
                if (outl->numCurves >= outl->capCurves && grow_curves(outl) < 0) return -1;
                outl->curves[outl->numCurves++] = (Curve) { beg, center, ctrl };
                beg = center;
            }
            ctrl = cur;
            gotCtrl = 1;
        }
    }
    if (gotCtrl) {
        if (outl->numCurves >= outl->capCurves && grow_curves(outl) < 0) return -1;
        outl->curves[outl->numCurves++] = (Curve) { beg, looseEnd, ctrl };
    } else {
        if (outl->numLines >= outl->capLines && grow_lines(outl) < 0) return -1;
        outl->lines[outl->numLines++] = (Line) { beg, looseEnd };
    }
    return 0;
}
static int simple_outline(SFT_Font *font, uf32 offset, uint numContours, Outline *outl) {
    uf16 *endPts = NULL;
    u8 *flags = NULL;
    uf16 numPts;
    uf16 beg;
    uint i;
    assert(numContours > 0);
    uf16 basePoint = outl->numPoints;
    if (!is_safe_offset(font, offset, numContours * 2 + 2)) goto failure2;
    numPts = getu16(font, offset + (numContours - 1) * 2);
    if (numPts >= UINT16_MAX) goto failure2;
    numPts++;
    if (outl->numPoints > UINT16_MAX - numPts) goto failure2;
    while (outl->capPoints < basePoint + numPts) if (grow_points(outl) < 0) goto failure2;
    STACK_ALLOC(endPts, uf16, 16, numContours);
    if (endPts == NULL) goto failure2;
    STACK_ALLOC(flags, u8, 128, numPts);
    if (flags == NULL) goto failure2;
    for (i = 0; i < numContours; ++i) {
        endPts[i] = getu16(font, offset);
        offset += 2;
    }
    for (i = 0; i < numContours - 1; ++i) if (endPts[i + 1] < endPts[i] + 1) goto failure2;
    offset += 2U + getu16(font, offset);
    if (simple_flags(font, &offset, numPts, flags) < 0) goto failure2;
    if (simple_points(font, offset, numPts, flags, outl->points + basePoint) < 0) goto failure2;
    outl->numPoints = (ul16) (outl->numPoints + numPts);
    beg = 0;
    for (i = 0; i < numContours; ++i) {
        uf16 count = endPts[i] - beg + 1;
        if (decode_contour(flags + beg, basePoint + beg, count, outl) < 0) goto failure2;
        beg = endPts[i] + 1;
    }
    STACK_FREE(endPts);
    STACK_FREE(flags);
    return 0;
    failure2:
        STACK_FREE(endPts);
        STACK_FREE(flags);
        return -1;
}
static int compound_outline(SFT_Font *font, uf32 offset, int recDepth, Outline *outl) {
    f64 local[6];
    uf32 outline;
    uint flags, glyph, basePoint;
    if (recDepth >= 4) return -1;
    do {
        memset(local, 0, sizeof local);
        if (!is_safe_offset(font, offset, 4)) return -1;
        flags = getu16(font, offset);
        glyph = getu16(font, offset + 2);
        offset += 4;
        if (!(flags & ACTUAL_XY_OFFSETS)) return -1;
        if (flags & OFFSETS_ARE_LARGE) {
            if (!is_safe_offset(font, offset, 4)) return -1;
            local[4] = geti16(font, offset);
            local[5] = geti16(font, offset + 2);
            offset += 4;
        } else {
            if (!is_safe_offset(font, offset, 2)) return -1;
            local[4] = geti8(font, offset);
            local[5] = geti8(font, offset + 1);
            offset += 2;
        }
        if (flags & GOT_A_SINGLE_SCALE) {
            if (!is_safe_offset(font, offset, 2)) return -1;
            local[0] = geti16(font, offset) / 16384.0;
            local[3] = local[0];
            offset += 2;
        } else if (flags & GOT_AN_X_AND_Y_SCALE) {
            if (!is_safe_offset(font, offset, 4)) return -1;
            local[0] = geti16(font, offset + 0) / 16384.0;
            local[3] = geti16(font, offset + 2) / 16384.0;
            offset += 4;
        } else if (flags & GOT_A_SCALE_MATRIX) {
            if (!is_safe_offset(font, offset, 8)) return -1;
            local[0] = geti16(font, offset + 0) / 16384.0;
            local[1] = geti16(font, offset + 2) / 16384.0;
            local[2] = geti16(font, offset + 4) / 16384.0;
            local[3] = geti16(font, offset + 6) / 16384.0;
            offset += 8;
        } else {
            local[0] = 1.0;
            local[3] = 1.0;
        }
        if (outline_offset(font, glyph, &outline) < 0) return -1;
        if (outline) {
            basePoint = outl->numPoints;
            if (decode_outline(font, outline, recDepth + 1, outl) < 0) return -1;
            transform_points(outl->numPoints - basePoint, outl->points + basePoint, local);
        }
    } while (flags & THERE_ARE_MORE_COMPONENTS);
    return 0;
}
static int decode_outline(SFT_Font *font, uf32 offset, int recDepth, Outline *outl) {
    int numContours;
    if (!is_safe_offset(font, offset, 10)) return -1;
    numContours = geti16(font, offset);
    if (numContours > 0) return simple_outline(font, offset + 10, (uint) numContours, outl);
    else if (numContours < 0) return compound_outline(font, offset + 10, recDepth, outl);
    else return 0;
}
static int is_flat(Outline *outl, Curve curve) {
    const f64 maxArea2 = 2.0;
    Point a = outl->points[curve.beg];
    Point b = outl->points[curve.ctrl];
    Point c = outl->points[curve.end];
    Point g = { b.x-a.x, b.y-a.y };
    Point h = { c.x-a.x, c.y-a.y };
    f64 area2 = fabs(g.x*h.y-h.x*g.y);
    return area2 <= maxArea2;
}
static int tesselate_curve(Curve curve, Outline *outl) {
#define STACK_SIZE 10
    Curve stack[STACK_SIZE];
    uint top = 0;
    for (;;) {
        if (is_flat(outl, curve) || top >= STACK_SIZE) {
            if (outl->numLines >= outl->capLines && grow_lines(outl) < 0)
                return -1;
            outl->lines[outl->numLines++] = (Line) { curve.beg, curve.end };
            if (top == 0) break;
            curve = stack[--top];
        } else {
            ul16 ctrl0 = outl->numPoints;
            if (outl->numPoints >= outl->capPoints && grow_points(outl) < 0) return -1;
            outl->points[ctrl0] = midpoint(outl->points[curve.beg], outl->points[curve.ctrl]);
            ++outl->numPoints;
            ul16 ctrl1 = outl->numPoints;
            if (outl->numPoints >= outl->capPoints && grow_points(outl) < 0) return -1;
            outl->points[ctrl1] = midpoint(outl->points[curve.ctrl], outl->points[curve.end]);
            ++outl->numPoints;
            ul16 pivot = outl->numPoints;
            if (outl->numPoints >= outl->capPoints && grow_points(outl) < 0) return -1;
            outl->points[pivot] = midpoint(outl->points[ctrl0], outl->points[ctrl1]);
            ++outl->numPoints;
            stack[top++] = (Curve) { curve.beg, pivot, ctrl0 };
            curve = (Curve) { pivot, curve.end, ctrl1 };
        }
    }
    return 0;
#undef STACK_SIZE
}
static int tesselate_curves(Outline *outl) {
    uint i;
    for (i = 0; i < outl->numCurves; ++i) if (tesselate_curve(outl->curves[i], outl) < 0) return -1;
    return 0;
}
static void draw_line(Raster buf, Point origin, Point goal) {
    Point delta;
    Point nextCrossing;
    Point crossingIncr;
    f64 halfDeltaX;
    f64 prevDistance = 0.0, nextDistance;
    f64 xAverage, yDifference;
    struct { int x, y; } pixel;
    struct { int x, y; } dir;
    int step, numSteps = 0;
    Cell *__restrict cptr, cell;
    delta.x = goal.x - origin.x;
    delta.y = goal.y - origin.y;
    dir.x = SIGN(delta.x);
    dir.y = SIGN(delta.y);
    if (!dir.y) return;
    crossingIncr.x = dir.x ? fabs(1.0 / delta.x) : 1.0;
    crossingIncr.y = fabs(1.0 / delta.y);
    if (!dir.x) {
        pixel.x = fast_floor(origin.x);
        nextCrossing.x = 100.0;
    } else {
        if (dir.x > 0) {
            pixel.x = fast_floor(origin.x);
            nextCrossing.x = (origin.x - pixel.x) * crossingIncr.x;
            nextCrossing.x = crossingIncr.x - nextCrossing.x;
            numSteps += fast_ceil(goal.x) - fast_floor(origin.x) - 1;
        } else {
            pixel.x = fast_ceil(origin.x) - 1;
            nextCrossing.x = (origin.x - pixel.x) * crossingIncr.x;
            numSteps += fast_ceil(origin.x) - fast_floor(goal.x) - 1;
        }
    }
    if (dir.y > 0) {
        pixel.y = fast_floor(origin.y);
        nextCrossing.y = (origin.y - pixel.y) * crossingIncr.y;
        nextCrossing.y = crossingIncr.y - nextCrossing.y;
        numSteps += fast_ceil(goal.y) - fast_floor(origin.y) - 1;
    } else {
        pixel.y = fast_ceil(origin.y) - 1;
        nextCrossing.y = (origin.y - pixel.y) * crossingIncr.y;
        numSteps += fast_ceil(origin.y) - fast_floor(goal.y) - 1;
    }
    nextDistance = MIN(nextCrossing.x, nextCrossing.y);
    halfDeltaX = 0.5 * delta.x;
    for (step = 0; step < numSteps; ++step) {
        xAverage = origin.x + (prevDistance + nextDistance) * halfDeltaX;
        yDifference = (nextDistance - prevDistance) * delta.y;
        cptr = &buf.cells[pixel.y * buf.width + pixel.x];
        cell = *cptr;
        cell.cover += yDifference;
        xAverage -= (f64) pixel.x;
        cell.area += (1.0 - xAverage) * yDifference;
        *cptr = cell;
        prevDistance = nextDistance;
        int alongX = nextCrossing.x < nextCrossing.y;
        pixel.x += alongX ? dir.x : 0;
        pixel.y += alongX ? 0 : dir.y;
        nextCrossing.x += alongX ? crossingIncr.x : 0.0;
        nextCrossing.y += alongX ? 0.0 : crossingIncr.y;
        nextDistance = MIN(nextCrossing.x, nextCrossing.y);
    }
    xAverage = origin.x + (prevDistance + 1.0) * halfDeltaX;
    yDifference = (1.0 - prevDistance) * delta.y;
    cptr = &buf.cells[pixel.y * buf.width + pixel.x];
    cell = *cptr;
    cell.cover += yDifference;
    xAverage -= (f64) pixel.x;
    cell.area += (1.0 - xAverage) * yDifference;
    *cptr = cell;
}
static void draw_lines(Outline *outl, Raster buf) {
    uint i;
    for (i = 0; i < outl->numLines; ++i) {
        Line  line   = outl->lines[i];
        Point origin = outl->points[line.beg];
        Point goal   = outl->points[line.end];
        draw_line(buf, origin, goal);
    }
}
static void post_process(Raster buf, u8 *image) {
    Cell cell;
    f64 accum = 0.0, value;
    uint i, num;
    num = (uint) buf.width * (uint) buf.height;
    for (i = 0; i < num; ++i) {
        cell     = buf.cells[i];
        value    = fabs(accum + cell.area);
        value    = MIN(value, 1.0);
        value    = value * 255.0 + 0.5;
        image[i] = (u8) value;
        accum   += cell.cover;
    }
}
static int render_outline(Outline *outl, f64 transform[6], SFT_Image image) {
    Cell *cells = NULL;
    Raster buf;
    uint numPixels;
    numPixels = (uint) image.width * (uint) image.height;
    STACK_ALLOC(cells, Cell, 128 * 128, numPixels);
    if (!cells) return -1;
    memset(cells, 0, numPixels * sizeof *cells);
    buf.cells  = cells;
    buf.width  = image.width;
    buf.height = image.height;
    transform_points(outl->numPoints, outl->points, transform);
    clip_points(outl->numPoints, outl->points, image.width, image.height);
    if (tesselate_curves(outl) < 0) {
        STACK_FREE(cells);
        return -1;
    }
    draw_lines(outl, buf);
    post_process(buf, (u8*)image.pixels);
    STACK_FREE(cells);
    return 0;
}
#endif
#ifdef __cplusplus
}
#endif