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

#ifndef RISC_V_SIMULATOR_INCLUDE_RESERVATION_STATION_H
#define RISC_V_SIMULATOR_INCLUDE_RESERVATION_STATION_H

#include "instructions.h"

struct RSEntry {
    bool        busy = false;
    Instruction instruction;
    WordType    Value1;
    WordType    Value2;
    WordType    Q1;
    WordType    Q2;
    SizeType    RoBIndex;
};

class ReservationStation {
public:
    ReservationStation() = default;
    ReservationStation(const ReservationStation&) = default;
    ReservationStation(ReservationStation&&) = default;

    ReservationStation& operator=(const ReservationStation&) = default;
    ReservationStation& operator=(ReservationStation&&) = default;

    ~ReservationStation() = default;

    void Flush();

private:
    constexpr static SizeType kEntryNumber_ = 32;

    RSEntry entries_[kEntryNumber_];
    RSEntry nextEntries_[kEntryNumber_];
};

#endif //RISC_V_SIMULATOR_INCLUDE_RESERVATION_STATION_H
