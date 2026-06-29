#pragma once

#include <cstdint>
#include <string>

enum class ProtocolType
{
    TCP,
    UDP,
    ICMP,
    UNKNOWN
};

enum class PacketDirection
{
    INBOUND,
    OUTBOUND,
    UNKNOWN
};

using Port = uint16_t;
using PacketSize = uint32_t;
using Timestamp = uint64_t;
using IPAddress = std::string; // Assuming IP address as string for simplicity
using DomainName = std::string;
