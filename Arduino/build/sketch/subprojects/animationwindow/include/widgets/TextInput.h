#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/subprojects/animationwindow/include/widgets/TextInput.h"
#pragma once

#include "Widget.h"
#include "Point.h"
#include <string>

namespace TDT4102 {
    namespace internal {
        const static unsigned int TEXT_INPUT_CHARACTER_LIMIT = 1000;
    }

    class TextInput : public TDT4102::Widget {
    private:
        std::string contents;
        std::string previousContents;
    protected:
        void update(nk_context* context) override;
    public:
        explicit TextInput(TDT4102::Point location, unsigned int width, unsigned int height, std::string initialText = "");
        std::string getText() const;
        void setText(std::string text);
    };
}