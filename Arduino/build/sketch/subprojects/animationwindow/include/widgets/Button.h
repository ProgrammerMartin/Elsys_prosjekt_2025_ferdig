#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/subprojects/animationwindow/include/widgets/Button.h"
#pragma once

#include "Widget.h"
#include "Point.h"
#include "Color.h"
#include <string>

namespace TDT4102 {
    class Button : public TDT4102::Widget {
    private:
        std::string label;
        bool lastRightMouseButtonState = false;
        bool lastLeftMouseButtonState = false;
        nk_color labelColor = nk_rgba(175, 175, 175, 255);
        nk_color buttonColor = nk_rgba(50,50,50,255);
        nk_color buttonColorHover = nk_rgba(40, 40, 40,255);
        nk_color buttonColorActive = nk_rgba(35, 35, 35,255);

    protected:
        void update(nk_context* context) override;
    public:
        explicit Button(TDT4102::Point location, unsigned int width, unsigned int height, std::string label);
        void setLabel(std::string newLabel);
        void setLabelColor(Color newColor) {labelColor = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};};
        void setButtonColor(Color newColor) {buttonColor = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};}
        void setButtonColorHover(Color newColor) {buttonColorHover = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};}
        void setButtonColorActive(Color newColor) {buttonColorActive = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};}
    };
}