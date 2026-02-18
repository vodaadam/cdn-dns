#include <iostream>
#include <netinet6/in6.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class IPv6Net {
public:
    IPv6Net(std::string ip, uint8_t prefixLen) {
        setPrefixLen(prefixLen);
        setIp(ip);
    }

    const std::vector<uint8_t> getIp() const { return ip; }

private:
    std::vector<uint8_t> ip;
    uint8_t prefixLen;

    void setIp(const std::string& ipStr) {
        if (ipStr.find('/') != std::string::npos) {
            throw std::invalid_argument("IP must not contain '/'. Prefix length is provided separately.");
        }

        in6_addr addr{};
        if (inet_pton(AF_INET6, ipStr.c_str(), &addr) != 1)
            throw std::invalid_argument("Invalid IPv6 address format.");

        ip.resize(16);
        std::memcpy(this->ip.data(), addr.s6_addr, 16);
        for(uint8_t el : ip )
            std::cout << el << std::endl;
    }

    void setPrefixLen(uint8_t prefixLen){
        if(prefixLen > 128)
            throw std::out_of_range("Prefix must be less than 128.");
        this->prefixLen = prefixLen;
    }
};


struct Node {
    uint8_t value;
    bool hasValue;
    std::shared_ptr<Node>children[16];
};

class Trie {
    Trie() {};

    bool insert(const IPv6Net& ipv6, int pop) {
        if (pop < 0 || pop > 255) {
            throw std::out_of_range("PoP value must fit into uint8_t (0..255) or change Node.value to uint16_t.");
        }
        return insertToTrie(ipv6, static_cast<uint8_t>(pop), trie, 0);
    }

    bool insertToTrie(const IPv6Net& ipv6, uint8_t pop, std::shared_ptr<Node>& node, int ipIndex) {
        if (!node) node = std::make_shared<Node>();

        auto ipLen = ipv6.getIp().size();

        if (ipIndex == ipLen) {
            node->hasValue = true;
            node->value = pop;
            return true;
        }

        auto& child = node->children[ipv6.getIp()[ipIndex]];
        if (!child) child = std::make_shared<Node>();
        return insertToTrie(ipv6, pop, child, ipIndex + 1);
    }

    std::pair<uint16_t, int> route(const IPv6Net& ecs) const{

    }
    std::shared_ptr<Node> trie;
};


int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
