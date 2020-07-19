#include <iostream>

int main() {
#if __cplusplus == 201703L
    std::cout << "C++17" << std::endl;
    return 0;
#else
    std::cout << "C++" << std::endl;
#endif
    return -1;
}
