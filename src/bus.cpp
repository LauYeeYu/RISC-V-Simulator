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

Bus::Bus() : memory_(2048000),
             instructionUnit_(),
             registerFile_(),
             reorderBuffer_(),
             reservationStation_(),
             loadStoreBuffer_() {};

void Bus::RegisterCommit(SizeType index, WordType value, SizeType dependency) {
    registerFile_.Write(index, value, dependency);
}

void Bus::Flush() {
    registerFile_.Flush();
    reorderBuffer_.Flush();
    reservationStation_.Flush();
}

void Bus::SetPC(WordType pc) {
    instructionUnit_.SetPC(pc);
}

Memory& Bus::GetMemory() {
    return memory_;
}

ReorderBuffer& Bus::GetReorderBuffer() {
    return reorderBuffer_;
}

LoadStoreBuffer& Bus::GetLoadStoreBuffer() {
    return loadStoreBuffer_;
}

RegisterFile& Bus::GetRegisterFile() {
    return registerFile_;
}

ReservationStation& Bus::GetReservationStation() {
    return reservationStation_;
}

void Bus::ClearPipeline() {
    registerFile_.ResetDependency();
    reorderBuffer_.Clear();
    reservationStation_.Clear();
    loadStoreBuffer_.ClearOnWrongPrediction();
    instructionUnit_.ResetStateOnClearPipeline();
}

void Bus::Run() {
    while (true) {
        loadStoreBuffer_.Execute(*this);
        instructionUnit_.FetchAndPush(*this);
        reservationStation_.Execute(reorderBuffer_);
        reorderBuffer_.TryCommit(*this);
        ++clock_;
        Flush();
    }

}

long Bus::Clock() const {
    return clock_;
}
