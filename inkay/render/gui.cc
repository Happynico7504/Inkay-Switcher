#include "eg2/libs/typedefs.h"
#include "eg2/tex/textures.hh"
#include "eg2/tex/font.hh"
#include "eg2/tex/color.hh"
#include "eg2/tex/raw.hh"
#include "eg2/gx2/textures.hh"
#include "eg2/render/screen.hh"
#include "eg2/controller/drc.hh"
#include "eg2/external/core.hh"
#include "inkay/render/gui.hh"
#include "inkay/repos/core.hh"
#include "inkay/repos/versions.hh"
#include "inkay/repos/download.hh"
#include "inkay/fs/dirs.hh"
#include <memory>

namespace Inkay {
    namespace GUI {
        static std::unique_ptr<Tex::LatteFont> OSCAFESTDFont;
        static std::unique_ptr<Tex::LatteColor> ColorChangingBars;
        static std::unique_ptr<Tex::LatteColor> ButtonsHolder;
        static std::unique_ptr<Tex::LatteTex> PortalBG;

        static bool AlreadyAppliedGUIChange = false;

        static const char* HBASManifest = "fs:/vol/external01/wiiu/apps/appstore/.get/packages/Inkay/manifest.install";

        // Scrollable source-list state (DOWNLOADSTATE_SELECT).
        static usize gSourceCursor = 0;
        static usize gSourceScrollTop = 0;
        static const usize kVisibleSourceRows = 4;
        static const f32 kSourceRowOffsets[kVisibleSourceRows] = { -105.0f, -35.0f, 35.0f, 105.0f };

        void Init(void) {
            OSCAFESTDFont = std::make_unique<Tex::LatteFont>(FONT_CAFESTD, 24.0f);
            OSCAFESTDFont->SwitchRenderType(FONT_ALIGN_CENTER);
            ColorChangingBars = std::make_unique<Tex::LatteColor>(Colors::Default);
            ButtonsHolder = std::make_unique<Tex::LatteColor>(Colors::Holder);
            PortalBG = std::make_unique<Tex::LatteTex>("fs:/vol/content/bakkuguraundo/bara.png");
        }

