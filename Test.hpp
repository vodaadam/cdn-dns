#pragma once

#include <utility>
#include <memory>
#include <stdexcept>

#include "Ipv6Net.hpp"

class Test {
public:
    Test(std::shared_ptr<IPv6Trie> trie) : trie(std::move(trie)) {
        insertData();
    }

    void insertDataTest() {
        std::cout << "INSERT DATA TESTS" << std::endl;
        auto addr2 = IPv6Net("2a0c:b641:90::", 49);

        std::cout << "Test: Same ipv6 that has been already inserted" << std::endl;
        {
            bool thrown = false;
            try {
                trie->insert(addr2, 178);
            } catch (const std::invalid_argument &e) {
                thrown = true;
            }
            if (!thrown)
                throw std::runtime_error("Expected std::invalid_argument was not thrown");
        }

        std::cout << "Test: Invalid prefix length" << std::endl;
        {
            bool thrown = false;
            try {
                trie->insert(addr2, 276);
            } catch (const std::out_of_range &e) {
                thrown = true;
            }
            if (!thrown)
                throw std::runtime_error("Expected std::invalid_argument was not thrown");
        }
    }

    void findTest() {
        std::cout << "FIND DATA TESTS" << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:90::", 48);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(178), 48}));
        }

        {
            auto addr = IPv6Net("2a0c:b641:9003::", 48);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(178), 32}));
        }

        {
            auto addr = IPv6Net("2a0c:b641:90::", 49);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(236), 49}));
        }
    }

private:

    void insertData() {
        auto addr1 = IPv6Net("2a0c:b641:90::", 48);
        auto addr2 = IPv6Net("2a0c:b641:901::", 48);
        auto addr3 = IPv6Net("2a0c:b641:90::", 49);
        auto addr4 = IPv6Net("2a0c:b641:9002::", 48);
        auto addr5 = IPv6Net("2a0c:b641::", 32);

        trie->insert(addr1, 178);
        trie->insert(addr2, 172);
        trie->insert(addr3, 236);
        trie->insert(addr4, 232);
        trie->insert(addr5, 178);
    }

    std::shared_ptr<IPv6Trie> trie;
};
