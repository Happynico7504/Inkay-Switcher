#pragma once

#include <gx2/texture.h>
#include "eg2/libs/typedefs.h"
#include "eg2/tex/raw.hh"

namespace Tex {
    class LatteTex {
        public:
            LatteTex(const char* path);
            ~LatteTex(void);

            void ChangeTexture(const char* path);

            void SetRect(f32 x, f32 y, f32 w, f32 h);
            Rect GetRect(void);

            void RenderTex(f32 x, f32 y, f32 w, f32 h);

            void RenderTexScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 x, f32 y);
            void RenderTexScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 x, f32 y);

            void RenderTexCentered(f32 offsetX, f32 offsetY, f32 ref_height, f32 ref_width);

            void RenderTexCenteredScaled(f32 base_width, f32 base_height, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY);
            void RenderTexCenteredScaled(RenderingAspectType aspect, f32 ref_width, f32 ref_height, f32 offsetX, f32 offsetY);

            void RenderBackgroundRepeat(void);

            void RenderBackgroundRepeatScaled(f32 base_width, f32 base_height);
            void RenderBackgroundRepeatScaled(RenderingAspectType aspect);
        private:
            GX2Texture* LoadTextureFromPath(const char* path);
            void RenderPartialColor(f32 x, f32 y, f32 width, f32 height, int partialX, int partialY, int partialW, int partialH, f32 r, f32 g, f32 b, f32 a);
            void RenderPartial(f32 x, f32 y, f32 w, f32 h, int px, int py, int pw, int ph);
            void RenderColor(f32 x, f32 y, f32 w, f32 h, f32 r, f32 g, f32 b, f32 a);
            void RenderExact(f32 x, f32 y, f32 w, f32 h, int realWidth, int realHeight);
            
            GX2Texture* texture = nullptr;
            Rect rect{};
    };
}