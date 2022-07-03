# RISC-V Simulator

This program is a RISC-V simulator written by C++.  It is a simple RISC-V
simulator that can simulate simplified RV32I instructions.

此項目是由 C++ 程式碼編寫的 RISC-V 模擬器。它可以模擬簡化 RV32I 指令的模擬器。

## Architecture 架構
- tomasulo algorithm
- 4-bit two-level local adaptive predictor 四位元兩級局部預測

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

## How to Use 使用方法
Compile the code with `CMake` and run the executable file.

透過 `Cmake` 編譯程式并執行。

## Performance optimizations 性能優化
### Branch Predictor Performance 分支預測性能
|   Test Case    | Success Rate | Total CPU Clock (Predicted) | Total CPU Clock (Always taken) |
|:--------------:|:------------:|:---------------------------:|:------------------------------:|
|  array_test1   |    54.55%    |             254             |              268               |
|  array_test2   |    50.00%    |             299             |              311               |
|   basicopt1    |    98.64%    |           633032            |             914496             |
|   bulgarian    |    90.33%    |           446440            |             643017             |
|      expr      |    76.58%    |             914             |              965               |
|      gcd       |    63.33%    |             602             |              711               |
|     hanoi      |    97.10%    |           290233            |             327159             |
|    lvalue2     |    66.67%    |             57              |               64               |
|     magic      |    76.27%    |           725310            |             871425             |
| manyarguments  |    80.00%    |             68              |               88               |
|   multiarray   |    56.79%    |            2962             |              2841              |
|     naive      |     N/A      |             33              |               33               |
|       pi       |    81.49%    |          137659899          |           173119132            |
|     qsort      |    91.70%    |           1474485           |            1810504             |
|     queens     |    75.61%    |           899030            |            1048146             |
| statement_test |    62.87%    |            1413             |              1576              |
|   superloop    |    91.85%    |           645199            |            1744502             |
|      tak       |    70.34%    |           2622430           |            2622437             |

### Multiple ALU units 多計算單元
The simulator have 4 types of ALU units:
模擬程式執行器有 4 種計算單元:
- Add ALU
  加減法計算單元
- Shift ALU
  左右移轉計算單元
- Logic ALU
  邏輯計算單元
- Set ALU
  比較計算單元

Each kind of ALUs unit can have more than one ALU units.  The following table
shows the performance on multiple ALU units.

各類計算單元可以有多個計算單元。下表顯示多個計算單元的性能。

|   Test Case    | Total CPU Clock (2 * 4) | Total CPU Clock (1 * 4) |
|:--------------:|:-----------------------:|:-----------------------:|
|  array_test1   |           254           |           257           |
|  array_test2   |           299           |           302           |
|   basicopt1    |         633032          |         633122          |
|   bulgarian    |         446440          |         446968          |
|      expr      |           914           |           930           |
|      gcd       |           602           |           602           |
|     hanoi      |         290233          |         291349          |
|    lvalue2     |           57            |           58            |
|     magic      |         725310          |         729414          |
| manyarguments  |           68            |           69            |
|   multiarray   |          2962           |          2964           |
|     naive      |           33            |           33            |
|       pi       |        137659899        |        138222807        |
|     qsort      |         1474485         |         1476714         |
|     queens     |         899030          |         907405          |
| statement_test |          1413           |          1426           |
|   superloop    |         645199          |         754101          |
|      tak       |         2622430         |         2622430         |


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
