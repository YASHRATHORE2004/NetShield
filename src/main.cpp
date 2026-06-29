#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "pcap_reader.h"
#include "packet_parser.h"

using namespace PacketAnalyzer;

// ==========================================
// Phase 2: Real-Time Intrusion Detection System
// ==========================================
class IntrusionDetector {
private:
    std::unordered_map<std::string, int> syn_counts;
    std::unordered_map<std::string, std::unordered_set<uint16_t>> port_scans;
    
    // Thresholds for detecting attacks
    const int SYN_FLOOD_THRESHOLD = 20; // 20 SYN packets without ACKs
    const int PORT_SCAN_THRESHOLD = 10; // 10 unique ports hit by a single IP

public:
    void analyze(const ParsedPacket& pkt) {
        if (!pkt.has_ip || !pkt.has_tcp) return;

        // 1. Detect Port Scans
        port_scans[pkt.src_ip].insert(pkt.dest_port);
        if (port_scans[pkt.src_ip].size() >= PORT_SCAN_THRESHOLD) {
            std::cout << "\n\033[1;31m[🚨 IDS ALERT] Port Scan Detected! IP: " 
                      << pkt.src_ip << " scanned " << port_scans[pkt.src_ip].size() << " unique ports.\033[0m\n";
            port_scans[pkt.src_ip].clear(); // Reset after alert
        }

        // 2. Detect SYN Floods
        // TCP Flags: SYN is 0x02, ACK is 0x10. A pure SYN packet has 0x02 set and 0x10 NOT set.
        if ((pkt.tcp_flags & 0x02) != 0 && (pkt.tcp_flags & 0x10) == 0) {
            syn_counts[pkt.src_ip]++;
            if (syn_counts[pkt.src_ip] >= SYN_FLOOD_THRESHOLD) {
                std::cout << "\n\033[1;31m[🚨 IDS ALERT] SYN Flood DoS Attack Detected! Source IP: " 
                          << pkt.src_ip << "\033[0m\n";
                syn_counts[pkt.src_ip] = 0; // Reset after alert
            }
        }
    }
};

// ... Print Packet Summary Function ...
void printPacketSummary(const ParsedPacket& pkt, int packet_num) {
    // We will make the output a bit more compact so alerts stand out!
    std::cout << "[Packet #" << packet_num << "] ";
    if (pkt.has_ip) {
        std::cout << pkt.src_ip << " -> " << pkt.dest_ip << " ";
    }
    if (pkt.has_tcp) {
        std::cout << "| TCP | Ports: " << pkt.src_port << "->" << pkt.dest_port 
                  << " | Flags: " << PacketParser::tcpFlagsToString(pkt.tcp_flags) << "\n";
    } else if (pkt.has_udp) {
        std::cout << "| UDP | Ports: " << pkt.src_port << "->" << pkt.dest_port << "\n";
    } else {
        std::cout << "| " << std::hex << pkt.ether_type << std::dec << "\n";
    }
}

void printUsage(const char* program_name) {
    std::cout << "Usage:\n";
    std::cout << "  Live capture:   " << program_name << " -i <interface> [max_packets]\n";
    std::cout << "  Offline file:   " << program_name << " -f <pcap_file> [max_packets]\n";
    std::cout << "  List interfaces:" << program_name << " -l\n";
}

int main(int argc, char* argv[]) {
    std::cout << "====================================\n";
    std::cout << "   NetShield: IDS Engine v3.0\n";
    std::cout << "====================================\n\n";
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    PcapReader reader;
    IntrusionDetector ids; // Initialize our new security engine

    if (mode == "-l") {
        std::cout << "Available network interfaces:\n";
        auto interfaces = PcapReader::getAvailableInterfaces();
        for (const auto& iface : interfaces) {
            std::cout << "  - " << iface << "\n";
        }
        return 0;
    }

    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string target = argv[2];
    int max_packets = (argc >= 4) ? std::stoi(argv[3]) : -1;

    if (mode == "-f") {
        if (!reader.openOffline(target)) return 1;
    } else if (mode == "-i") {
        if (!reader.openLive(target)) return 1;
    } else {
        printUsage(argv[0]);
        return 1;
    }
    
    std::cout << "\n--- Intrusion Detection Engine Running ---\n";
    
    RawPacket raw_packet;
    ParsedPacket parsed_packet;
    int packet_count = 0;
    
    while (true) {
        if (reader.readNextPacket(raw_packet)) {
            packet_count++;
            
            if (PacketParser::parse(raw_packet, parsed_packet)) {
                printPacketSummary(parsed_packet, packet_count);
                // Feed the packet into our IDS rules engine
                ids.analyze(parsed_packet); 
            }
            
            if (max_packets > 0 && packet_count >= max_packets) {
                break;
            }
        } else if (mode == "-f") {
            break;
        }
    }
    
    reader.close();
    return 0;
}