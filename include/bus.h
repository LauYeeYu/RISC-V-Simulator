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

private:
    void Flush();
    Memory             memory_;
    RegisterFile       registerFile_;
    ReorderBuffer      reorderBuffer_;
    ReservationStation reservationStation_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_BUS_H
