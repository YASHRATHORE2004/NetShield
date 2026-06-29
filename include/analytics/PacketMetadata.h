#ifndef NETSHIELD_ANALYTICS_PACKETMETADATA_H
#define NETSHIELD_ANALYTICS_PACKETMETADATA_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace DPI::Analytics {

// Lightweight representation of a single processed packet for analytics.
struct PacketMetadata {
    std::uint64_t timestamp = 0;
    std::string source_ip;
    std::string destination_ip;
    std::uint16_t source_port = 0;
    std::uint16_t destination_port = 0;
    std::string protocol;
    std::size_t packet_size = 0;
    std::optional<std::string> hostname;
    bool is_tls = false;
    bool is_http = false;
};

} // namespace DPI::Analytics

#endif // NETSHIELD_ANALYTICS_PACKETMETADATA_H
