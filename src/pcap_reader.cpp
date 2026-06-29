#include "pcap_reader.h"
#include <iostream>
#include <cstring>

namespace PacketAnalyzer {

PcapReader::~PcapReader() {
    close();
}

bool PcapReader::openOffline(const std::string& filename) {
    close();
    handle_ = pcap_open_offline(filename.c_str(), errbuf_);
    if (handle_ == nullptr) {
        std::cerr << "Error opening PCAP file: " << errbuf_ << std::endl;
        return false;
    }
    std::cout << "Successfully opened offline PCAP: " << filename << std::endl;
    return true;
}

bool PcapReader::openLive(const std::string& interface, int snaplen, bool promisc, int timeout_ms) {
    close();
    // pcap_open_live requires root/admin privileges
    handle_ = pcap_open_live(interface.c_str(), snaplen, promisc ? 1 : 0, timeout_ms, errbuf_);
    if (handle_ == nullptr) {
        std::cerr << "Error opening live interface (" << interface << "): " << errbuf_ << std::endl;
        std::cerr << "Hint: Did you run with sudo/administrator privileges?" << std::endl;
        return false;
    }
    std::cout << "Listening on live interface: " << interface << " (Promiscuous mode: " << (promisc ? "ON" : "OFF") << ")" << std::endl;
    return true;
}

void PcapReader::close() {
    if (handle_ != nullptr) {
        pcap_close(handle_);
        handle_ = nullptr;
    }
}

bool PcapReader::readNextPacket(RawPacket& packet) {
    if (handle_ == nullptr) return false;

    struct pcap_pkthdr* pcap_header;
    const u_char* pcap_data;

    // Read the next packet using libpcap
    int res = pcap_next_ex(handle_, &pcap_header, &pcap_data);
    
    if (res == 1) { // Packet read successfully
        packet.header.ts_sec = pcap_header->ts.tv_sec;
        packet.header.ts_usec = pcap_header->ts.tv_usec;
        packet.header.incl_len = pcap_header->caplen;
        packet.header.orig_len = pcap_header->len;

        // Copy raw bytes into our vector
        packet.data.assign(pcap_data, pcap_data + pcap_header->caplen);
        return true;
    } 
    // res == 0 means timeout (for live capture), we just return false to try again
    // res == -1 means error, res == -2 means EOF
    if (res == -1) {
        std::cerr << "Error reading packet: " << pcap_geterr(handle_) << std::endl;
    }
    return false;
}

std::vector<std::string> PcapReader::getAvailableInterfaces() {
    std::vector<std::string> interfaces;
    pcap_if_t* alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];
    
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding devices: " << errbuf << std::endl;
        return interfaces;
    }
    
    for (pcap_if_t* d = alldevs; d != nullptr; d = d->next) {
        if (d->name) interfaces.push_back(d->name);
    }
    pcap_freealldevs(alldevs);
    return interfaces;
}

} // namespace PacketAnalyzer