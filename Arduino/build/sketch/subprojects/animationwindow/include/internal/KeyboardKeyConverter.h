#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/subprojects/animationwindow/include/internal/KeyboardKeyConverter.h"
#pragma once

#include "KeyboardKey.h"
#include "SDL.h"

namespace TDT4102 {
    namespace internal {
        KeyboardKey convertSDLKeyToKeyboardKey(SDL_Keysym key);
    }
}