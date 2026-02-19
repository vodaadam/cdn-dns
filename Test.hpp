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

        std::cout << "Test: LPM - address inside /49 should return PoP from /49 (236) and length 49" << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:90::1", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(236), 49}));
        }

        std::cout << "Test: LPM - /49 boundary (7fff) is still inside /49 -> (236, 49)" << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:90:7fff::1", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(236), 49}));
        }

        std::cout << "Test: LPM - /49 boundary (8000) is outside /49, should fall back to /48 -> (178, 48)"
                  << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:90:8000::1", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(178), 48}));
        }

        std::cout << "Test: LPM - address in 2a0c:b641:901::/48 should return (172, 48)" << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:901::abcd", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(172), 48}));
        }

        std::cout << "Test: LPM - address in 2a0c:b641:9002::/48 should return (232, 48)" << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:9002::beef", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(232), 48}));
        }

        std::cout << "Test: LPM - fallback to /32 (2a0c:b641::/32) should return (178, 32)" << std::endl;
        {
            auto addr = IPv6Net("2a0c:b641:abcd::1", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(178), 32}));
        }

        std::cout << "Test: No route - address outside inserted prefixes must throw std::invalid_argument" << std::endl;
        {
            bool thrown = false;
            try {
                trie->find(IPv6Net("2a0c:b642::1", 128));
            } catch (const std::invalid_argument &) {
                thrown = true;
            }
            if (!thrown)
                throw std::runtime_error("Expected std::invalid_argument was not thrown (no route)");
        }

        std::cout << "Test: Default route - after inserting ::/0, unknown address should return (1, 0)" << std::endl;
        {
            trie->insert(IPv6Net("::", 0), 1);

            auto addr = IPv6Net("2001:db8::1", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(1), 0}));
        }

        std::cout << "Test: LPM - host route /128 must override /49 (insert 2a0c:b641:90::1/128 -> 99)" << std::endl;
        {
            trie->insert(IPv6Net("2a0c:b641:90::1", 128), 99);

            auto addr = IPv6Net("2a0c:b641:90::1", 128);
            auto res = trie->find(addr);
            assert((res == std::pair<uint8_t, int>{static_cast<uint8_t>(99), 128}));

            std::cout
                    << "Test: LPM - another address in the same /49 has no host route, should stay on /49 -> (236, 49)"
                    << std::endl;
            auto addr2 = IPv6Net("2a0c:b641:90::2", 128);
            auto res2 = trie->find(addr2);
            assert((res2 == std::pair<uint8_t, int>{static_cast<uint8_t>(236), 49}));
        }

        std::cout << "FIND DATA TESTS OK" << std::endl;
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
