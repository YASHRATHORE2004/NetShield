# Modules

## Overview

This repository is organized into a small set of modules that each cover one area of responsibility in the DPI pipeline. The modules are intentionally separated so that parsing, rule evaluation, connection tracking, and threading can be understood independently.

## Core modules

### src/main.cpp

- Responsibility: Provides a basic entry point for the project.
- Important classes: No major class implementation is central here; it serves as a simple launcher for the DPI flow.
- Dependencies: Uses the core DPI engine and related headers.
- Future extension points: This would be a natural place to add CLI argument parsing, configuration loading, and mode selection.

### src/main_simple.cpp

- Responsibility: Provides a simpler single-threaded or simplified execution path.
- Important classes: No dedicated class here; it is more of an alternative demonstration entry point.
- Dependencies: Depends on the parser, reader, and basic DPI logic.
- Future extension points: Could be used as a reference baseline for testing and comparison against the multithreaded path.

### src/main_working.cpp

- Responsibility: Contains a working single-threaded implementation path for packet processing.
- Important classes: Uses the reader and parser directly.
- Dependencies: PCAP reading, parsing, and basic inspection helpers.
- Future extension points: This could become a compatibility path for regression testing or educational demos.

### src/main_dpi.cpp

- Responsibility: Provides another entry point focused on DPI behavior.
- Important classes: Leans on the core engine abstractions.
- Dependencies: Depends on the DPI engine and supporting components.
- Future extension points: Suitable for adding richer command-line or batch-processing modes.

### src/dpi_mt.cpp

- Responsibility: Represents the multithreaded processing implementation.
- Important classes: Uses the same core concepts as the main engine but in a threaded design.
- Dependencies: Depends on the load balancer, fast-path, queue, and tracking modules.
- Future extension points: This is a good place to evolve toward advanced scheduling, backpressure, and observability.

### src/dpi_engine.cpp

- Responsibility: Central orchestrator for the DPI pipeline.
- Important classes: DPIEngine.
- Dependencies: Uses the reader, parser, load balancer, fast-path processor, rule manager, and connection tracker modules.
- Future extension points: This is the natural integration point for configuration loading, metrics export, and lifecycle management.

### src/pcap_reader.cpp

- Responsibility: Reads raw PCAP files and exposes packet headers and payloads.
- Important classes: PcapReader.
- Dependencies: Standard file I/O and the PCAP header structures from the header.
- Future extension points: Add support for live capture, pcapng, packet filtering, and streaming input.

### src/packet_parser.cpp

- Responsibility: Parses Ethernet, IPv4, TCP, and UDP headers and extracts payload positions.
- Important classes: PacketParser.
- Dependencies: Uses portable byte-order utilities and the PCAP data structures.
- Future extension points: Support IPv6, ICMP, application-layer parsing, and deeper payload inspection.

### src/sni_extractor.cpp

- Responsibility: Extracts SNI values from TLS Client Hello packets and also supports HTTP host and DNS query extraction.
- Important classes: SNIExtractor, QUICSNIExtractor, HTTPHostExtractor, DNSExtractor.
- Dependencies: Uses byte parsing and simple protocol heuristics.
- Future extension points: Expand to full TLS parsing, QUIC/HTTP3 support, and richer domain classification.

### src/types.cpp

- Responsibility: Implements helpers for flow formatting and application type mapping.
- Important classes: FiveTuple, AppType-related helpers.
- Dependencies: No heavy external dependencies; mostly string and enum utilities.
- Future extension points: Add more advanced application fingerprinting and richer reporting helpers.

### src/rule_manager.cpp

- Responsibility: Stores and evaluates blocking rules for IPs, applications, domains, and ports.
- Important classes: RuleManager.
- Dependencies: Uses thread-safe containers and the types module.
- Future extension points: Introduce rule priority, time-based rules, policy groups, and configuration-driven rule sources.

### src/connection_tracker.cpp

- Responsibility: Maintains per-flow state, statistics, and classification state.
- Important classes: ConnectionTracker, GlobalConnectionTable.
- Dependencies: Depends on the types model and threading primitives.
- Future extension points: Add more advanced flow lifecycle tracking, session state machines, and distributed aggregation.

### src/load_balancer.cpp

- Responsibility: Distributes packets to fast-path workers using a consistent hash of the five-tuple.
- Important classes: LoadBalancer, LBManager.
- Dependencies: Uses the packet job and queue abstractions.
- Future extension points: Add weighted scheduling, dynamic worker scaling, and more sophisticated load-aware routing.

### src/fast_path.cpp

- Responsibility: Executes the main inspection and decision logic for each packet.
- Important classes: FastPathProcessor, FPManager.
- Dependencies: Depends on connection tracking, rule evaluation, SNI extraction, and the queue abstraction.
- Future extension points: This is the most obvious place to add anomaly detection, richer classification, and performance instrumentation.

## Header modules

### include/pcap_reader.h

- Responsibility: Declares the PCAP file reader and packet structures.
- Important classes: PcapReader, RawPacket, PcapGlobalHeader, PcapPacketHeader.
- Dependencies: Standard library and stream support.
- Future extension points: Add streaming and pcapng compatibility.

### include/packet_parser.h

- Responsibility: Declares protocol parsing structures and helpers.
- Important classes: PacketParser, ParsedPacket, protocol constants.
- Dependencies: Depends on pcap-related definitions.
- Future extension points: Expand to IPv6 and higher-layer protocol parsing.

### include/sni_extractor.h

- Responsibility: Declares the protocol-specific extractors used for classification.
- Important classes: SNIExtractor, QUICSNIExtractor, HTTPHostExtractor, DNSExtractor.
- Dependencies: Standard library and protocol parsing helpers.
- Future extension points: Add more robust TLS parsing and HTTP/3 support.

### include/types.h

- Responsibility: Defines shared types used across the engine.
- Important classes and enums: FiveTuple, AppType, ConnectionState, PacketAction, Connection, PacketJob, DPIStats.
- Dependencies: Core standard library support.
- Future extension points: Add richer metadata fields and serialization support.

### include/rule_manager.h

- Responsibility: Declares the rule engine interface.
- Important classes: RuleManager.
- Dependencies: Uses shared types and thread-safe collections.
- Future extension points: Add rule DSL, policy data model, and hot-reload support.

### include/connection_tracker.h

- Responsibility: Declares the connection state model and global aggregation helpers.
- Important classes: ConnectionTracker, GlobalConnectionTable.
- Dependencies: Uses the shared types model.
- Future extension points: Add export interfaces for analytics and distributed state.

### include/load_balancer.h

- Responsibility: Declares the load balancer abstractions.
- Important classes: LoadBalancer, LBManager.
- Dependencies: Depends on queues and packet job abstraction.
- Future extension points: Add adaptive routing and worker affinity.

### include/fast_path.h

- Responsibility: Declares the packet processing worker interface.
- Important classes: FastPathProcessor, FPManager.
- Dependencies: Uses connection tracking, rules, and queues.
- Future extension points: Add plugin-style processing stages and richer inspection hooks.

### include/thread_safe_queue.h

- Responsibility: Implements a reusable thread-safe queue for inter-thread communication.
- Important classes: ThreadSafeQueue.
- Dependencies: Standard threading primitives.
- Future extension points: Add metrics, bounded backpressure enhancements, and priority support.

### include/dpi_engine.h

- Responsibility: Declares the top-level orchestrator.
- Important classes: DPIEngine.
- Dependencies: Aggregates the rest of the engine modules.
- Future extension points: Add service lifecycle management, runtime plugins, and external API hooks.
