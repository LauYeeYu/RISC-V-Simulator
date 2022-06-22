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

#include "register.h"

Register::operator WordType&() {
    return value_;
}

bool Register::SetStall(bool stall) {
    stall_ = stall;
    return stall_;
}

bool Register::Stall() const { return stall_; }

Register& Register::operator=(WordType data) {
    value_ = data;
    return *this;
}

WordType RegisterSet::Read(SizeType index) {
    return registers_[index];
}

WordType RegisterSet::Write(SizeType index, WordType value) {
    if (index == 0) {
        return 0;
    } else {
        return registers_[index] = value;
    }
}
