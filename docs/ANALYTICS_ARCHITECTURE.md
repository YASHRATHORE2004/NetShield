# Analytics Architecture

This document describes the data model layer for NetShield's analytics module.

## Data Models

### AnalyticsTypes.h

This header file defines reusable enums and type aliases used throughout the analytics module to ensure consistency and avoid magic numbers. It includes:

- `enum class ProtocolType`: Defines different network protocols (e.g., TCP, UDP, ICMP, UNKNOWN).
- `enum class PacketDirection`: Specifies the direction of a packet (e.g., INBOUND, OUTBOUND, UNKNOWN).
- `using Port = uint16_t`: Alias for port numbers.
- `using PacketSize = uint32_t`: Alias for packet sizes.
- `using Timestamp = uint64_t`: Alias for timestamps.
- `using IPAddress = std::string`: Alias for IP addresses.
- `using DomainName = std::string`: Alias for domain names.

### IPStatistics.h

This class is responsible for maintaining statistics on a per-IP basis. It provides the following functionalities:

- `void increment(const IPAddress& ip)`: Increments the count for a given IP address.
- `uint32_t getCount(const IPAddress& ip) const`: Returns the current count for a given IP address.
- `std::vector<std::pair<IPAddress, uint32_t>> getTopN(size_t n) const`: Returns the top N IP addresses by count.
- `void clear()`: Clears all collected IP statistics.

Internally, it uses an `std::unordered_map` to store IP addresses and their corresponding counts.

### PortStatistics.h

This class tracks statistics related to destination ports. It offers the following methods:

- `void increment(Port port)`: Increments the count for a given port.
- `uint32_t getCount(Port port) const`: Returns the current count for a given port.
- `std::vector<std::pair<Port, uint32_t>> getTopN(size_t n) const`: Returns the top N ports by count.
- `void clear()`: Clears all collected port statistics.

It utilizes an `std::unordered_map` to store port numbers and their counts.

### DomainStatistics.h

This class is designed to track statistics for hostnames (domains). It provides these functionalities:

- `void increment(const DomainName& domain)`: Increments the count for a given domain. Empty strings are ignored and domains are normalized to lowercase.
- `uint32_t getCount(const DomainName& domain) const`: Returns the current count for a given domain.
- `std::vector<std::pair<DomainName, uint32_t>> getTopN(size_t n) const`: Returns the top N domains by count.
- `void clear()`: Clears all collected domain statistics.

An `std::unordered_map` is used internally to store domain names and their counts.

## Statistics Layer

The statistics classes (ProtocolStatistics, IPStatistics, PortStatistics, DomainStatistics) are designed to be independent from the `AnalyticsEngine` for several reasons:

- **Modularity and Reusability**: This separation promotes a modular design, allowing each statistics class to be developed, tested, and maintained independently. It also makes these classes reusable in other parts of the system or in different analytics contexts without being coupled to a specific engine implementation.
- **Flexibility in Integration**: The independence allows for flexible integration with various `AnalyticsEngine` designs. Different engines might have different requirements for how they collect and process data, and by keeping statistics classes separate, the engine can choose how to utilize them.
- **Separation of Concerns**: Each class has a single responsibility (e.g., `IPStatistics` only deals with IP address counts). The `AnalyticsEngine` would then be responsible for orchestrating how these statistics are updated based on parsed packets, rather than having the statistics logic embedded within the engine itself.
- **Testability**: Independent classes are easier to unit test. Each statistics class can be tested in isolation, ensuring its correctness without needing to set up a full `AnalyticsEngine` environment.

### ProtocolStatistics.h

This class tracks network protocol statistics. It provides the following functionalities:

- `void increment(ProtocolType protocol)`: Increments the count for a given protocol.
- `void decrement(ProtocolType protocol)`: Decrements the count for a given protocol, if supported and count is greater than 0.
- `void reset()`: Clears all collected protocol statistics.
- `uint32_t getCount(ProtocolType protocol) const`: Returns the current count for a given protocol.
- `std::unordered_map<ProtocolType, uint32_t> getAllStatistics() const`: Returns all collected protocol statistics.

Internally, it uses an `std::unordered_map` to store protocol types and their corresponding counts.
