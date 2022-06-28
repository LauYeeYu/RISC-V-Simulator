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

#include <iostream>
#include <string>

Memory::Memory(SizeType size) {
    memory_ = new ByteType[size];
}

Memory::~Memory() {
    delete[] memory_;
}

WordType Memory::ReadWord(SizeType index) const {
    ready_ = false;
    return *(reinterpret_cast<WordType*>(memory_ + index));
}

WordType Memory::ReadHalfWord(SizeType index) const {
    ready_ = false;
    return static_cast<WordType>(*(reinterpret_cast<HalfWordType*>(memory_ + index)));
}

WordType Memory::ReadSignedHalfWord(SizeType index) const {
    ready_ = false;
    return static_cast<WordType>(static_cast<SignedWordType>(
        *(reinterpret_cast<SignedHalfWordType*>(memory_ + index))));
}

WordType Memory::ReadByte(SizeType index) const {
    ready_ = false;
    return static_cast<WordType>(*(memory_ + index));
}

WordType Memory::ReadSignedByte(SizeType index) const {
    ready_ = false;
    return static_cast<WordType>(static_cast<SignedWordType>(*(
        reinterpret_cast<SignedByteType*>(memory_ + index))));
}

void Memory::StoreWord(SizeType index, WordType value) {
    ready_ = false;
    *(reinterpret_cast<WordType*>(memory_ + index)) = value;
}

void Memory::StoreHalfWord(SizeType index, HalfWordType value) {
    ready_ = false;
    *(reinterpret_cast<HalfWordType*>(memory_ + index)) = value;
}

void Memory::StoreByte(SizeType index, ByteType value) {
    ready_ = false;
    memory_[index] = value;
}

void Memory::Init() {
    std::string token;
    ByteType address = 0;
while (std::cin >> token) {
        if (token[0] == '@') {
            std::string address_str = token.substr(1);
            address = std::stoi(address_str, nullptr, 16);
        } else {
            memory_[address] = static_cast<ByteType>(std::stoi(token, nullptr, 16));
            ++address;
        }
    }
}

void Memory::ResetReadyState() {
    ready_ = true;
}

bool Memory::Ready() const {
    return ready_;
}

WordType Memory::ReadInstruction(SizeType index) const {
    return *(reinterpret_cast<WordType*>(memory_ + index));
}
