#include <iostream>

int main() {
#if __cplusplus == 201703L
    std::cerr << "C++17" << std::endl;
    return 0;
#else
    std::cerr << "C++" << std::endl;
#endif
    return -1;
}
