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

#ifndef RISC_V_SIMULATOR_INCLUDE_BUS_H
#define RISC_V_SIMULATOR_INCLUDE_BUS_H

#include "instructions.h"
#include "load_store_buffer.h"
#include "memory.h"
#include "register.h"
#include "reorder_buffer.h"
#include "reservation_station.h"

class Bus {
public:
    Bus();
    Bus(const Bus&) = default;
    Bus(Bus&&) = default;
    Bus& operator=(const Bus&) = default;
    Bus& operator=(Bus&&) = default;
    ~Bus() = default;

    void ClearPipeline();

    void RegisterCommit(SizeType index, WordType value, SizeType dependency);

    void SetPC(WordType pc);

    void Run();

    [[nodiscard]] long Clock() const;

    [[nodiscard]] Memory& GetMemory();

    [[nodiscard]] ReorderBuffer& GetReorderBuffer();

    [[nodiscard]] LoadStoreBuffer& GetLoadStoreBuffer();

    [[nodiscard]] RegisterFile& GetRegisterFile();

    [[nodiscard]] ReservationStation& GetReservationStation();

private:
    void Flush();

    long  clock_ = 0;

    class InstructionUnit    instructionUnit_;
    class Memory             memory_;
    class RegisterFile       registerFile_;
    class ReorderBuffer      reorderBuffer_;
    class ReservationStation reservationStation_;
    class LoadStoreBuffer    loadStoreBuffer_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_BUS_H
