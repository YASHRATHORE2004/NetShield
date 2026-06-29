#ifndef NETSHIELD_ANALYTICS_TRAFFICSNAPSHOT_H
#define NETSHIELD_ANALYTICS_TRAFFICSNAPSHOT_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace DPI::Analytics {

// Immutable snapshot of analytics state at a point in time.
struct TrafficSnapshot {
    std::size_t total_packets = 0;
    std::size_t total_bytes = 0;
    double average_packet_size = 0.0;
    std::size_t largest_packet = 0;
    std::size_t smallest_packet = 0;

    std::unordered_map<std::string, std::size_t> protocol_distribution;
    std::vector<std::pair<std::string, std::size_t>> top_source_ips;
    std::vector<std::pair<std::string, std::size_t>> top_destination_ips;
    std::vector<std::pair<std::uint16_t, std::size_t>> top_ports;
    std::vector<std::pair<std::string, std::size_t>> top_domains;
};

} // namespace DPI::Analytics

#endif // NETSHIELD_ANALYTICS_TRAFFICSNAPSHOT_H
