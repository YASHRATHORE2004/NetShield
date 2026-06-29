# Roadmap

## Vision

The current repository is a strong foundation for a networking platform that can evolve from a prototype DPI engine into a production-grade traffic analysis and enforcement system.

## Near-term evolution

### Analytics Engine

Introduce a dedicated analytics layer to compute traffic trends, protocol distributions, top talkers, and anomaly summaries from processed packets and connection metadata.

### Threat Detection

Expand beyond simple rule blocking and add heuristic and signature-based threat detection for suspicious traffic patterns, repeated connection failures, and unusual protocol behavior.

### Configuration System

Add a formal configuration model that can be loaded from files or environment variables. This should support runtime updates for rules, worker counts, logging behavior, and policy settings.

### Logging

Implement structured logging with severity levels, component-specific tagging, file rotation, and optional console output. Logging should be available for packet handling, rule decisions, and runtime diagnostics.

### Benchmarking

Add benchmark harnesses to measure packet throughput, latency, memory usage, and rule evaluation overhead under representative traffic loads.

## Reporting and visualization

### JSON Reports

Support exporting inspection results, statistics, and summaries in JSON for downstream dashboards and automation workflows.

### CSV Reports

Add CSV export for connection summaries, blocked flows, and classification distributions.

### Dashboard

Create a simple dashboard interface that can display live or historical traffic insights, rule status, throughput, and classification results.

## Quality and engineering maturity

### Testing

Introduce a layered testing strategy with unit tests for parsing and rule logic, integration tests for the packet pipeline, and performance tests for throughput benchmarks.

### CI/CD

Add continuous integration and deployment automation for build validation, tests, artifact generation, and release packaging.

## Production-grade milestones

1. Harden the parser for broader protocol coverage.
2. Introduce a configuration-driven policy engine.
3. Add observability, logging, and reporting.
4. Improve concurrency management and scalability.
5. Add automated validation and release pipelines.
6. Prepare the system for live traffic capture and operational deployment.
