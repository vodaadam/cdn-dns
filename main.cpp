#include <iostream>
#include <utility>
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
    IPv6Net(const std::string &addrBytes, uint8_t prefixLen) {
        setPrefixLen(prefixLen);
        setIp(addrBytes);
    }

    const std::vector<uint8_t> &getIp() const { return addrBytes; }

    const uint8_t &getPrefixLen() const { return prefixLen; }

    std::string toString() const {
        in6_addr addr{};
        if (addrBytes.size() == 16) {
            std::memcpy(addr.s6_addr, addrBytes.data(), 16);
        }
        char buf[INET6_ADDRSTRLEN]{};
        const char *ok = inet_ntop(AF_INET6, &addr, buf, sizeof(buf));
        std::string ip = ok ? std::string(buf) : std::string("<invalid>");
        return ip + "/" + std::to_string(static_cast<int>(prefixLen));
    }

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
    }

    void setPrefixLen(uint8_t newPrefixLen) {
        if (newPrefixLen > 128)
            throw std::out_of_range("Prefix must be less than 128.");
        this->prefixLen = newPrefixLen;
    }
};


struct Node {
    uint8_t value;
    bool hasValue = false;
    std::shared_ptr<Node> children[2];
};

class IPv6Trie { ;
public:
    IPv6Trie() = default;

    void insert(const IPv6Net &ipv6, int pop) {
        if (pop < 0 || pop > 255) {
            throw std::out_of_range("PoP value must fit into uint8_t (0..255)");
        }
        auto copy = makeTrieCopy(this->trie);
        this->trie = insertToTrie(ipv6, static_cast<uint8_t>(pop), copy, 0);

    }

    uint8_t find(const IPv6Net &ecs) const {

        return findInTrie(ecs, trie, 0);
    }

    static std::shared_ptr<Node> makeTrieCopy(const std::shared_ptr<Node> &node) {
        if (!node) return nullptr;
        auto newNode = std::make_shared<Node>(*node);
        for (auto &child: newNode->children)
            if (child)
                child = makeTrieCopy(child);
        return newNode;
    }

private:

    /**
     * Finds bit's value
     * @param addrBytes address represented in bytes
     * @param bitIndex bitIndex in ipv6 address
     * @return bit value 0 or 1
     * */
    static int bitValueAt(const std::vector<uint8_t> &addrBytes, const int bitIndex) {
        const uint8_t byte = addrBytes[bitIndex / 8];
        const int bitInByte = 7 - (bitIndex % 8);
        return (byte >> bitInByte) & 0x1;
    }

    /**
     * Inserts IPv6 prefix (address + prefixLen) into hex trie.
     *
     * @param ipv6 IPv6 prefix (network address in bytes + prefix length in bits)
     * @param pop  PoP ID assigned to this prefix
     * @param node current trie node
     * @param hexIndex index of current hex digit (nibble) being processed
     * @return std::share_ptr<Node> trie with inserted value
     * */
    std::shared_ptr<Node> insertToTrie(const IPv6Net &ipv6, uint8_t pop, std::shared_ptr<Node> &node, int bitIndex) {
        if (!node) node = std::make_shared<Node>();

        int ipLenBit = ipv6.getPrefixLen();

        if (bitIndex == ipLenBit) {
            if (node->hasValue)
                throw std::invalid_argument("Ipv6 " + ipv6.toString() + "already exists in IPv6Trie.");
            node->hasValue = true;
            node->value = pop;
            return node;
        }

        int idx = bitValueAt(ipv6.getIp(), bitIndex);

        auto &child = node->children[idx];
        node->children[idx] = insertToTrie(ipv6, pop, child, bitIndex + 1);
        return node;
    }

    uint8_t findInTrie(const IPv6Net &ipv6, const std::shared_ptr<Node> &node, int bitIndex) const {
        if (!node ||
            bitIndex > ipv6.getPrefixLen())
            throw std::invalid_argument("Ipv6 address" + ipv6.toString() + "does not have any nearby Pop server.\n");

        auto idx = bitValueAt(ipv6.getIp(), bitIndex);
        auto child = node->children[idx];

        if (!child
            && node->hasValue)
            return node->value;

        return findInTrie(ipv6, child, bitIndex + 1);
    }

    std::shared_ptr<Node> trie;
};

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

private:

    void insertData() {
        auto addr1 = IPv6Net("2a0c:b641:90::", 48);
        auto addr2 = IPv6Net("2a0c:b641:901::", 48);
        auto addr3 = IPv6Net("2a0c:b641:90::", 49);

        trie->insert(addr1, 178);
        trie->insert(addr2, 172);
        trie->insert(addr3, 236);
    }

    std::shared_ptr<IPv6Trie> trie;
};


int main() {
    std::cout << "Hello, World!" << std::endl;
    auto trie = std::make_shared<IPv6Trie>();
    auto test = Test(trie);
    test.insertDataTest();
    return 0;
}
