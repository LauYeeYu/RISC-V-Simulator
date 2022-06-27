// RISC-V Simulator
// Copyright (C) 2022 Lau Yee-Yu
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "instructions.h"

#include <cassert>

#include "type.h"

namespace {

ByteType GetOpCode(WordType instruction) { return instruction & 0b0110111; }

SizeType GetDestinationRegister(WordType instruction) { return (instruction >>  7) & 0b11111; }
SizeType GetRegister1          (WordType instruction) { return (instruction >> 15) & 0b11111; }
SizeType GetRegister2          (WordType instruction) { return (instruction >> 20) & 0b11111; }

WordType SignedExtend(WordType value) {
    return (value & 0x800) ? (value | 0xFFFFF000) : value;
}

/// 31:12 imm[31:12]
WordType GetUpperImmediate(WordType instruction) { return instruction & 0xFFFFF000; }

/// 31:20 imm[11:0]
WordType GetUnsignedLowerImmediate(WordType instruction) { return (instruction >> 20) & 0xFFF; }

WordType GetSignedLowerImmediate(WordType instruction) {
    return SignedExtend(GetUnsignedLowerImmediate(instruction));
}

/// 31:12 imm[20|10:1|11|19:12]
WordType GetJALImmediate(WordType instruction) {
    WordType immediate = instruction & 0xFF000; // 19:12
    immediate |= (instruction >> 9) & 0x800; // 11
    immediate |= (instruction >> 20) & 0b11111111110; // 10:1
    immediate |= (instruction >> 11) & 0x100000; // 20
    return SignedExtend(immediate);
}

/// 31:25 imm[12|10:5] 11:7 imm[4:1|11]
WordType GetBranchImmediate(WordType instruction) {
    WordType immediate = (instruction >> 7) & 0b11110; // 4:1
    immediate |= (instruction << 4) & 0x800; // 11
    immediate |= (instruction >> 20) & 0x7E0; // 10:5
    immediate |= (instruction >> 19) & 0x800; // 12
    return SignedExtend(immediate);
}

/// 31:25 imm[11:5] 11:7 imm[4:0]
WordType GetStoreImmediate(WordType instruction) {
    WordType immediate = (instruction >> 7) & 0b11111; // 4:0
    immediate |= (instruction >> 20) & 0xFE0; // 10:5
    return SignedExtend(immediate);
}

/// 24:20 shift amount
WordType GetShiftAmount(WordType instruction) { return (instruction >> 7) & 0b11111; }

/// 14:12
WordType GetFunction3(WordType instruction) { return (instruction >> 12) & 0b111; }

/// 31:25
WordType GetFunction7(WordType instruction) { return (instruction >> 25) & 0b1111111; }

Instruction GetBranchInstruction(WordType instruction) {
    WordType function3 = GetFunction3(instruction);
    switch (function3) {
        case 0b000: // BEQ
            return Instruction::BEQ;
        case 0b001: // BNE
            return Instruction::BNE;
        case 0b100: // BLT
            return Instruction::BLT;
        case 0b101: // BGE
            return Instruction::BGE;
        case 0b110: // BLTU
            return Instruction::BLTU;
        case 0b111: // BGEU
            return Instruction::BGEU;
        default:
            assert(false);
    }
}

Instruction GetLoadInstruction(WordType instruction) {// LB & LH & LW & LBU & LHU
    WordType function3 = GetFunction3(instruction);
    switch (function3) {
        case 0b000: // LB
            return Instruction::LB;
        case 0b001: // LH
            return Instruction::LH;
        case 0b010: // LW
            return Instruction::LW;
        case 0b100: // LBU
            return Instruction::LBU;
        case 0b101: // LHU
            return Instruction::LHU;
        default:
            assert(false);
    }
}

Instruction GetStoreInstruction(WordType instruction) {
    WordType function3 = GetFunction3(instruction);
    switch (function3) {
        case 0b000: // SB
            return Instruction::SB;
        case 0b001: // SH
            return Instruction::SH;
        case 0b010: // SW
            return Instruction::SW;
        default:
            assert(false);
    }
}

Instruction GetImmediateInstruction(WordType instruction) {
    WordType function3 = GetFunction3(instruction);
    WordType function7 = GetFunction7(instruction);
    switch (function3) {
        case 0b000: // ADDI
            return Instruction::ADDI;
        case 0b001: // SLLI
            return Instruction::SLLI;
        case 0b010: // SLTI
            return Instruction::SLTI;
        case 0b011: // SLTIU
            return Instruction::SLTIU;
        case 0b100: // XORI
            return Instruction::XORI;
        case 0b101: // SRLI & SRAI
            switch (function7) {
                case 0b0000000: // SRLI
                    return Instruction::SRLI;
                case 0b0100000: // SRAI
                    return Instruction::SRAI;
                default:
                    assert(false);
            }
        case 0b110: // SRAI
            return Instruction::SRAI;
        case 0b111: // ORI
            return Instruction::ORI;
        default:
            assert(false);
    }
}

Instruction GetArithmeticInstruction(WordType instruction) {
    WordType function3 = GetFunction3(instruction);
    WordType function7 = GetFunction7(instruction);
    switch (function3) {
        case 0b000: // ADD & SUB
            switch (function7) {
                case 0b0000000: // ADD
                    return Instruction::ADD;
                case 0b0100000: // SUB
                    return Instruction::SUB;
                default:
                    assert(false);
            }
        case 0b001: // SLL
            return Instruction::SLL;
        case 0b010: // SLT
            return Instruction::SLT;
        case 0b011: // SLTU
            return Instruction::SLTU;
        case 0b100: // XOR
            return Instruction::XOR;
        case 0b101: // SRL & SRA
            switch (function7) {
                case 0b0000000: // SRL
                    return Instruction::SRL;
                case 0b0100000: // SRA
                    return Instruction::SRA;
                default:
                    assert(false);
            }
        case 0b110: // OR
            return Instruction::OR;
        case 0b111: // AND
            return Instruction::AND;
        default:
            assert(false);
    }
}

InstructionInfo GetInstructionInfo(WordType instruction) {
    InstructionInfo info;
    ByteType opCode = GetOpCode(instruction);
    switch (opCode) {
        case 0b0110111: // LUI
            info.instruction = Instruction::LUI;
            info.immediate = GetUpperImmediate(instruction);
            info.destinationRegister = GetDestinationRegister(instruction);
            break;
        case 0b0010111: // AUIPC
            info.instruction = Instruction::AUIPC;
            info.immediate = GetUpperImmediate(instruction);
            info.destinationRegister = GetDestinationRegister(instruction);
            break;
        case 0b1101111: // JAL
            info.instruction = Instruction::JAL;
            info.immediate = GetJALImmediate(instruction);
            info.destinationRegister = GetDestinationRegister(instruction);
            break;
        case 0b1100111: // JALR
            info.instruction = Instruction::JALR;
            info.immediate = GetSignedLowerImmediate(instruction);
            info.destinationRegister = GetDestinationRegister(instruction);
            info.register1 = GetRegister1(instruction);
            break;
        case 0b1100011: // BEQ & BNE & BLT & BGE & BLTU & BGEU
            info.instruction = GetBranchInstruction(instruction);
            info.immediate = GetBranchImmediate(instruction);
            info.register1 = GetRegister1(instruction);
            info.register2 = GetRegister2(instruction);
            break;
        case 0b0000011: // LB & LH & LW & LBU & LHU
            info.instruction = GetLoadInstruction(instruction);
            info.immediate = GetSignedLowerImmediate(instruction);
            info.destinationRegister = GetDestinationRegister(instruction);
            info.register1 = GetRegister1(instruction);
            break;
        case 0b0100011: // SB & SH & SW
            info.instruction = GetStoreInstruction(instruction);
            info.immediate = GetStoreImmediate(instruction);
            info.register1 = GetRegister1(instruction);
            info.register2 = GetRegister2(instruction);
            break;
        case 0b0010011: // ADDI & SLTI & SLTIU & XORI & ORI & ANDI & SLLI & SRLI & SRAI
            info.instruction = GetImmediateInstruction(instruction);
            if (info.instruction == Instruction::SRLI ||
                info.instruction == Instruction::SRAI) {
                info.immediate = GetShiftAmount(instruction);
            } else if (info.instruction == Instruction::SLTIU) {
                info.immediate = GetUnsignedLowerImmediate(instruction);
            } else {
                info.immediate = GetSignedLowerImmediate(instruction);
            }
            info.destinationRegister = GetDestinationRegister(instruction);
            info.register1 = GetRegister1(instruction);
            break;
        case 0b0110011: // ADD & SUB & SLL & SLT & SLTU & XOR & SRL & SRA & OR & AND
            info.instruction = GetArithmeticInstruction(instruction);
            info.destinationRegister = GetDestinationRegister(instruction);
            info.register1 = GetRegister1(instruction);
            info.register2 = GetRegister2(instruction);
            break;
        default:
            assert(false);
    }
    return info;
}

} // namespace

void InstructionUnit::FetchAndPush(WordType instruction, Memory& memory) {
    WordType currentInstruction = memory.ReadInstruction(instruction);
    InstructionInfo info = GetInstructionInfo(currentInstruction);
    // TODO
}

void InstructionUnit::SetPC(WordType pc) {
    PC_ = pc;
}
