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

class IPv6Trie {
public:
    IPv6Trie() = default;

    void insert(const IPv6Net &ipv6, int pop) {
        if (pop < 0 || pop > 255) {
            throw std::out_of_range("PoP value must fit into uint8_t (0..255)");
        }
        this->trie = insertToTrie(ipv6, static_cast<uint8_t>(pop), this->trie, 0);
    }

    /**
     * Routes an ECS subnet using Longest Prefix Match
     * @param ecs ECS subnet
     * @return (pop, scopePrefixLen)
     * @throws std::invalid_argument If no route exists
     */
    std::pair<uint8_t, int> find(const IPv6Net &ecs) {
        std::optional<std::pair<uint8_t, int> > value = findInTrie(ecs, trie, 0);

        if (!value.has_value())
            throw std::invalid_argument("Ipv6 address" + ecs.toString() + "does not have any nearby Pop server.\n");

        return value.value();
    }

private:

    /**
     * Reads one bit from an IPv6 address
     * @param addrBytes IPv6 address bytes
     * @param bitIndex Bit index
     * @return Bit value (0 or 1)
     * */
    static int bitValueAt(const std::vector<uint8_t> &addrBytes, const int bitIndex) {
        const uint8_t byte = addrBytes[bitIndex / 8];
        const int bitInByte = 7 - (bitIndex % 8);
        return (byte >> bitInByte) & 0x1;
    }

    /**
     * Inserts a prefix into the bit trie up to prefixLen
     * @param prefix Routing prefix to insert
     * @param pop PoP id for this prefix
     * @param node Current node
     * @param bitIndex Current bit depth
     * @return Updated node pointer
     * @throws std::invalid_argument If the prefix already exists
     * */
    std::shared_ptr<Node> insertToTrie(const IPv6Net &ecs, uint8_t pop, std::shared_ptr<Node> &node, int bitIndex) {
        if (!node) node = std::make_shared<Node>();

        int ipLenBit = ecs.getPrefixLen();

        if (bitIndex == ipLenBit) {
            if (node->hasValue)
                throw std::invalid_argument("Ipv6 " + ecs.toString() + "already exists in IPv6Trie.");
            node->hasValue = true;
            node->value = pop;
            return node;
        }

        int idx = bitValueAt(ecs.getIp(), bitIndex);
        auto &child = node->children[idx];
        node->children[idx] = insertToTrie(ecs, pop, child, bitIndex + 1);
        return node;
    }

    /**
     * Performs LPM lookup for ECS
     * @param ecs ECS subnet
     * @param node Current node
     * @param bitIndex Current bit depth
     * @return (pop, matchedLen) or nullopt
     * */
    std::optional<std::pair<uint8_t, int>>
    findInTrie(const IPv6Net &ecs, const std::shared_ptr<Node> &node, int bitIndex) const {
        if (!node)
            return std::nullopt;

        std::optional<std::pair<uint8_t, int>> value = std::nullopt;

        if (node->hasValue)
            value = std::make_pair(node->value, bitIndex);

        if (bitIndex == ecs.getPrefixLen())
            return value;

        auto idx = bitValueAt(ecs.getIp(), bitIndex);
        auto child = node->children[idx];

        if (node->hasValue)
            value = std::make_pair(node->value, bitIndex);

        auto newValue = findInTrie(ecs, child, bitIndex + 1);
        if (newValue.has_value())
            value = newValue;

        return value;
    }

    std::shared_ptr<Node> trie;
};