#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class IPv6Net {
public:
    IPv6Net(std::string addrBytes, uint8_t prefixLen) {
        setPrefixLen(prefixLen);
        setIp(addrBytes);
    }

    const std::vector<uint8_t> &getIp() const { return addrBytes; }

    const uint8_t &getPrefixLen() const { return prefixLen; }

private:
    std::vector<uint8_t> addrBytes;
    uint8_t prefixLen;

    void setIp(const std::string &ipStr) {
        if (ipStr.find('/') != std::string::npos) {
            throw std::invalid_argument("IP must not contain '/'. Prefix length is provided separately.");
        }

        in6_addr addr{};
        if (inet_pton(AF_INET6, ipStr.c_str(), &addr) != 1)
            throw std::invalid_argument("Invalid IPv6 address format.");

        addrBytes.resize(16);
        std::memcpy(this->addrBytes.data(), addr.s6_addr, 16);
        for (uint8_t el: addrBytes)
            std::cout << el << std::endl;
    }

    void setPrefixLen(uint8_t prefixLen) {
        if (prefixLen > 128)
            throw std::out_of_range("Prefix must be less than 128.");
        this->prefixLen = prefixLen;
    }
};


struct Node {
    uint8_t value;
    bool hasValue = false;
    std::shared_ptr<Node> children[16];
};

class IPv6Trie { ;
public:
    IPv6Trie() = default;

    void insert(const IPv6Net &ipv6, int pop) {
        if (pop < 0 || pop > 255) {
            throw std::out_of_range("PoP value must fit into uint8_t (0..255)");
        }
        this->trie = insertToTrie(ipv6, static_cast<uint8_t>(pop), trie, 0);
    }

    uint8_t route(const IPv6Net &ecs) const {

    }

private:

    static int nibbleIndexAt(const std::vector<uint8_t> &ip, const int hexIndex) {
        const auto &byte = ip[hexIndex / 2];
        return (hexIndex % 2 == 0) ? (byte >> 4) & 0xf : byte & 0xf;
    }

    /**
     * Inserts IPv6 prefix (address + prefixLen) into hex trie.
     *
     * @param ipv6 IPv6 prefix (network address in bytes + prefix length in bits)
     * @param pop  PoP ID assigned to this prefix
     * @param node current trie node
     * @param hexIndex index of current hex digit (nibble) being processed
     * @return std::share_ptr<Node> trie with inserted value
     */
    std::shared_ptr<Node> insertToTrie(const IPv6Net &ipv6, uint8_t pop, std::shared_ptr<Node> &node, int hexIndex) {
        if (!node) node = std::make_shared<Node>();

        int ipLenHex = ipv6.getPrefixLen() / 4;

        if (hexIndex == ipLenHex) {
            node->hasValue = true;

            node->value = pop;
            return node;
        }

        int idx = nibbleIndexAt(ipv6.getIp(), hexIndex);

        auto &child = node->children[idx];
        node->children[idx] = insertToTrie(ipv6, pop, child, hexIndex + 1);
        return node;
    }

    std::shared_ptr<Node> trie;
};


int main() {
    std::cout << "Hello, World!" << std::endl;
    auto trie = IPv6Trie();
    auto ipv6Net = IPv6Net("2a0c:b641:90::", 48);
    trie.insert(ipv6Net, 174);
    return 0;
}
