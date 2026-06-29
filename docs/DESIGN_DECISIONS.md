# Design Decisions

## Current architectural decisions

### Pipeline-based architecture

The repository uses a staged packet processing architecture with separate components for reading, parsing, dispatching, inspecting, deciding, and writing. This choice keeps each stage focused and makes the code easier to understand.

### Threaded worker model

The multi-threaded implementation uses load balancers and fast-path processors to distribute work. This was chosen to demonstrate concurrency and make the design more scalable than a single-threaded implementation.

### Flow-based connection tracking

Connections are modeled using a five-tuple and a connection state object. This makes it possible to track application classification and blocking decisions per flow rather than per packet.

### Rule-driven blocking

The engine evaluates simple blocking rules for IPs, apps, domains, and ports. This keeps the policy model lightweight while still demonstrating practical enforcement logic.

### Output-oriented design

Packets are forwarded or dropped based on inspection outcomes, and the accepted packets are written to a new PCAP file. This makes the engine easy to test and visualize using standard networking tools.

## Trade-offs

### Simplicity over completeness

The current implementation favors readability and educational clarity over full protocol coverage and production reliability.

### Concurrency over operational complexity

The threaded design demonstrates parallel processing, but it does not yet include the sophisticated scheduling, backpressure, and resilience mechanisms that a large-scale deployment would require.

### Lightweight rule engine over policy richness

The rule system is intentionally simple. It is easy to use but does not yet support complex policy expressions or dynamic updates.

## Potential improvements

- Introduce a formal configuration system.
- Add structured logging with clear component context.
- Improve parser robustness for more packet types.
- Introduce better metrics and reporting hooks.
- Add test coverage around packet handling and rules.
- Support hot reloading of rules and runtime policies.

## Future refactoring opportunities

The repository would benefit from a few targeted refactors over time:

- Consolidate repeated parsing logic and helper methods.
- Separate policy evaluation from packet processing more explicitly.
- Introduce interfaces or plugin hooks for inspection stages.
- Reduce tight coupling between the orchestrator and the worker modules.
- Provide a clearer state machine for connection lifecycle management.

## Summary

The current design is appropriate for a prototype or learning-oriented DPI engine. Its modularity and threaded structure provide a good base for future growth, but the next stage should focus on robustness, configurability, observability, and testing.
