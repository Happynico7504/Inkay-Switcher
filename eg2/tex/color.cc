#include <string>
#include "eg2/libs/typedefs.h"
#include "eg2/tex/color.hh"
#include "eg2/tex/raw.hh"
#include "eg2/render/scan.hh"

namespace Tex {
    LatteColor::LatteColor(u32 hexColor) { this->color = Tex::U322F32(hexColor); }
    LatteColor::LatteColor(Color color) { this->color = Tex::U82F32(color); }
    LatteColor::LatteColor(F32Color color) { this->color = color; }

    void LatteColor::SetColor(u32 hex) { this->color = Tex::U322F32(hex); }
    void LatteColor::SetColor(const Color c) { this->color = Tex::U82F32(c); }
    void LatteColor::SetColor(const F32Color c) { this->color = c; }

    void LatteColor::SetRect(f32 x, f32 y, f32 w, f32 h) { this->rect = { x, y, w, h }; }
    Rect LatteColor::GetRect(void) { return this->rect; }

    void LatteColor::DrawSolid(f32 x, f32 y, f32 w, f32 h) { Tex::RawQuadColor(x, y, w, h, this->color.r, this->color.g, this->color.b, this->color.a); }

    void LatteColor::DrawSolidScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 x, f32 y) {
        f32 sx = Render::gTargetWidth  / base_width;
        f32 sy = Render::gTargetHeight / base_height;

        f32 width_scaled  = ref_width  * sx;
        f32 height_scaled = ref_height * sy;
        f32 x_scaled = x * sx;
        f32 y_scaled = y * sy;

        DrawSolid(x_scaled, y_scaled, width_scaled, height_scaled);
    }

    void LatteColor::DrawSolidScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 x, f32 y) {
        const AspectRatio& ar = gAspectRatios[aspect];
        DrawSolidScaled(ar.width, ar.height, ref_width, ref_height, x, y);
    }

    void LatteColor::DrawSolidCentered(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY) {
        f32 sx = Render::gTargetWidth  / base_width;
        f32 sy = Render::gTargetHeight / base_height;

        f32 width_scaled  = ref_width;
        f32 height_scaled = ref_height;

        f32 x = (Render::gTargetWidth  - width_scaled)  * 0.5f + (offsetX * sx);
        f32 y = (Render::gTargetHeight - height_scaled) * 0.5f + (offsetY * sy);

        DrawSolid(x, y, width_scaled, height_scaled);
        SetRect(x, y, width_scaled, height_scaled);
    }

    void LatteColor::DrawSolidCenteredScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY) {
        f32 sx = Render::gTargetWidth  / base_width;
        f32 sy = Render::gTargetHeight / base_height;

        f32 width_scaled  = ref_width  * sx;
        f32 height_scaled = ref_height * sy;

        f32 x = (Render::gTargetWidth  - width_scaled)  * 0.5f + (offsetX * sx);
        f32 y = (Render::gTargetHeight - height_scaled) * 0.5f + (offsetY * sy);

        DrawSolid(x, y, width_scaled, height_scaled);
        SetRect(x, y, width_scaled, height_scaled);
    }

    void LatteColor::DrawSolidCenteredScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY) {
        const AspectRatio& ar = gAspectRatios[aspect];
        DrawSolidCenteredScaled(ar.width, ar.height, ref_width, ref_height, offsetX, offsetY);
    }
}