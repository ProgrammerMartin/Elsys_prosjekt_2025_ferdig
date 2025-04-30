#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/subprojects/animationwindow/include/widgets/DropdownList.h"
#pragma once

#include "Widget.h"
#include <string>
#include <vector>

namespace TDT4102 {
    class DropdownList : public TDT4102::Widget {
    private:
        std::vector<std::string> options;
        unsigned int selectedIndex = 0;
    protected:
        void update(nk_context* context) override;
    public:
        explicit DropdownList(TDT4102::Point location, unsigned int width, unsigned int height, std::vector<std::string> &initialOptions);
        std::string getSelectedValue() const;
        void setOptions(std::vector<std::string> &updatedOptionsList);
    };
}