#include "app/Application.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        nesmart::app::Application application;
        return application.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
}