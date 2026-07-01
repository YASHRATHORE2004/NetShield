#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>  // For CSV Export
#include <chrono>   // For 5-second intervals
#include "pcap_reader.h"
#include "packet_parser.h"
#include "httplib.h" 

using namespace PacketAnalyzer;

// Global thread-safe stats for the API
std::atomic<int> total_packets_captured(0);
std::atomic<int> total_alerts_triggered(0);
std::mutex stats_mutex;
std::unordered_map<std::string, int> top_ips;

// ==========================================
// NEW: Machine Learning Feature Exporter
// ==========================================
struct MLFeatures {
    int total_packets = 0;
    int syn_count = 0;
    std::unordered_set<uint16_t> unique_ports;
};

class MLExporter {
private:
    std::unordered_map<std::string, MLFeatures> ip_features;
    std::chrono::steady_clock::time_point last_dump_time;
    std::mutex ml_mutex;

public:
    MLExporter() {
        last_dump_time = std::chrono::steady_clock::now();
        // Create CSV and write headers if starting fresh
        std::ofstream file("traffic_features.csv", std::ios::app);
        file << "timestamp,ip,total_packets,syn_count,unique_ports\n";
    }

    void aggregate(const ParsedPacket& pkt) {
        if (!pkt.has_ip) return;
        
        std::lock_guard<std::mutex> lock(ml_mutex);
        
        // 1. Extract Features
        auto& feat = ip_features[pkt.src_ip];
        feat.total_packets++;
        
        if (pkt.has_tcp) {
            feat.unique_ports.insert(pkt.dest_port);
            // Check for SYN flag (0x02)
            if ((pkt.tcp_flags & 0x02) != 0) {
                feat.syn_count++;
            }
        }

        // 2. Check if 5 seconds have passed to export data
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_dump_time).count() >= 5) {
            exportToCSV();
            last_dump_time = now;
        }
    }

private:
    void exportToCSV() {
        std::ofstream file("traffic_features.csv", std::ios::app);
        std::time_t t = std::time(nullptr);
        
        for (const auto& pair : ip_features) {
            // Only log IPs that are actually sending traffic
            if (pair.second.total_packets > 0) {
                file << t << "," 
                     << pair.first << ","
                     << pair.second.total_packets << ","
                     << pair.second.syn_count << ","
                     << pair.second.unique_ports.size() << "\n";
            }
        }
        ip_features.clear(); // Reset for the next 5-second window
    }
};

// ==========================================
// Intrusion Detector (Static Rules)
// ==========================================
class IntrusionDetector {
private:
    std::unordered_map<std::string, int> syn_counts;
    std::unordered_map<std::string, std::unordered_set<uint16_t>> port_scans;
    const int SYN_FLOOD_THRESHOLD = 20;
    const int PORT_SCAN_THRESHOLD = 10;

public:
    void analyze(const ParsedPacket& pkt) {
        if (pkt.has_ip) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            top_ips[pkt.src_ip]++;
        }

        if (!pkt.has_ip || !pkt.has_tcp) return;

        port_scans[pkt.src_ip].insert(pkt.dest_port);
        if (port_scans[pkt.src_ip].size() >= PORT_SCAN_THRESHOLD) {
            std::cout << "\n\033[1;31m[🚨 IDS ALERT] Port Scan Detected! IP: " << pkt.src_ip << "\033[0m\n";
            total_alerts_triggered++;
            port_scans[pkt.src_ip].clear(); 
        }

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
        res.set_header("Access-Control-Allow-Origin", "*"); 
        
        std::string json = "{";
        json += "\"total_packets\": " + std::to_string(total_packets_captured) + ",";
        json += "\"total_alerts\": " + std::to_string(total_alerts_triggered) + ",";
        
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

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -i <interface>\n";
}

int main(int argc, char* argv[]) {
    std::cout << "====================================\n";
    std::cout << " NetShield: ML Pipeline Engine v5.0\n";
    std::cout << "====================================\n\n";
    
    if (argc < 3) { printUsage(argv[0]); return 1; }

    std::string mode = argv[1];
    std::string target = argv[2];
    PcapReader reader;
    IntrusionDetector ids;
    MLExporter ml_exporter; // Initialize ML Exporter

    if (mode == "-i") {
        if (!reader.openLive(target)) return 1;
    } else { return 1; }
    
    std::thread api_thread(startAPIServer);
    api_thread.detach(); 

    std::cout << "\n--- Engine Running (ML Data Logging Active) ---\n";
    
    RawPacket raw_packet;
    ParsedPacket parsed_packet;
    
    while (true) {
        if (reader.readNextPacket(raw_packet)) {
            total_packets_captured++;
            
            if (PacketParser::parse(raw_packet, parsed_packet)) {
                if (total_packets_captured % 100 == 0) {
                    std::cout << "\r[Status] Processed " << total_packets_captured << " packets..." << std::flush;
                }
                
                // 1. Check static IDS rules
                ids.analyze(parsed_packet); 
                
                // 2. Feed data into ML Pipeline
                ml_exporter.aggregate(parsed_packet);
            }
        }
    }
    
    reader.close();
    return 0;
}