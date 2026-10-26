# RV32I C-Based Low-Level Systems Emulator

A lightweight, bare-metal compatible RISC-V RV32I instruction set architecture (ISA) emulator written from scratch in pure C. Designed for high-performance systems simulation, low-latency execution analysis, and computer architecture research.

## Features

* **Complete RV32I Base Instruction Set:** Supports arithmetic (`ADD`, `SUB`, `ADDI`), logical (`AND`, `OR`, `XOR`, `ANDI`, `ORI`), memory access (`LW`, `SW`), conditional branches (`BEQ`, `BNE`), and unconditional jumps (`JAL`).
* **Von Neumann Architecture Simulation:** Implements a linear, dynamically allocated byte-addressable memory model (`1 MB`) with strict bounds checking.
* **Little-Endian Memory Layout & Byte Assembly:** Manual assembly of 32-bit instructions from raw byte streams.
* **Zero Dependencies:** Written in standard C (`C99`) with zero external libraries, ensuring portability across bare-metal environments, Linux systems, and terminal emulators.
* **Register State Dumper:** Provides detailed architectural state inspection (`x0` through `x31`) post-execution.

---

## Architectural Overview

The emulator models the core components of a 32-bit RISC-V processor:
1. **CPU State (`CPU` struct):** Manages 32 general-purpose registers (`uint32_t`), the Program Counter (`PC`), and a pointer to the simulated memory space.
2. **Fetch-Decode-Execute Pipeline:**
   * **Fetch:** Retrieves 32-bit instructions from memory using little-endian byte reassembly.
   * **Decode:** Extracts instruction fields (`opcode`, `rd`, `funct3`, `rs1`, `rs2`, `funct7`) and handles immediate sign-extensions for I, S, B, and J type instructions.
   * **Execute:** Evaluates operations and updates register states while ensuring `x0` remains hardwired to zero.

---

## Code Structure

```text
.
├── main.c        # Core emulator logic, memory loader, and execution engine
└── README.md     # Project documentation and architecture guide
