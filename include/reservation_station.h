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

#include "ALU.h"
#include "instructions.h"

class RegisterFile;
class ReorderBuffer;

struct RSEntry {
    bool        busy = false;
    bool        empty = true;
    bool        Q1Constraint = false;
    bool        Q2Constraint = false;
    bool        executing = false;
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

    void Execute(ReorderBuffer& reorderBuffer);

    bool Add(const RSEntry& entry);

    void Clear();

private:
    constexpr static SizeType kEntryNumber_ = 32;

    void FetchResult(ReorderBuffer& reorderBuffer);

    void UpdateBusyState(const ReorderBuffer& reorderBuffer);

    void PushDataIntoALU();

    RSEntry entries_[kEntryNumber_];
    RSEntry nextEntries_[kEntryNumber_];

    AddALU   addALU_[2];
    ShiftALU shiftALU_[2];
    LogicALU logicALU_[2];
    SetALU   setALU_[2];
};

#endif //RISC_V_SIMULATOR_INCLUDE_RESERVATION_STATION_H
