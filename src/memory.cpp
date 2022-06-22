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

#include "memory.h"

Memory::Memory(SizeType size) {
    memory_ = new ByteType[size];
}

Memory::~Memory() {
    delete[] memory_;
}

WordType Memory::ReadWord(SizeType index) const {
    return *(reinterpret_cast<WordType*>(memory_ + index));
}

HalfWordType Memory::ReadHalfWord(SizeType index) const {
    return *(reinterpret_cast<HalfWordType*>(memory_ + index));
}

ByteType Memory::ReadByte(SizeType index) const {
    return *(memory_ + index);
}

void Memory::StoreWord(SizeType index, WordType value) {
    *(reinterpret_cast<WordType*>(memory_ + index)) = value;
}

void Memory::StoreHalfWord(SizeType index, HalfWordType value) {
    *(reinterpret_cast<HalfWordType*>(memory_ + index)) = value;
}

void Memory::StoreByte(SizeType index, ByteType value) {
    memory_[index] = value;
}
