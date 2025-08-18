#include "emberMath.h"
#include <iostream>



int main()
{
    Float2 a = Float2(1, 2);
    Float2 b = Float2::one;
    Float2 c = a + b;

    std::cout << c.ToString() << std::endl;
}