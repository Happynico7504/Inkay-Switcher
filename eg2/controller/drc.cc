#include <vpad/input.h>
#include "eg2/libs/typedefs.h"
#include "eg2/sys/report.hh"
#include "eg2/controller/drc.hh"
#include "eg2/render/scan.hh"

namespace DRC {
    namespace Controller {
        VPADStatus DRCInput[Channel::DRC_MAXCON];
        VPADReadError DRCError[Channel::DRC_MAXCON];

        static bool PrevTouched[Channel::DRC_MAXCON]; 
        static bool TouchTriggered[Channel::DRC_MAXCON];
    }

    static bool GetTouchPos(Vector2i* outPos, VPADChan channel) {
        if (!IsValidChannel(channel)) return false;
        if (Controller::DRCError[channel] != VPAD_READ_SUCCESS) return false;
        if (!Controller::DRCInput[channel].tpNormal.touched) return false;

        VPADTouchData calibrated;
        VPADGetTPCalibratedPointEx(channel, VPAD_TP_854X480, &calibrated, &Controller::DRCInput[channel].tpNormal);

        if (calibrated.touched) {
            if (outPos) {
                outPos->x = (s32)calibrated.x;
                outPos->y = (s32)calibrated.y;
            }
            return true;
        }
        return false;
    }

    void Update(void) { 
        for (u8 i = 0; i < Channel::DRC_MAXCON; i++) {
            VPADRead((VPADChan)i, &Controller::DRCInput[i], 1, &Controller::DRCError[i]);
            if (Controller::DRCError[i] == VPAD_READ_SUCCESS) {
                bool currentlyTouched = (Controller::DRCInput[i].tpNormal.touched != 0);

                Controller::TouchTriggered[i] = currentlyTouched && !Controller::PrevTouched[i];
                Controller::PrevTouched[i] = currentlyTouched;
            }
        }
    }

    bool IsValidChannel(VPADChan ch) { return (u8)ch < Channel::DRC_MAXCON; }

    bool IsTouchInside(f32 x, f32 y, f32 w, f32 h) { return IsTouchInsideEx(x, y, w, h, DRC::Channel::DRC0); }

    bool IsTouchInsideEx(f32 x, f32 y, f32 w, f32 h, VPADChan channel) {
        if (!IsValidChannel(channel)) return false;
        if (Controller::DRCError[channel] != VPAD_READ_SUCCESS) return false;
        Vector2i touchPos;
        if (GetTouchPos(&touchPos, channel)) return (touchPos.x >= (s32)x && touchPos.x <= (s32)(x + w) && touchPos.y >= (s32)y && touchPos.y <= (s32)(y + h));
        return false;
    }

    bool IsTouchInsideSingle(f32 x, f32 y, f32 w, f32 h) { return IsTouchInsideSingleEx(x, y, w, h, DRC::Channel::DRC0); }

    bool IsTouchInsideSingleEx(f32 x, f32 y, f32 w, f32 h, VPADChan channel) {
        if (!IsValidChannel(channel)) return false;
        if (Controller::DRCError[channel] != VPAD_READ_SUCCESS) return false;

        if (Controller::TouchTriggered[channel]) return IsTouchInsideEx(x, y, w, h, channel);
        return false;
    }

    bool ButtonHeld(u32 button) { return ButtonHeldEx(button, DRC::Channel::DRC0); }

    bool ButtonHeldEx(u32 button, VPADChan channel) {
        if (!IsValidChannel(channel)) return false;
        if (Controller::DRCError[channel] == VPAD_READ_SUCCESS) return (Controller::DRCInput[channel].hold & button) != 0;
        return false;
    }

    bool ButtonTriggered(u32 button) { return ButtonTriggeredEx(button, DRC::Channel::DRC0); }

    bool ButtonTriggeredEx(u32 button, VPADChan channel) {
        if (!IsValidChannel(channel)) return false;
        if (Controller::DRCError[channel] == VPAD_READ_SUCCESS) return (Controller::DRCInput[channel].trigger & button) != 0;
        return false;
    }
}