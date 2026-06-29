# Architecture

## High-level architecture

This repository implements a lightweight Deep Packet Inspection (DPI) pipeline for processing PCAP traffic and making simple forwarding or dropping decisions based on parsed packet metadata and rule evaluation.

The design centers on a staged pipeline:

1. A PCAP reader loads network traffic from disk.
2. A parser extracts Ethernet, IPv4, TCP/UDP, and payload information.
3. A load-balancer layer distributes packets to worker threads.
4. Fast-path processors inspect connection state, classify traffic, evaluate rules, and decide whether to forward or drop packets.
5. An output writer emits the forwarded packets to a new PCAP file.

## Packet processing pipeline

The runtime flow is:

- Input PCAP -> Reader
- Reader -> PacketParser -> PacketJob creation
- PacketJob -> LoadBalancer -> FastPathProcessor
- FastPathProcessor -> RuleManager and ConnectionTracker -> Output decision
- Forwarded packets -> Output writer

## Component diagram

```text
+------------------+
| PCAP Input File  |
+--------+---------+
         |
         v
+------------------+
| PcapReader       |
+--------+---------+
         |
         v
+------------------+
| PacketParser     |
+--------+---------+
         |
         v
+------------------+
| DPIEngine        |
| - reader thread  |
| - LB threads     |
| - FP threads     |
| - output thread  |
+--------+---------+
         |
         +--------+-------------------+
                  |                   |
                  v                   v
      +-------------------+   +-------------------+
      | LoadBalancer      |   | RuleManager       |
      | (threaded)        |   | (blocking rules) |
      +--------+----------+   +-------------------+
               |
               v
      +-------------------+
      | FastPathProcessor |
      | (threaded)        |
      +--------+----------+
               |
               v
      +-------------------+
      | ConnectionTracker |
      +--------+----------+
               |
               v
      +-------------------+
      | Output Writer     |
      +-------------------+
```

## Thread interaction

The architecture uses a producer-consumer model with multiple worker threads:

- Reader thread: Reads the input PCAP and creates packet jobs.
- Load balancer threads: Receive jobs from the reader and route them based on a hash of the five-tuple.
- Fast-path processor threads: Process packets, update flow state, inspect payload, and enforce rules.
- Output thread: Writes forwarded packets to the output PCAP.

This design allows parallel inspection of packets, but it remains a prototype-oriented implementation rather than a fully hardened production pipeline.

## Data flow

Packet data travels through these main data structures:

- RawPacket: Raw bytes from the PCAP reader.
- ParsedPacket: Normalized protocol information from the parser.
- PacketJob: Enriched packet representation passed between threads.
- FiveTuple: Flow identity used for hashing and connection tracking.
- Connection: Per-flow state and classification information.
- DPIStats: Aggregated runtime statistics.

## Main execution path

The main execution path begins in the DPI engine entry points and proceeds as follows:

1. The engine is initialized with configuration values for the number of load balancers and fast-path workers.
2. The rule manager, fast-path manager, load-balancer manager, and global connection table are constructed.
3. Processing starts by launching the output thread, fast-path threads, load balancer threads, and the reader thread.
4. The reader parses each packet and pushes it into a load-balancer queue.
5. Each load balancer selects a fast-path worker using a hash of the flow tuple.
6. Each fast-path worker updates connection state, attempts classification from SNI or HTTP data, evaluates rules, and decides to forward or drop the packet.
7. Forwarded packets are queued for the output thread and written to disk.

## Notes on the current design

The present implementation is intentionally modular and uses clear separation between parsing, rule evaluation, connection tracking, and dispatch. The main architectural pattern is a threaded packet pipeline with shared components and per-worker state.
