#include <BLIB/Containers.hpp>

int main() {
    std::cout << "Creating\n";
    bl::Any<8> any = std::string("hi");
    std::cout << "created\n";
    std::cout << any.get<std::string>();
    return 0;
}