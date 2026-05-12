#include <string>
#include "eg2/libs/typedefs.h"
#include "eg2/gx2/textures.hh"
#include "eg2/tex/textures.hh"
#include "eg2/heap/mem2.hh"
#include "eg2/tex/raw.hh"
#include "eg2/render/scan.hh"
#include "eg2/external/textures.hh"
#include "eg2/external/core.hh"

namespace Tex {
    GX2Texture* LatteTex::LoadTextureFromPath(const char* path) {
        u32 size = 0;
        u8* buffer = (u8*)IO::ReadFile(path, FILEMODE_BIN, &size);

        if (!buffer || size == 0) return nullptr;

        std::string p(path);
        std::string ext;

        if (p.find_last_of('.') != std::string::npos) {
            ext = p.substr(p.find_last_of('.') + 1);
            for (auto &c : ext) c = (char)std::tolower((u8)c);
        }

        GX2Texture* tex = nullptr;

        if (ext == "png") tex = IO::PNGRead(buffer, size);
        else if (ext == "jpg" || ext == "jpeg") tex = IO::JPGRead(buffer, size);
        else if (ext == "gif") tex = IO::GIFRead(buffer, size, 0);
        else tex = IO::PNGRead(buffer, size);

        Heap::MEM2::Free(buffer);
        return tex;
    }

    LatteTex::LatteTex(const char* path) { this->texture = LoadTextureFromPath(path); }

    LatteTex::~LatteTex(void) {
        if (this->texture) {
            GX2::DestroyTexture(&this->texture);
            this->texture = nullptr;
        }
    }

    void LatteTex::ChangeTexture(const char* path) {
        GX2Texture* newTex = LoadTextureFromPath(path);
        if (!newTex) return;

        if (this->texture) GX2::DestroyTexture(&this->texture);

        this->texture = newTex;
    }

    void LatteTex::SetRect(f32 x, f32 y, f32 w, f32 h) { this->rect = { x, y, w, h }; }
    Rect LatteTex::GetRect(void) { return this->rect; }

    void LatteTex::RenderPartial(f32 x, f32 y, f32 w, f32 h, int px, int py, int pw, int ph) { RenderPartialColor(x, y, w, h, px, py, pw, ph, 1.f, 1.f, 1.f, 1.f); }
    void LatteTex::RenderExact(f32 x, f32 y, f32 w, f32 h, int realWidth, int realHeight) { RenderPartial(x, y, w, h, 0, 0, realWidth, realHeight); }

    void LatteTex::RenderPartialColor(f32 x, f32 y, f32 w, f32 h, int partialX, int partialY, int partialW, int partialH, f32 r, f32 g, f32 b, f32 a) { 
        if (!this->texture) return;
        Tex::RawPartialColor(this->texture, x, y, w, h, partialX, partialY, partialW, partialH, r, g, b, a);
    }

    void LatteTex::RenderColor(f32 x, f32 y, f32 w, f32 h, f32 r, f32 g, f32 b, f32 a) {
        if (!this->texture) return;
        RenderPartialColor(x, y, w, h, 0, 0, this->texture->surface.width, this->texture->surface.height, r, g, b, a); 
    }

    void LatteTex::RenderTex(f32 x, f32 y, f32 w, f32 h) {
        if (!this->texture) return;
        RenderPartial(x, y, w, h, 0, 0, this->texture->surface.width, this->texture->surface.height); 
    }

    void LatteTex::RenderTexScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 x, f32 y) {
        if (!this->texture) return;

        f32 width_scaled  = ref_width  * (Render::gTargetWidth  / base_width);
        f32 height_scaled = ref_height * (Render::gTargetHeight / base_height);
        f32 x_scaled = x * (Render::gTargetWidth / base_width);
        f32 y_scaled = y * (Render::gTargetHeight / base_height);

        RenderTex(x_scaled, y_scaled, width_scaled, height_scaled);
    }

    void LatteTex::RenderTexScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 x, f32 y) {
        if (!this->texture) return;

        const AspectRatio& ar = gAspectRatios[aspect];
        RenderTexScaled(ar.width, ar.height, ref_width, ref_height, x, y);
    }

    void LatteTex::RenderTexCentered(f32 offsetX, f32 offsetY, f32 ref_height, f32 ref_width) {
        if (!this->texture) return;

        f32 width_scaled  = ref_width;
        f32 height_scaled = ref_height;

        f32 x = (Render::gTargetWidth  - width_scaled)  * 0.5f + offsetX;
        f32 y = (Render::gTargetHeight - height_scaled) * 0.5f + offsetY;

        RenderTex(x, y, width_scaled, height_scaled);
        SetRect(x, y, width_scaled, height_scaled);
    }

    void LatteTex::RenderTexCenteredScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY) {
        if (!this->texture) return;

        f32 sx = Render::gTargetWidth  / base_width;
        f32 sy = Render::gTargetHeight / base_height;

        f32 width_scaled  = ref_width  * sx;
        f32 height_scaled = ref_height * sy;

        f32 x = (Render::gTargetWidth  - width_scaled)  * 0.5f + (offsetX * sx);
        f32 y = (Render::gTargetHeight - height_scaled) * 0.5f + (offsetY * sy);

        RenderTex(x, y, width_scaled, height_scaled);
        SetRect(x, y, width_scaled, height_scaled);
    }

    void LatteTex::RenderTexCenteredScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY) {
        if (!this->texture) return;

        const AspectRatio& ar = gAspectRatios[aspect];
        RenderTexCenteredScaled(ar.width, ar.height, ref_width, ref_height, offsetX, offsetY);
    }

    void LatteTex::RenderBackgroundRepeat(void) {
        if (!this->texture) return;
        Tex::BackgroundRepeat(this->texture);
    }

    void LatteTex::RenderBackgroundRepeatScaled(f32 base_width, f32 base_height) {
        if (!this->texture) return;

        f32 sx = Render::gTargetWidth  / base_width;
        f32 sy = Render::gTargetHeight / base_height;

        Tex::BackgroundRepeatScaled(this->texture, sx, sy);
    }

    void LatteTex::RenderBackgroundRepeatScaled(RenderingAspectType aspect) {
        if (!this->texture) return;

        const AspectRatio& ar = gAspectRatios[aspect];
        RenderBackgroundRepeatScaled(ar.width, ar.height);
    }
}