        void Render(void) {
            if (PortalBG) PortalBG->RenderBackgroundRepeatScaled(RENDERRATIO_ASPECT_1920x1080);

            if (ColorChangingBars) {
                ColorChangingBars->DrawSolidCenteredScaled(RENDERRATIO_ASPECT_854x480, 1000.0f, 55.0f, 0.0f, -214.0f);
                ColorChangingBars->DrawSolidCenteredScaled(RENDERRATIO_ASPECT_854x480, 1000.0f, 55.0f, 0.0f, 214.0f);
            }

            switch (Inkay::Download::State) {
                case DOWNLOADSTATE_SELECT: {
                    usize total = Inkay::Repos::Sources.size();

                    if (!Inkay::Repos::SourcesLoaded || total == 0) {
                        if (OSCAFESTDFont) {
                            OSCAFESTDFont->SetColor(Colors::Black);
                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, 0.0f,
                                RENDERRATIO_ASPECT_854x480, 500.0f, 90.0f,
                                "Failed to load the version list.\nCheck your internet connection."
                            );
                        }
                        break;
                    }

                    usize visibleCount = (total < kVisibleSourceRows) ? total : kVisibleSourceRows;

                    if (ButtonsHolder) {
                        for (usize i = 0; i < visibleCount; i++) {
                            usize idx = gSourceScrollTop + i;
                            bool focused = (idx == gSourceCursor);
                            ButtonsHolder->SetColor(focused ? 0xFFFFFFFFu : (u32)Colors::Holder);
                            ButtonsHolder->DrawSolidCenteredScaled(RENDERRATIO_ASPECT_854x480, 350.0f, 70.0f, 0.0f, kSourceRowOffsets[i]);
                        }
                    }

                    if (OSCAFESTDFont) {
                        OSCAFESTDFont->SetColor(Colors::Black);
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, -175.0f,
                            RENDERRATIO_ASPECT_854x480, 370.0f, 60.0f,
                            "Select an Inkay version to install"
                        );

                        for (usize i = 0; i < visibleCount; i++) {
                            usize idx = gSourceScrollTop + i;
                            const Inkay::Repos::SourceInfo& src = Inkay::Repos::Sources[idx];

                            OSCAFESTDFont->SetColor(src.color);
                            OSCAFESTDFont->RenderTextCenteredScaled(
                                15.0f, kSourceRowOffsets[i] - 5.0f,
                                RENDERRATIO_ASPECT_854x480, 170.0f, 80.0f,
                                src.name.c_str()
                            );
                        }

                        // Scroll affordance, only needed once the list overflows the 4 visible rows.
                        // NOTE: coordinates derived from the row layout below (see IsTouchInsideSingle
                        // calls in Update()) rather than measured on hardware/Cemu - nudge if misaligned.
                        if (total > kVisibleSourceRows) {
                            OSCAFESTDFont->SetColor(gSourceScrollTop > 0 ? Colors::Black : Colors::Holder);
                            OSCAFESTDFont->RenderTextCenteredScaled(
                                250.0f, -50.0f,
                                RENDERRATIO_ASPECT_854x480, 60.0f, 40.0f,
                                "^"
                            );

                            OSCAFESTDFont->SetColor((gSourceScrollTop + visibleCount < total) ? Colors::Black : Colors::Holder);
                            OSCAFESTDFont->RenderTextCenteredScaled(
                                250.0f, 50.0f,
                                RENDERRATIO_ASPECT_854x480, 60.0f, 40.0f,
                                "v"
                            );

                            char counter[64];
                            snprintf(counter, sizeof(counter), "%d / %d", (int)(gSourceCursor + 1), (int)total);

                            OSCAFESTDFont->SetColor(Colors::Black);
                            OSCAFESTDFont->RenderTextCenteredScaled(
                                250.0f, 0.0f,
                                RENDERRATIO_ASPECT_854x480, 80.0f, 30.0f,
                                counter
                            );
                        }

                        OSCAFESTDFont->SetColor(Colors::Browser);
                        OSCAFESTDFont->RenderTextScaled(
                            RENDERRATIO_ASPECT_854x480, 150.0f, 70.0f,
                            "Help/More Info",
                            690.0f, 435.0f
                        );
                    }

                    break;
                }

