# Hybrid Solidity Static Vulnerability Analysis Engine

A high-performance, hybrid static analysis tool engineered in C and Python to detect critical security vulnerabilities in Ethereum smart contracts. Designed for automated vulnerability assessment, abstract syntax tree (AST) pattern matching, and secure smart contract development.

## Features

* **Hybrid Architecture:** Combines a low-level, high-throughput C-based lexical stream scanner with a Python-powered rule execution and AST inspection engine.
* **Critical Vulnerability Detection:** Scans for prevalent smart contract security flaws including:
  * Reentrancy attacks (`CALL` value transfers before state updates)
  * Integer arithmetic overflows and underflows
  * Unchecked low-level calls (`call`, `delegatecall`)
  * Access control and authorization bypass vectors
* **Modular Rule Engine:** Extensible architecture allowing developers to define custom security patterns and heuristics.
* **Structured Output:** Generates machine-readable JSON security reports detailing vulnerability types, line numbers, and severity tiers.
* **Zero-Overhead Parsing:** Rapidly processes raw `.sol` source files with minimal memory footprint.

---

## Architectural Overview

The engine operates on a dual-tier pipeline:
1. **C Core (Lexical Analysis & High-Speed Ingestion):** Handles raw file I/O, memory mapping, and high-speed tokenization to filter out irrelevant whitespace and comment structures.
2. **Python Frontend (Pattern Matching & AST Heuristics):** Interfacing with the C core output, it constructs behavioral patterns, evaluates control flow graphs, and flags security violations against established vulnerability taxonomies.

---

## Code Structure

```text
.
├── core/           # C-based low-level stream parser and token scanner
├── engine/         # Python static analysis and rule-matching logic
├── tests/          # Vulnerable sample smart contracts for test benchmarking
└── README.md       # Project documentation and architecture guide
