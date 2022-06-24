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

#include "bus.h"

#include "instructions.h"
#include "memory.h"
#include "register.h"
#include "reorder_buffer.h"

Bus::Bus() : registerFile_(), reorderBuffer_(), memory_(204800) {
    memory_.Init();
}

void Bus::RegisterCommit(SizeType index, WordType value, SizeType dependency) {
    registerFile_.Write(index, value, dependency);
}
