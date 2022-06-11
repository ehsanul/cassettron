#include "Envelope.h"
#include <iostream>

Envelope env;

int main() {
    env.setT1(100);
    env.setT2(100);
    env.setT3(100);
    env.setT4(1000);
    env.setL2(0.0);
    env.setL3(0.3);
    std::cout << "Time 0: " << env.holdLevel(0) << "\n";
    std::cout << "Time 1: " << env.holdLevel(1) << "\n";
    std::cout << "Time 50: " << env.holdLevel(50) << "\n";
    std::cout << "Time 100: " << env.holdLevel(100) << "\n";
    std::cout << "Time 150: " << env.holdLevel(150) << "\n";
    std::cout << "Time 200: " << env.holdLevel(200) << "\n";
    std::cout << "Time 1000: " << env.holdLevel(1000) << "\n";
    std::cout << "Time 1500: " << env.releaseLevel(500) << "\n";
    std::cout << "Time 2000: " << env.releaseLevel(1000) << "\n";

    return 0;
}
