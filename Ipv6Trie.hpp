#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

#include "Ipv6Net.hpp"
#include <sys/socket.h>

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
        this->trie = insertToTrie(ipv6, static_cast<uint8_t>(pop), this->trie, 0);
    }

    std::pair<uint8_t, int> find(const IPv6Net &ecs) const {
        std::optional<std::pair<uint8_t, int> > value = findInTrie(ecs, trie, 0);

        if (!value.has_value())
            throw std::invalid_argument("Ipv6 address" + ecs.toString() + "does not have any nearby Pop server.\n");

        return value.value();
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
    std::shared_ptr<Node> insertToTrie(const IPv6Net &ipv6, uint8_t pop, std::shared_ptr<Node> &oldNode, int bitIndex) {
        auto node = oldNode ? std::make_shared<Node>(*oldNode) : std::make_shared<Node>();

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

    std::optional<std::pair<uint8_t, int>>
    findInTrie(const IPv6Net &ipv6, const std::shared_ptr<Node> &node, int bitIndex) const {
        if (!node ||
            bitIndex > ipv6.getPrefixLen())
            return std::nullopt;

        auto idx = bitValueAt(ipv6.getIp(), bitIndex);
        auto child = node->children[idx];

        std::optional<std::pair<uint8_t, int>> value = std::nullopt;
        if (node->hasValue)
            value = std::make_pair(node->value, bitIndex);

        auto newValue = findInTrie(ipv6, child, bitIndex + 1);
        if (newValue.has_value())
            value = newValue;

        return value;
    }

    std::shared_ptr<Node> trie;
};