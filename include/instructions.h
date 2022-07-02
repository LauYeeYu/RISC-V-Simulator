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

#ifndef RISC_V_SIMULATOR_INCLUDE_INSTRUCTIONS_H
#define RISC_V_SIMULATOR_INCLUDE_INSTRUCTIONS_H

#include "memory.h"
#include "predictor.h"
#include "register.h"
#include "type.h"

class Bus;

enum class Instruction {
    LUI, // Load Upper Immediate
    AUIPC, // Add Upper Immediate
    JAL, // Jump and Link
    JALR, // Jump and Link Register
    BEQ, // Branch Equal
    BNE, // Branch Not Equal
    BLT, // Branch Less Than
    BGE, // Branch Greater Than or Equal
    BLTU, // Branch Less Than Unsigned
    BGEU, // Branch Greater Than or Equal Unsigned
    LB, // Load Byte
    LH, // Load Half Word
    LW, // Load Word
    LBU, // Load Byte Unsigned
    LHU, // Load Half Word Unsigned
    SB, // Store Byte
    SH, // Store Half Word
    SW, // Store Word
    ADDI, // Add Immediate
    SLTI, // Set Less Than Immediate
    SLTIU, // Set Less Than Immediate Unsigned
    XORI, // Exclusive OR Immediate
    ORI, // OR Immediate
    ANDI, // AND Immediate
    SLLI, // Shift Left Immediate
    SRLI, // Shift Right Logical Immediate
    SRAI, // Shift Right Arithmetic Immediate
    ADD, // Add
    SUB, // Subtract
    SLL, // Shift Left
    SLT, // Set Less Than
    SLTU, // Set Less Than Unsigned
    XOR, // Exclusive OR
    SRL, // Shift Right Logical
    SRA, // Shift Right Arithmetic
    OR, // OR
    AND, // AND
    END // End of Main 0x0ff00513
};

struct InstructionInfo {
    Instruction instruction;
    SizeType register1;
    SizeType register2;
    SizeType destinationRegister;
    WordType immediate;
};

class InstructionUnit {
public:
    InstructionUnit();
    InstructionUnit(const InstructionUnit&) = default;
    InstructionUnit(InstructionUnit&&) = default;

    InstructionUnit& operator=(const InstructionUnit&) = default;
    InstructionUnit& operator=(InstructionUnit&&) = default;

    ~InstructionUnit() = default;

    /**
     * Decode the instruction.
     * @param bus
     * @return the instruction info
     */
    void FetchAndPush(Bus& bus);

    /**
     * Set the PC.  Please note that is function is called only when the
     * prediction is incorrect.
     * @param pc
     */
    void SetPC(WordType pc);

    void ResetStateOnClearPipeline();

private:
    bool           stall_ = false;
    bool           end_ = false;
    SizeType       endDependency_ = 0;
    SignedWordType immediate_ = 0; // for JALR
    SizeType       dependency_ = 0; // for JALR
    Register       PC_;
    Predictor      predictor_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_INSTRUCTIONS_H
