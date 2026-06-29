#ifndef PCAP_READER_H
#define PCAP_READER_H

#include <cstdint>
#include <string>
#include <vector>
#include <pcap.h>

namespace PacketAnalyzer {

// PCAP Packet Header (matches your original structure)
struct PcapPacketHeader {
    uint32_t ts_sec;         // Timestamp seconds
    uint32_t ts_usec;        // Timestamp microseconds
    uint32_t incl_len;       // Number of bytes captured
    uint32_t orig_len;       // Actual length of packet on wire
};

// Represents a single captured packet
struct RawPacket {
    PcapPacketHeader header;
    std::vector<uint8_t> data;
};

class PcapReader {
public:
    PcapReader() = default;
    ~PcapReader();

    // Open an offline .pcap file
    bool openOffline(const std::string& filename);
    
    // NEW: Open a live network interface (e.g., "eth0", "wlan0")
    bool openLive(const std::string& interface, int snaplen = 65535, bool promisc = true, int timeout_ms = 1000);
    
    void close();
    bool readNextPacket(RawPacket& packet);
    bool isOpen() const { return handle_ != nullptr; }

    // Helper to get a list of available interfaces
    static std::vector<std::string> getAvailableInterfaces();

private:
    pcap_t* handle_ = nullptr;
    char errbuf_[PCAP_ERRBUF_SIZE];
};

} // namespace PacketAnalyzer

#endif // PCAP_READER_H