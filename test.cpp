#include "Envelope.h"
#include <iostream>

Envelope env;

int main() {
    env.setT1(100);
    env.setT2(100);
    env.setL2(0.0);
    std::cout << "Time 0: " << env.getLevel(0) << "\n";
    std::cout << "Time 1: " << env.getLevel(1) << "\n";
    std::cout << "Time 50: " << env.getLevel(50) << "\n";
    std::cout << "Time 100: " << env.getLevel(100) << "\n";
    std::cout << "Time 150: " << env.getLevel(150) << "\n";
    std::cout << "Time 200: " << env.getLevel(200) << "\n";

    std::cout << "Time 1000: " << env.getLevel(1000) << "\n";

    return 0;
}
