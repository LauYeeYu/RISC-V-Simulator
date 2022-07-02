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

#ifndef RISC_V_SIMULATOR_INCLUDE_MEMORY_H
#define RISC_V_SIMULATOR_INCLUDE_MEMORY_H

#include "type.h"

class Memory {
public:
    explicit Memory(SizeType size);
    ~Memory();

    [[nodiscard]] WordType ReadWord(SizeType index) const;
    [[nodiscard]] WordType ReadHalfWord(SizeType index) const;
    [[nodiscard]] WordType ReadSignedHalfWord(SizeType index) const;
    [[nodiscard]] WordType ReadByte(SizeType index) const;
    [[nodiscard]] WordType ReadSignedByte(SizeType index) const;

    [[nodiscard]] WordType ReadInstruction(SizeType index) const;

    void StoreWord(SizeType index, WordType value);
    void StoreHalfWord(SizeType index, HalfWordType value);
    void StoreByte(SizeType index, ByteType value);

    /**
     * Init from stdin.
     */
    void Init();

private:
    ByteType* memory_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_MEMORY_H
