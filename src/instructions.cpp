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
#include <cstdlib>
#include <iostream>

#include "bus.h"
#include "load_store_buffer.h"
#include "reorder_buffer.h"
#include "type.h"

InstructionUnit::InstructionUnit() : stall_(false),
                                     end_(false),
                                     endDependency_(0),
                                     immediate_(0),
                                     dependency_(0),
                                     PC_(),
                                     predictor_() {
    PC_ = 0;
}

namespace {

ByteType GetOpCode(WordType instruction) { return instruction & 0b1111111; }

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

void InstructionUnit::FetchAndPush(Bus& bus) {
    if (stall_) {
        if (bus.GetReorderBuffer()[dependency_].ready) {
            stall_ = false;
            PC_ = bus.GetReorderBuffer()[dependency_].value + immediate_;
        }
    }
    if (bus.GetReorderBuffer().Full()) {
        return;
    }
    WordType currentInstruction = bus.GetMemory().ReadInstruction(PC_);
    if (currentInstruction == 0x0ff00513) {
        ReorderBufferEntry entry;
        entry.ready = true;
        entry.type = ReorderType::end;
        bus.GetReorderBuffer().Add(entry, bus);
        return;
    }
    InstructionInfo info = GetInstructionInfo(currentInstruction);
    switch (info.instruction) {
        case Instruction::LUI: { // Load Upper Immediate
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = true;
            entry.value = info.immediate;
            entry.index = info.destinationRegister;
            bus.GetReorderBuffer().Add(entry, bus);
            PC_ += 4;
            break;
        }
        case Instruction::AUIPC: // Add Upper Immediate to PC
            PC_ += static_cast<SignedWordType>(info.immediate);
            break;
        case Instruction::JAL: { // Jump and Link
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = true;
            entry.value = PC_ + 4;
            entry.index = info.destinationRegister;
            bus.GetReorderBuffer().Add(entry, bus);
            PC_ += static_cast<SignedWordType>(info.immediate);
            break;
        }
        case Instruction::JALR: { // Jump and Link Register
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = true;
            entry.value = PC_ + 4;
            entry.index = info.destinationRegister;
            bus.GetReorderBuffer().Add(entry, bus);
            if (!bus.GetRegisterFile().Dirty(info.register1)) {
                PC_ = ((bus.GetRegisterFile().Read(info.register1) + static_cast<SignedWordType>(info.immediate))) & ~1;
            } else if (bus.GetReorderBuffer()[bus.GetRegisterFile().Dependency(info.register1)].ready) {
                PC_ = ((bus.GetReorderBuffer()[bus.GetRegisterFile().Dependency(info.register1)].value +
                        static_cast<SignedWordType>(info.immediate))) & ~1;
            } else {
                stall_ = true;
                immediate_ = static_cast<SignedWordType>(info.immediate);
                dependency_ = info.register1;
            }
            break;
        }
        case Instruction::BEQ: // Branch on Equal
        case Instruction::BNE: // Branch on Not Equal
        case Instruction::BLT: // Branch on Less Than
        case Instruction::BGE: // Branch on Greater Than or Equal
        case Instruction::BLTU: // Branch on Less Than Unsigned
        case Instruction::BGEU: { // Branch if Equal
            ReorderBufferEntry entry;
            entry.type = ReorderType::branch;
            entry.ready = false;
            RSEntry rsEntry;
            rsEntry.instruction = info.instruction;
            if (bus.GetRegisterFile().Dirty(info.register1)) {
                rsEntry.Q1 = bus.GetRegisterFile().Dependency(info.register1);
                if (bus.GetReorderBuffer()[rsEntry.Q1].ready) {
                    rsEntry.Value1 = bus.GetReorderBuffer()[rsEntry.Q1].value;
                } else {
                    rsEntry.Q1Constraint = true;
                    rsEntry.busy = true;
                }
            } else {
                rsEntry.Value1 = bus.GetRegisterFile().Read(info.register1);
            }
            if (bus.GetRegisterFile().Dirty(info.register2)) {
                rsEntry.Q2 = bus.GetRegisterFile().Dependency(info.register2);
                if (bus.GetReorderBuffer()[rsEntry.Q2].ready) {
                    rsEntry.Value2 = bus.GetReorderBuffer()[rsEntry.Q2].value;
                } else {
                    rsEntry.Q2Constraint = true;
                    rsEntry.busy = true;
                }
            } else {
                rsEntry.Value2 = bus.GetRegisterFile().Read(info.register2);
            }
            if (predictor_.Predict()) {
                entry.index = PC_ + 4;
                PC_ += static_cast<SignedWordType>(info.immediate);
            } else {
                entry.index = PC_ + static_cast<SignedWordType>(info.immediate);
                PC_ += 4;
            }
            rsEntry.RoBIndex = bus.GetReorderBuffer().Add(entry, bus);
            bus.GetReservationStation().Add(rsEntry);
            break;
        }
        case Instruction::LB: // Load Byte
        case Instruction::LH: // Load Halfword
        case Instruction::LW: // Load Word
        case Instruction::LBU: // Load Byte Unsigned
        case Instruction::LHU: { // Load Halfword Unsigned
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = false;
            entry.index = info.destinationRegister;
            LoadStoreEntry lsEntry;
            lsEntry.type = info.instruction;
            lsEntry.offset = static_cast<SignedWordType>(info.immediate);
            lsEntry.ready = true;
            if (bus.GetRegisterFile().Dirty(info.register1)) {
                lsEntry.baseConstraintIndex = bus.GetRegisterFile().Dependency(info.register1);
                if (bus.GetReorderBuffer()[lsEntry.baseConstraintIndex].ready) {
                    lsEntry.base = bus.GetReorderBuffer()[lsEntry.baseConstraintIndex].value;
                } else {
                    lsEntry.baseConstraint = true;
                    lsEntry.ready = false;
                }
            } else {
                lsEntry.base = bus.GetRegisterFile().Read(info.register1);
            }
            lsEntry.RoBIndex = bus.GetReorderBuffer().Add(entry, bus);
            bus.GetLoadStoreBuffer().Add(lsEntry);
            PC_ += 4;
            break;
        }
        case Instruction::SB: // Store Byte
        case Instruction::SH: // Store Halfword
        case Instruction::SW: { // Store Word
            ReorderBufferEntry entry;
            entry.type = ReorderType::memoryWrite;
            entry.ready = false;
            LoadStoreEntry lsEntry;
            lsEntry.type = info.instruction;
            lsEntry.offset = static_cast<SignedWordType>(info.immediate);
            if (bus.GetRegisterFile().Dirty(info.register1)) {
                lsEntry.baseConstraintIndex = bus.GetRegisterFile().Dependency(info.register1);
                if (bus.GetReorderBuffer()[lsEntry.baseConstraintIndex].ready) {
                    lsEntry.base = bus.GetReorderBuffer()[lsEntry.baseConstraintIndex].value;
                } else {
                    lsEntry.baseConstraint = true;
                }
            } else {
                lsEntry.base = bus.GetRegisterFile().Read(info.register1);
            }
            if (bus.GetRegisterFile().Dirty(info.register2)) {
                lsEntry.valueConstraintIndex = bus.GetRegisterFile().Dependency(info.register2);
                if (bus.GetReorderBuffer()[lsEntry.valueConstraintIndex].ready) {
                    lsEntry.value = bus.GetReorderBuffer()[lsEntry.valueConstraintIndex].value;
                } else {
                    lsEntry.valueConstraint = true;
                }
            } else {
                lsEntry.value = bus.GetRegisterFile().Read(info.register2);
            }
            entry.index = bus.GetLoadStoreBuffer().GetEndIndex();
            lsEntry.RoBIndex = bus.GetReorderBuffer().Add(entry, bus);
            bus.GetLoadStoreBuffer().Add(lsEntry);
            PC_ += 4;
            break;
        }
        case Instruction::ADDI: { // Add Immediate
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = false;
            entry.index = info.destinationRegister;
            RSEntry rsEntry;
            rsEntry.instruction = info.instruction;
            if (bus.GetRegisterFile().Dirty(info.register1)) {
                rsEntry.Q1 = bus.GetRegisterFile().Dependency(info.register1);
                if (bus.GetReorderBuffer()[rsEntry.Q1].ready) {
                    rsEntry.Value1 = bus.GetReorderBuffer()[rsEntry.Q1].value;
                } else {
                    rsEntry.Q1Constraint = true;
                    rsEntry.busy = true;
                }
            } else {
                rsEntry.Value1 = bus.GetRegisterFile().Read(info.register1);
            }
            rsEntry.Value2 = info.immediate;
            rsEntry.RoBIndex = bus.GetReorderBuffer().Add(entry, bus);
            bus.GetReservationStation().Add(rsEntry);
            PC_ += 4;
            break;
        }
        case Instruction::SLTI: // Set Less Than Immediate
        case Instruction::SLTIU: // Set Less Than Immediate Unsigned
        case Instruction::XORI: // Exclusive OR Immediate
        case Instruction::ORI: // OR Immediate
        case Instruction::ANDI:  // AND Immediate
        case Instruction::SLLI: // Shift Left Logical Immediate
        case Instruction::SRLI: // Shift Right Logical Immediate
        case Instruction::SRAI: { // Shift Right Arithmetic Immediate
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = false;
            entry.index = info.destinationRegister;
            RSEntry rsEntry;
            rsEntry.instruction = info.instruction;
            if (bus.GetRegisterFile().Dirty(info.register1)) {
                rsEntry.Q1 = bus.GetRegisterFile().Dependency(info.register1);
                if (bus.GetReorderBuffer()[rsEntry.Q1].ready) {
                    rsEntry.Value1 = bus.GetReorderBuffer()[rsEntry.Q1].value;
                } else {
                    rsEntry.Q1Constraint = true;
                    rsEntry.busy = true;
                }
            } else {
                rsEntry.Value1 = bus.GetRegisterFile().Read(info.register1);
            }
            rsEntry.Value2 = info.immediate;
            rsEntry.RoBIndex = bus.GetReorderBuffer().Add(entry, bus);
            bus.GetReservationStation().Add(rsEntry);
            PC_ += 4;
            break;
        }
        case Instruction::ADD: // Add
        case Instruction::SUB: // Subtract
        case Instruction::SLL: // Shift Left Logical
        case Instruction::SLT: // Set Less Than
        case Instruction::SLTU: // Set Less Than Unsigned
        case Instruction::XOR: // Exclusive OR
        case Instruction::SRL: // Shift Right Logical
        case Instruction::SRA: // Shift Right Arithmetic
        case Instruction::OR: // OR
        case Instruction::AND: { // AND
            ReorderBufferEntry entry;
            entry.type = ReorderType::registerWrite;
            entry.ready = false;
            entry.index = info.destinationRegister;
            RSEntry rsEntry;
            rsEntry.instruction = info.instruction;
            if (bus.GetRegisterFile().Dirty(info.register1)) {
                rsEntry.Q1 = bus.GetRegisterFile().Dependency(info.register1);
                if (bus.GetReorderBuffer()[rsEntry.Q1].ready) {
                    rsEntry.Value1 = bus.GetReorderBuffer()[rsEntry.Q1].value;
                } else {
                    rsEntry.Q1Constraint = true;
                    rsEntry.busy = true;
                }
            } else {
                rsEntry.Value1 = bus.GetRegisterFile().Read(info.register1);
            }
            if (bus.GetRegisterFile().Dirty(info.register2)) {
                rsEntry.Q2 = bus.GetRegisterFile().Dependency(info.register2);
                if (bus.GetReorderBuffer()[rsEntry.Q2].ready) {
                    rsEntry.Value2 = bus.GetReorderBuffer()[rsEntry.Q2].value;
                } else {
                    rsEntry.Q2Constraint = true;
                    rsEntry.busy = true;
                }
            } else {
                rsEntry.Value2 = bus.GetRegisterFile().Read(info.register2);
            }
            rsEntry.RoBIndex = bus.GetReorderBuffer().Add(entry, bus);
            bus.GetReservationStation().Add(rsEntry);
            PC_ += 4;
            break;
        }
        default:
            assert(false);
    }
}

void InstructionUnit::SetPC(WordType pc) {
    PC_ = pc;
}
