#pragma once

#include "eg2/libs/typedefs.h"
#include "eg2/tex/raw.hh"
#include "eg2/render/frames.hh"

namespace Tex {
    class LatteColor {
        public:
            LatteColor(u32 hexColor);
            LatteColor(Color color);
            LatteColor(F32Color color);
            ~LatteColor(void) = default;

            void SetColor(u32 hex);
            void SetColor(const Color c);
            void SetColor(const F32Color c);

            void SetRect(f32 x, f32 y, f32 w, f32 h);
            Rect GetRect(void);

            void DrawSolid(f32 x, f32 y, f32 w, f32 h);
            void DrawSolidScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 x, f32 y);
            void DrawSolidScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 x, f32 y);
            void DrawSolidCentered(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY);
            void DrawSolidCenteredScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY);
            void DrawSolidCenteredScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY);
        private:
            F32Color color;
            Rect rect{};
    };
}