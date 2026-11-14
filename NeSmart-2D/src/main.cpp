#include "app/Application.hpp"
#include <exception>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H



int main() {
    FT_Library library;
    if (FT_Init_FreeType(&library) != 0)
    {
        throw std::runtime_error("FreeType init failed");
    }
    FT_Done_FreeType(library);
    try {
        nesmart::app::Application application;
        return application.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
}