                case DOWNLOADSTATE_HBAS_WARNING: {
                    if (OSCAFESTDFont && ButtonsHolder) {
                        ButtonsHolder->DrawSolidCenteredScaled(RENDERRATIO_ASPECT_854x480, 600.0f, 160.0f, 0.0f, 0.0f);

                        OSCAFESTDFont->SetColor(Colors::Black);
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, -60.0f,
                            RENDERRATIO_ASPECT_854x480, 560.0f, 70.0f,
                            "HBAS Inkay detected!"
                        );
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, 10.0f,
                            RENDERRATIO_ASPECT_854x480, 560.0f, 60.0f,
                            "Please uninstall Inkay from the"
                        );
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, 60.0f,
                            RENDERRATIO_ASPECT_854x480, 560.0f, 60.0f,
                            "Homebrew App Store first."
                        );
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, 130.0f,
                            RENDERRATIO_ASPECT_854x480, 200.0f, 50.0f,
                            "B - Go Back"
                        );
                    }

                    break;
                }

                case DOWNLOADSTATE_SELECT_ENV: {
                    usize envCount = Inkay::Dirs::gEnvironments.size();

                    if (envCount > 1) {
                        if (OSCAFESTDFont && ButtonsHolder) {
                            ButtonsHolder->DrawSolidCenteredScaled(
                                RENDERRATIO_ASPECT_854x480, 500.0f, 180.0f,
                                0.0f, 0.0f
                            );

                            OSCAFESTDFont->SetColor(Colors::Black);
                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, -140.0f,
                                RENDERRATIO_ASPECT_854x480, 450.0f, 70.0f,
                                "Select installation environment"
                            );

                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, 205.0f,
                                RENDERRATIO_ASPECT_854x480, 150.0f, 60.0f,
                                "A - Confirm"
                            );

                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, -80.0f,
                                RENDERRATIO_ASPECT_854x480, 90.0f, 50.0f,
                                "^"
                            );

                            usize selected = Inkay::Dirs::gSelectedEnv;

                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, -20.0f,
                                RENDERRATIO_ASPECT_854x480, 300.0f, 50.0f,
                                Inkay::Dirs::gEnvironments[selected].c_str()
                            );

                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, 50.0f,
                                RENDERRATIO_ASPECT_854x480, 60.0f, 40.0f,
                                "v"
                            );

                            char counter[64];
                            snprintf(counter, sizeof(counter), "%d / %d", (int)(selected + 1), (int)envCount);

                            OSCAFESTDFont->RenderTextCenteredScaled(
                                0.0f, 130.0f,
                                RENDERRATIO_ASPECT_854x480, 80.0f, 40.0f,
                                counter
                            );

                            OSCAFESTDFont->SetColor(Colors::Browser);
                            OSCAFESTDFont->RenderTextScaled(
                                RENDERRATIO_ASPECT_854x480, 150.0f, 70.0f,
                                "Help/More Info",
                                690.0f, 435.0f
                            );
                        }
                    }

                    break;
                }

                case DOWNLOADSTATE_DOWNLOADING: {
                    if (OSCAFESTDFont) {
                        OSCAFESTDFont->SetColor(Colors::Black);
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, 0.0f,
                            RENDERRATIO_ASPECT_854x480, 350.0f, 75.0f,
                            "Downloading"
                        );
                    }

                    break;
                }

                case DOWNLOADSTATE_ERROR: {
                    if (OSCAFESTDFont) {
                        OSCAFESTDFont->SetColor(Colors::Black);
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, 0.0f,
                            RENDERRATIO_ASPECT_854x480, 500.0f, 90.0f,
                            Inkay::Download::HasError.load(std::memory_order_acquire)
                                ? Inkay::Download::LastError.c_str()
                                : "Unknown error"
                        );
                    }

                    break;
                }

                case DOWNLOADSTATE_FINISHED: {
                    if (OSCAFESTDFont) {
                        OSCAFESTDFont->SetColor(Colors::Black);
                        OSCAFESTDFont->RenderTextCenteredScaled(
                            0.0f, 0.0f,
                            RENDERRATIO_ASPECT_854x480, 350.0f, 75.0f,
                            "Installed Inkay!\nPress B to Restart"
                        );
                    }

                    break;
                }
            }
        }

        static void TriggerDownload(void) {
            Inkay::Download::SelectedEnvironment = Inkay::Dirs::gEnvironments[Inkay::Dirs::gSelectedEnv];
            Inkay::Download::State.store(DOWNLOADSTATE_DOWNLOADING);
            Inkay::Download::StartSourceDownload(Inkay::Download::PendingIndex);
        }

        void Update(void) {
            Inkay::Download::UpdateDownloads();
            DRC::Update();

            if (!AlreadyAppliedGUIChange) {
                u32 color = Colors::Default;

                switch (Inkay::Versions::FileState) {
                    case Inkay::Versions::LocalFileState::ValidJuxt:
                        if (PortalBG) PortalBG->ChangeTexture("fs:/vol/content/bakkuguraundo/pretendo.png");
                        color = Colors::Juxt;
                        break;

                    case Inkay::Versions::LocalFileState::Missing:
                        color = Colors::NoInkay;
                        break;

                    default:
                        break;
                }

                if (ColorChangingBars) ColorChangingBars->SetColor(color);
                AlreadyAppliedGUIChange = true;
            }

            if (Inkay::Download::State == DOWNLOADSTATE_SELECT) {
                usize total = Inkay::Repos::Sources.size();

                auto selectSource = [](usize idx) {
                    Inkay::Download::PendingIndex = (int)idx;
                    if (IO::FileExists(HBASManifest)) {
                        Inkay::Download::State.store(DOWNLOADSTATE_HBAS_WARNING);
                    } else {
                        Inkay::Download::State.store(DOWNLOADSTATE_SELECT_ENV);
                        Inkay::Dirs::LoadEnvironments();
                    }
                };

                if (Inkay::Repos::SourcesLoaded && total > 0) {
                    if (gSourceCursor >= total) gSourceCursor = total - 1;

                    usize visibleCount = (total < kVisibleSourceRows) ? total : kVisibleSourceRows;
                    if (gSourceScrollTop > total - visibleCount) gSourceScrollTop = total - visibleCount;

                    static const f32 kRowTouchY[kVisibleSourceRows] = { 92.5f, 162.5f, 232.5f, 302.5f };

                    for (usize i = 0; i < visibleCount; i++) {
                        if (DRC::IsTouchInsideSingle(302.0f, kRowTouchY[i], 350.0f, 70.0f)) {
                            usize idx = gSourceScrollTop + i;
                            gSourceCursor = idx;
                            selectSource(idx);
                            break;
                        }
                    }

                    if (total > kVisibleSourceRows) {
                        // Derived from kRowTouchY via the same centered-scaled mapping (center 477/232.5,
                        // touch = center + offset - size/2) - verify visually on hardware/Cemu.
                        if (DRC::IsTouchInsideSingle(677.0f, 162.5f, 60.0f, 40.0f) && gSourceScrollTop > 0) {
                            gSourceScrollTop--;
                        }
                        if (DRC::IsTouchInsideSingle(677.0f, 262.5f, 60.0f, 40.0f) && gSourceScrollTop + visibleCount < total) {
                            gSourceScrollTop++;
                        }
                    }

                    if (DRC::ButtonTriggered(DRC::Input::Down) && gSourceCursor + 1 < total) {
                        gSourceCursor++;
                        if (gSourceCursor >= gSourceScrollTop + visibleCount) gSourceScrollTop = gSourceCursor - visibleCount + 1;
                    }

                    if (DRC::ButtonTriggered(DRC::Input::Up) && gSourceCursor > 0) {
                        gSourceCursor--;
                        if (gSourceCursor < gSourceScrollTop) gSourceScrollTop = gSourceCursor;
                    }

                    if (DRC::ButtonTriggered(DRC::Input::A)) selectSource(gSourceCursor);
                }

                if (DRC::IsTouchInsideSingle(760.0f, 435.0f, 150.0f, 70.0f)) Inkay::Repos::AboutBrowserURL();

            } else if (Inkay::Download::State == DOWNLOADSTATE_HBAS_WARNING) {
                if (DRC::ButtonTriggered(DRC::Input::B))
                    Inkay::Download::State.store(DOWNLOADSTATE_SELECT);

            } else if (Inkay::Download::State == DOWNLOADSTATE_SELECT_ENV) {
                usize envCount = Inkay::Dirs::gEnvironments.size();

                if (envCount > 1) {
                    if (DRC::IsTouchInsideSingle(382.0f, 265.0f, 90.0f, 50.0f)) Inkay::Dirs::gSelectedEnv = (Inkay::Dirs::gSelectedEnv + 1) % envCount;
                    if (DRC::IsTouchInsideSingle(382.0f, 165.0f, 60.0f, 40.0f)) Inkay::Dirs::gSelectedEnv = (Inkay::Dirs::gSelectedEnv + envCount - 1) % envCount;

                    if (DRC::ButtonTriggered(DRC::Input::A)) TriggerDownload();
                } else {
                    TriggerDownload();
                }

                if (DRC::IsTouchInsideSingle(760.0f, 435.0f, 150.0f, 70.0f)) Inkay::Repos::AboutBrowserURL();

            } else if ((Inkay::Download::State == DOWNLOADSTATE_FINISHED) && (DRC::ButtonTriggered(DRC::Input::B))) {
                Render::FullyRebootConsole();
            }
        }

        void Shutdown(void) {
            if (OSCAFESTDFont) OSCAFESTDFont.reset();
            if (ColorChangingBars) ColorChangingBars.reset();
            if (ButtonsHolder) ButtonsHolder.reset();
            if (PortalBG) PortalBG.reset();
        }
    }
}
