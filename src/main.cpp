#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <atomic>
#include <mutex>
#include "pcap_reader.h"
#include "packet_parser.h"
#include "httplib.h" // New HTTP Library

using namespace PacketAnalyzer;

// Global thread-safe stats for the API
std::atomic<int> total_packets_captured(0);
std::atomic<int> total_alerts_triggered(0);
std::mutex stats_mutex;
std::unordered_map<std::string, int> top_ips;

class IntrusionDetector {
private:
    std::unordered_map<std::string, int> syn_counts;
    std::unordered_map<std::string, std::unordered_set<uint16_t>> port_scans;
    const int SYN_FLOOD_THRESHOLD = 20;
    const int PORT_SCAN_THRESHOLD = 10;

public:
    void analyze(const ParsedPacket& pkt) {
        // 1. TRACK ALL IP TRAFFIC FIRST (Fix for the empty JSON array)
        if (pkt.has_ip) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            top_ips[pkt.src_ip]++;
        }

        // 2. ONLY check TCP packets for the IDS attacks
        if (!pkt.has_ip || !pkt.has_tcp) return;

        // Detect Port Scans
        port_scans[pkt.src_ip].insert(pkt.dest_port);
        if (port_scans[pkt.src_ip].size() >= PORT_SCAN_THRESHOLD) {
            std::cout << "\n\033[1;31m[🚨 IDS ALERT] Port Scan Detected! IP: " << pkt.src_ip << "\033[0m\n";
            total_alerts_triggered++;
            port_scans[pkt.src_ip].clear(); 
        }

        // Detect SYN Floods
        if ((pkt.tcp_flags & 0x02) != 0 && (pkt.tcp_flags & 0x10) == 0) {
            syn_counts[pkt.src_ip]++;
            if (syn_counts[pkt.src_ip] >= SYN_FLOOD_THRESHOLD) {
                std::cout << "\n\033[1;31m[🚨 IDS ALERT] SYN Flood DoS Attack Detected! Source IP: " << pkt.src_ip << "\033[0m\n";
                total_alerts_triggered++;
                syn_counts[pkt.src_ip] = 0; 
            }
        }
    }
};

void startAPIServer() {
    httplib::Server svr;

    svr.Get("/stats", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*"); // Allow frontend connections
        
        std::string json = "{";
        json += "\"total_packets\": " + std::to_string(total_packets_captured) + ",";
        json += "\"total_alerts\": " + std::to_string(total_alerts_triggered) + ",";
        
        // Safely extract top IPs
        json += "\"active_ips\": [";
        {
            std::lock_guard<std::mutex> lock(stats_mutex);
            bool first = true;
            for (const auto& pair : top_ips) {
                if (!first) json += ",";
                json += "{\"" + pair.first + "\": " + std::to_string(pair.second) + "}";
                first = false;
            }
        }
        json += "]";
        json += "}";

        res.set_content(json, "application/json");
    });

    std::cout << "[API] Listening for dashboard connections on http://localhost:8080/stats\n";
    svr.listen("0.0.0.0", 8080);
}

// ... [Keep printUsage function here] ...
void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -i <interface>\n";
}

int main(int argc, char* argv[]) {
    std::cout << "====================================\n";
    std::cout << " NetShield: Live Engine & API v4.0\n";
    std::cout << "====================================\n\n";
    
    if (argc < 3) { printUsage(argv[0]); return 1; }

    std::string mode = argv[1];
    std::string target = argv[2];
    PcapReader reader;
    IntrusionDetector ids;

    if (mode == "-i") {
        if (!reader.openLive(target)) return 1;
    } else { return 1; }
    
    // Launch the API Server on a background thread
    std::thread api_thread(startAPIServer);
    api_thread.detach(); // Let it run independently

    std::cout << "\n--- Engine Running ---\n";
    
    RawPacket raw_packet;
    ParsedPacket parsed_packet;
    
    while (true) {
        if (reader.readNextPacket(raw_packet)) {
            total_packets_captured++;
            
            if (PacketParser::parse(raw_packet, parsed_packet)) {
                // Print minimal output to console so it doesn't freeze
                if (total_packets_captured % 100 == 0) {
                    std::cout << "\r[Status] Processed " << total_packets_captured << " packets..." << std::flush;
                }
                ids.analyze(parsed_packet); 
            }
        }
    }
    
    reader.close();
    return 0;
}