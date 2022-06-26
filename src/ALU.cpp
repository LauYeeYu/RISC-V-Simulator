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

#include "ALU.h"

void ALU::Flush() {
    if (busy) {
        busy = false;
        finished = true;
    } else {
        finished = false;
    }
    result = nextResult;
    index = nextIndex;
}

WordType ALU::Result() const {
    return result;
}

WordType ALU::Index() const {
    return index;
}


bool ALU::Busy() const {
    return busy;
}

bool ALU::Finished() const {
    return finished;
}

void AddALU::Execute(WordType input1, WordType input2, SizeType place, Instruction instruction) {
    busy = true;
    if (instruction == Instruction::SUB) {
        this->nextResult = input1 - input2;
    } else { // ADDI & ADD
        this->nextResult = input1 + input2;
    }
    this->nextIndex = place;
}

void ShiftALU::Execute(WordType input1, WordType input2, SizeType place, Instruction instruction) {
    busy = true;
    if (instruction == Instruction::SLL || instruction == Instruction::SLLI) {
        this->nextResult = input1 << input2;
    } else if (instruction == Instruction::SRL || instruction == Instruction::SRLI) {
        this->nextResult = input1 >> input2;
    } else { // SRA & SRAI
        this->nextResult = static_cast<SignedWordType>(input1) >> input2;
    }
    this->nextIndex = place;
}

void SetALU::Execute(WordType input1, WordType input2, SizeType place, Instruction instruction) {
    busy = true;
    if (instruction == Instruction::SLT || instruction == Instruction::SLTI) {
        this->nextResult = static_cast<WordType>(
            static_cast<SignedWordType>(input1) < static_cast<SignedWordType>(input2));
    } else { // SLTU & SLTIU
        this->nextResult = input1 < input2;
    }
    this->nextIndex = place;
}

void LogicALU::Execute(WordType input1, WordType input2, SizeType place, Instruction instruction) {
    busy = true;
    if (instruction == Instruction::XOR || instruction == Instruction::XORI) {
        this->nextResult = input1 ^ input2;
    } else if (instruction == Instruction::OR || instruction == Instruction::ORI) {
        this->nextResult = input1 | input2;
    } else { // AND & ANDI
        this->nextResult = input1 & input2;
    }
    this->nextIndex = place;
}
