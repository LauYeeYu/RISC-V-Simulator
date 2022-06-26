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

#ifndef RISC_V_SIMULATOR_INCLUDE_ALU_H
#define RISC_V_SIMULATOR_INCLUDE_ALU_H

#include "instructions.h"
#include "type.h"

class ALU {
public:
    ALU() = default;
    ALU(const ALU&) = default;
    ALU(ALU&&) = default;

    ALU& operator=(const ALU&) = default;
    ALU& operator=(ALU&&) = default;

    ~ALU() = default;

    [[nodiscard]] bool Busy() const;

    [[nodiscard]] bool Finished() const;

    [[nodiscard]] WordType Result() const;

    [[nodiscard]] WordType Index() const;

    void Flush();

protected:
    bool busy = false;
    bool finished = false;
    WordType result = 0;
    WordType index = 0;
    WordType nextResult = 0;
    SizeType nextIndex = 0;
};

class AddALU : public ALU {
public:
    AddALU() = default;
    AddALU(const AddALU&) = default;
    AddALU(AddALU&&) = default;

    AddALU& operator=(const AddALU&) = default;
    AddALU& operator=(AddALU&&) = default;

    ~AddALU() = default;

    void Execute(WordType input1,
                 WordType input2,
                 SizeType place,
                 Instruction instruction);
};

class ShiftALU : public ALU {
public:
    ShiftALU() = default;
    ShiftALU(const ShiftALU&) = default;
    ShiftALU(ShiftALU&&) = default;

    ShiftALU& operator=(const ShiftALU&) = default;
    ShiftALU& operator=(ShiftALU&&) = default;

    ~ShiftALU() = default;

    void Execute(WordType input1,
                 WordType input2,
                 SizeType place,
                 Instruction instruction);
};

class SetALU : public ALU {
public:
    SetALU() = default;
    SetALU(const SetALU&) = default;
    SetALU(SetALU&&) = default;

    SetALU& operator=(const SetALU&) = default;
    SetALU& operator=(SetALU&&) = default;

    ~SetALU() = default;

    void Execute(WordType input1,
                 WordType input2,
                 SizeType place,
                 Instruction instruction);

};

class LogicALU : public ALU {
public:
    LogicALU() = default;
    LogicALU(const LogicALU&) = default;
    LogicALU(LogicALU&&) = default;

    LogicALU& operator=(const LogicALU&) = default;
    LogicALU& operator=(LogicALU&&) = default;

    ~LogicALU() = default;

    void Execute(WordType input1,
                 WordType input2,
                 SizeType place,
                 Instruction instruction);
};

#endif //RISC_V_SIMULATOR_INCLUDE_ALU_H
