#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE (1024 * 1024)

typedef struct {
    uint32_t regs[32];
    uint32_t pc;
    uint8_t *memory;
} CPU;

void cpu_init(CPU *cpu) {
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->pc = 0x00000000;
    cpu->memory = (uint8_t *)malloc(MEMORY_SIZE);
    if (!cpu->memory) {
        fprintf(stderr, "Error: Failed to allocate memory.\n");
        exit(1);
    }
    memset(cpu->memory, 0, MEMORY_SIZE);
}

void cpu_free(CPU *cpu) {
    if (cpu->memory) {
        free(cpu->memory);
    }
}

void cpu_dump_registers(CPU *cpu) {
    printf("\n=== Register Dump (PC: 0x%08X) ===\n", cpu->pc);
    for (int i = 0; i < 32; i += 4) {
        printf("x%02d: 0x%08X  x%02d: 0x%08X  x%02d: 0x%08X  x%02d: 0x%08X\n",
               i, cpu->regs[i],
               i+1, cpu->regs[i+1],
               i+2, cpu->regs[i+2],
               i+3, cpu->regs[i+3]);
    }
    printf("===================================\n");
}

void cpu_load_program(CPU *cpu, const uint32_t *program, size_t size) {
    for (size_t i = 0; i < size; i++) {
        uint32_t inst = program[i];
        cpu->memory[i * 4]     = inst & 0xFF;
        cpu->memory[i * 4 + 1] = (inst >> 8) & 0xFF;
        cpu->memory[i * 4 + 2] = (inst >> 16) & 0xFF;
        cpu->memory[i * 4 + 3] = (inst >> 24) & 0xFF;
    }
}

uint32_t cpu_fetch(CPU *cpu) {
    if (cpu->pc + 3 >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Memory access out of bounds at PC: 0x%08X\n", cpu->pc);
        exit(1);
    }
    uint32_t instruction = cpu->memory[cpu->pc] |
                          (cpu->memory[cpu->pc + 1] << 8) |
                          (cpu->memory[cpu->pc + 2] << 16) |
                          (cpu->memory[cpu->pc + 3] << 24);
    return instruction;
}

void cpu_execute(CPU *cpu, uint32_t inst) {
    uint8_t opcode = inst & 0x7F;
    uint8_t rd     = (inst >> 7) & 0x1F;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1    = (inst >> 15) & 0x1F;
    uint8_t rs2    = (inst >> 20) & 0x1F;
    uint8_t funct7 = (inst >> 25) & 0x7F;

    uint32_t next_pc = cpu->pc + 4;

    switch (opcode) {
        case 0x33:
            if (funct3 == 0x0 && funct7 == 0x00) {
                cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
            } else if (funct3 == 0x0 && funct7 == 0x20) {
                cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
            } else if (funct3 == 0x7 && funct7 == 0x00) {
                cpu->regs[rd] = cpu->regs[rs1] & cpu->regs[rs2];
            } else if (funct3 == 0x6 && funct7 == 0x00) {
                cpu->regs[rd] = cpu->regs[rs1] | cpu->regs[rs2];
            } else if (funct3 == 0x4 && funct7 == 0x00) {
                cpu->regs[rd] = cpu->regs[rs1] ^ cpu->regs[rs2];
            }
            break;

        case 0x13:
            if (funct3 == 0x0) {
                int32_t imm = (int32_t)inst >> 20;
                cpu->regs[rd] = cpu->regs[rs1] + imm;
            } else if (funct3 == 0x7) {
                int32_t imm = (int32_t)inst >> 20;
                cpu->regs[rd] = cpu->regs[rs1] & imm;
            } else if (funct3 == 0x6) {
                int32_t imm = (int32_t)inst >> 20;
                cpu->regs[rd] = cpu->regs[rs1] | imm;
            }
            break;

        case 0x03:
            if (funct3 == 0x2) {
                int32_t imm = (int32_t)inst >> 20;
                uint32_t addr = cpu->regs[rs1] + imm;
                if (addr + 3 < MEMORY_SIZE) {
                    cpu->regs[rd] = cpu->memory[addr] |
                                   (cpu->memory[addr + 1] << 8) |
                                   (cpu->memory[addr + 2] << 16) |
                                   (cpu->memory[addr + 3] << 24);
                } else {
                    fprintf(stderr, "Error: Load memory out of bounds at 0x%08X\n", addr);
                    exit(1);
                }
            }
            break;

        case 0x23:
            if (funct3 == 0x2) {
                int32_t imm = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
                if (imm & 0x800) imm |= 0xFFFFF000;
                uint32_t addr = cpu->regs[rs1] + imm;
                if (addr + 3 < MEMORY_SIZE) {
                    cpu->memory[addr]     = cpu->regs[rs2] & 0xFF;
                    cpu->memory[addr + 1] = (cpu->regs[rs2] >> 8) & 0xFF;
                    cpu->memory[addr + 2] = (cpu->regs[rs2] >> 16) & 0xFF;
                    cpu->memory[addr + 3] = (cpu->regs[rs2] >> 24) & 0xFF;
                } else {
                    fprintf(stderr, "Error: Store memory out of bounds at 0x%08X\n", addr);
                    exit(1);
                }
            }
            break;

        case 0x63: {
            int32_t imm = (((inst >> 31) & 0x1) << 12) |
                          (((inst >> 25) & 0x3F) << 5) |
                          (((inst >> 8) & 0xF) << 1) |
                          (((inst >> 7) & 0x1) << 11);
            if (imm & 0x1000) imm |= 0xFFFFE000;

            if (funct3 == 0x0) {
                if (cpu->regs[rs1] == cpu->regs[rs2]) next_pc = cpu->pc + imm;
            } else if (funct3 == 0x1) {
                if (cpu->regs[rs1] != cpu->regs[rs2]) next_pc = cpu->pc + imm;
            }
            break;
        }

        case 0x6F: {
            int32_t imm = (((inst >> 31) & 0x1) << 20) |
                          (((inst >> 12) & 0xFF) << 12) |
                          (((inst >> 20) & 0x1) << 11) |
                          (((inst >> 21) & 0x3FF) << 1);
            if (imm & 0x100000) imm |= 0xFFE00000;

            cpu->regs[rd] = cpu->pc + 4;
            next_pc = cpu->pc + imm;
            break;
        }

        default:
            fprintf(stderr, "Error: Unknown opcode 0x%02X at PC 0x%08X\n", opcode, cpu->pc);
            exit(1);
    }

    cpu->regs[0] = 0;
    cpu->pc = next_pc;
}

int main() {
    CPU cpu;
    cpu_init(&cpu);

    printf("RISC-V RV32I Emulator Core Initialized.\n");

    // Test program (Machine Instructions)
    uint32_t program[] = {
        0x00A00093, // addi x1, x0, 10
        0x01400113, // addi x2, x0, 20
        0x002081B3, // add  x3, x1, x2
        0x00302023, // sw   x3, 0(x0)
        0x00002203  // lw   x4, 0(x0)
    };

    size_t prog_size = sizeof(program) / sizeof(program[0]);
    cpu_load_program(&cpu, program, prog_size);

    for (size_t i = 0; i < prog_size; i++) {
        uint32_t inst = cpu_fetch(&cpu);
        cpu_execute(&cpu, inst);
    }

    cpu_dump_registers(&cpu);

    cpu_free(&cpu);
    return 0;
}
