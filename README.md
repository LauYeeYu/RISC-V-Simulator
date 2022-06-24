# RISC-V Simluator

This program is a RISC-V simulator written by C++.  It is a simple RISC-V
simulator that can simulate simplified RV32I instructions.

此項目是由 C++ 程式碼編寫的 RISC-V 模擬器。它可以模擬簡化 RV32I 指令的模擬器。

## Supported Instructions 支援指令
| Instruction | Description                         |
|:-----------:|-------------------------------------|
|     LUI     | Load Upper Immediate                |
|    AUIPC    | Add Upper Immediate to PC           |
|     JAL     | Jump and Link                       |
|    JALR     | Jump and Link Register              |
|     BEQ     | Branch if Equal                     |
|     BNE     | Branch if Not Equal                 |
|     BLT     | Branch if Less Than                 |
|     BGE     | Branch if Greater Than              |
|    BLTU     | Branch if Less Than Unsigned        |
|    BGEU     | Branch if Greater Than Unsigned     |
|     LB      | Load Byte                           |
|     LH      | Load Halfword                       |
|     LW      | Load Word                           |
|     LBU     | Load Byte Unsigned                  |
|     LHU     | Load Halfword Unsigned              |
|     SB      | Store Byte                          |
|     SH      | Store Halfword                      |
|     SW      | Store Word                          |
|    ADDI     | Add Immediate                       |
|    SLTI     | Set on Less Than Immediate          |
|    SLTIU    | Set on Less Than Immediate Unsigned |
|    XORI     | Exclusive OR Immediate              |
|     ORI     | OR Immediate                        |
|    ANDI     | AND Immediate                       |
|    SLLI     | Shift Left Logical                  |
|    SRLI     | Shift Right Logical                 |
|    SRAI     | Shift Right Arithmetic              |
|     ADD     | Add                                 |
|     SUB     | Subtract                            |
|     SLL     | Shift Left Logical                  |
|     SLT     | Set on Less Than                    |
|    SLTU     | Set on Less Than Unsigned           |
|     XOR     | Exclusive OR                        |
|     SRL     | Shift Right Logical                 |
|     SRA     | Shift Right Arithmetic              |
|     OR      | OR                                  |
|     AND     | AND                                 |

### Instruction Format
```text
31           25 24         20 19         15 14 12 11          7 6       0
+--------------+-------------+-------------+-----+-------------+---------+
|                   imm[31:12]                   |     rd      | 0110111 | LUI
|                   imm[31:12]                   |     rd      | 0010111 | AUIPC
|             imm[20|10:1|11|19:12]              |     rd      | 1101111 | JAL
|         imm[11:0]          |     rs1     | 000 |     rd      | 1100111 | JALR 
| imm[12|10:5] |     rs2     |     rs1     | 000 | imm[4:1|11] | 1100011 | BEQ
| imm[12|10:5] |     rs2     |     rs1     | 001 | imm[4:1|11] | 1100011 | BNE
| imm[12|10:5] |     rs2     |     rs1     | 100 | imm[4:1|11] | 1100011 | BLT
| imm[12|10:5] |     rs2     |     rs1     | 101 | imm[4:1|11] | 1100011 | BGE
| imm[12|10:5] |     rs2     |     rs1     | 110 | imm[4:1|11] | 1100011 | BLTU
| imm[12|10:5] |     rs2     |     rs1     | 111 | imm[4:1|11] | 1100011 | BGEU
|         imm[11:0]          |     rs1     | 000 |     rd      | 0000011 | LB
|         imm[11:0]          |     rs1     | 001 |     rd      | 0000011 | LH
|         imm[11:0]          |     rs1     | 010 |     rd      | 0000011 | LW
|         imm[11:0]          |     rs1     | 100 |     rd      | 0000011 | LBU
|         imm[11:0]          |     rs1     | 101 |     rd      | 0000011 | LHU
|  imm[11:5]   |     rs2     |     rs1     | 000 |  imm[4:0]   | 0100011 | SB
|  imm[11:5]   |     rs2     |     rs1     | 001 |  imm[4:0]   | 0100011 | SH
|  imm[11:5]   |     rs2     |     rs1     | 010 |  imm[4:0]   | 0100011 | SW
|         imm[11:0]          |     rs1     | 000 |     rd      | 0010011 | ADDI
|         imm[11:0]          |     rs1     | 010 |     rd      | 0010011 | SLTI
|         imm[11:0]          |     rs1     | 011 |     rd      | 0010011 | SLTIU
|         imm[11:0]          |     rs1     | 100 |     rd      | 0010011 | XORI
|         imm[11:0]          |     rs1     | 110 |     rd      | 0010011 | ORI
|         imm[11:0]          |     rs1     | 111 |     rd      | 0010011 | ANDI
|   0000000    |    shamt    |     rs1     | 001 |     rd      | 0010011 | SLLI
|   0000000    |    shamt    |     rs1     | 101 |     rd      | 0010011 | SRLI
|   0100000    |    shamt    |     rs1     | 101 |     rd      | 0010011 | SRAI
|   0000000    |     rs2     |     rs1     | 000 |     rd      | 0110011 | ADD
|   0100000    |     rs2     |     rs1     | 000 |     rd      | 0110011 | SUB
|   0000000    |     rs2     |     rs1     | 001 |     rd      | 0110011 | SLL
|   0000000    |     rs2     |     rs1     | 010 |     rd      | 0110011 | SLT
|   0000000    |     rs2     |     rs1     | 011 |     rd      | 0110011 | SLTU
|   0000000    |     rs2     |     rs1     | 100 |     rd      | 0110011 | XOR
|   0000000    |     rs2     |     rs1     | 101 |     rd      | 0110011 | SRL
|   0100000    |     rs2     |     rs1     | 101 |     rd      | 0110011 | SRA
|   0000000    |     rs2     |     rs1     | 110 |     rd      | 0110011 | OR
|   0000000    |     rs2     |     rs1     | 111 |     rd      | 0110011 | AND
```

## License 許可證

RISC-V Simulator

Copyright (C) 2022  Lau Yee-Yu

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
