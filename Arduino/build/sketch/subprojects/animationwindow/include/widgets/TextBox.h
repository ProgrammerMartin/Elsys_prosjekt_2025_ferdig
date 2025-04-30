#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/subprojects/animationwindow/include/widgets/TextBox.h"
#pragma once

#include "Widget.h"
#include "Point.h"
#include "Color.h"
#include <string>

namespace TDT4102 {
    namespace internal {
        const static unsigned int TEXT_BOX_CHARACTER_LIMIT = 1000;
    }

    class TextBox : public TDT4102::Widget {
    private:
        std::string contents;
        nk_color textColor = nk_rgba(175, 175, 175, 255);
        nk_color boxColor = nk_rgba(50 , 50, 50, 255);
        nk_color borderColor = nk_rgba(50, 50, 50, 255);
    protected:
        void update(nk_context* context) override;
    public:
        explicit TextBox(TDT4102::Point location, unsigned int width, unsigned int height, std::string initialText = "");
        std::string getText() const;
        void setText(std::string updatedText);
        void setTextColor(TDT4102::Color newColor) {textColor = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};}
        void setBoxColor(TDT4102::Color newColor) {boxColor = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};}
        void setBorderColor(TDT4102::Color newColor) {borderColor = nk_color{(nk_byte)newColor.redChannel, (nk_byte)newColor.greenChannel, (nk_byte)newColor.blueChannel, (nk_byte)newColor.alphaChannel};}
    };
}