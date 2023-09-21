#include <iostream>
#include "ext/dotenv.h"

int main() {
    dotenv::init(".env");
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
