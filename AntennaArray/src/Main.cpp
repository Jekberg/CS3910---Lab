#include "AntennaArray.h"
#include <iostream>

int main()
{
    AntennaArray aa{3, 90.0};
    std::cout << aa.evaluate({0.5, 1.0, 1.5});
}