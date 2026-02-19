#include <iostream>
#include <memory>

#include "Ipv6Trie.hpp"
#include "Test.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto trie = std::make_shared<IPv6Trie>();
    auto test = Test(trie);
    test.insertDataTest();
    test.findTest();
    return 0;
}
