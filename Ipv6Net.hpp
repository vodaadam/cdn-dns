#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>

#include <netinet/in.h>
#include <arpa/inet.h>

class IPv6Net {
public:
    /**
     * Creates an IPv6 prefix (address + prefix length)
     * @param ipStr IPv6 address string without "/"
     * @param prefixLen Prefix length in bits
     */
    IPv6Net(const std::string &ipStr, uint8_t prefixLen) {
        setPrefixLen(prefixLen);
        setIp(ipStr);
    }

    /**
     * Returns the IPv6 address as 16 bytes
     * @return Address bytes
     */
    const std::vector<uint8_t> &getIp() const { return addrBytes; }

    /**
     * Returns prefix length in bits
     * @return Prefix length
     */
    uint8_t getPrefixLen() const { return prefixLen; }

    /**
     * Formats as "addr/prefixLen"
     * @return String representation
     */
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
    uint8_t prefixLen{0};

    /**
     * Parses IPv6 address string into bytes
     * @param ipStr IPv6 address without "/"
     * @throws std::invalid_argument If ipStr is invalid or contains '/'
     */
    void setIp(const std::string &ipStr) {
        if (ipStr.find('/') != std::string::npos) {
            throw std::invalid_argument("IP must not contain '/'. Prefix length is provided separately.");
        }

        in6_addr addr{};
        if (inet_pton(AF_INET6, ipStr.c_str(), &addr) != 1) {
            throw std::invalid_argument("Invalid IPv6 address format.");
        }

        addrBytes.resize(16);
        std::memcpy(addrBytes.data(), addr.s6_addr, 16);
    }

    /**
     * Validates prefix length
     * @param newPrefixLen Prefix length
     * @throws std::out_of_range If newPrefixLen > 128
     */
    void setPrefixLen(uint8_t newPrefixLen) {
        if (newPrefixLen > 128) {
            throw std::out_of_range("Prefix must be <= 128.");
        }
        prefixLen = newPrefixLen;
    }
};
