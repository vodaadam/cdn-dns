#include <iostream>

class IPv6Net {
public:
    IPv6Net(std::string ip, uint8_t prefixLen) {
        setPrefixLen(prefixLen);
        setIp(ip);
    }

private:
    std::array<uint8_t, 16> ip;
    uint8_t prefixLen;

    void setIp(std::string ip) {

    }

    void setPrefixLen(uint8_t prefixLen){
        if(prefixLen > 128)
            throw std::out_of_range("Prefix must be less than 128.");
        this->prefixLen = prefixLen;
    }

};


struct Node {
    uint8_t key;
    uint8_t value;
    bool hasValue;
    std::shared_ptr<Node>children[16];
};

class Trie {
    Trie() {};

    bool insert (std::string ivp6, int value){


        return true;
    }

    std::pair<uint16_t, int> Route(const IPv6Net& ecs) const{

    }



};


int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
