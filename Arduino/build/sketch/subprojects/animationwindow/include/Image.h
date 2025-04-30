#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/subprojects/animationwindow/include/Image.h"
#pragma once

#include <string>
#include "Point.h"
#include <filesystem>
#include "SDL_render.h"

namespace TDT4102 {
    struct Image {
        // Allow the texture to be read out and drawn despite being private
        friend class AnimationWindow;

    public:
        explicit Image();
        explicit Image(std::filesystem::path pathToImageFile);
        ~Image();

        int width = 0;
        int height = 0;
    private:

        SDL_Texture *texture = nullptr;
        std::filesystem::path src = "non existent file";

        void load(SDL_Renderer* renderer);
        void draw(SDL_Renderer* renderer, TDT4102::Point location, int imageWidth = 0, int imageHeight = 0);
    };
}
