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

#ifndef RISC_V_SIMULATOR_INCLUDE_LOAD_STORE_BUFFER_H
#define RISC_V_SIMULATOR_INCLUDE_LOAD_STORE_BUFFER_H

#include "instructions.h"
#include "circular_queue.h"

class Bus;
class ReorderBuffer;

struct LoadStoreEntry {
    bool ready = false;
    bool baseConstraint = false;
    bool valueConstraint = false;
    Instruction    type;
    WordType       base;
    WordType       baseConstraintIndex;
    SignedSizeType offset;
    WordType       value;
    WordType       valueConstraintIndex;
    SizeType       RoBIndex;
};

class LoadStoreBuffer {
public:
    LoadStoreBuffer() = default;
    LoadStoreBuffer(const LoadStoreBuffer&) = default;
    LoadStoreBuffer(LoadStoreBuffer&&) = default;

    LoadStoreBuffer& operator=(const LoadStoreBuffer&) = default;
    LoadStoreBuffer& operator=(LoadStoreBuffer&&) = default;

    ~LoadStoreBuffer() = default;

    [[nodiscard]] bool Full() const;

    void Add(const LoadStoreEntry& entry);

    void Execute(Bus& bus);

    LoadStoreEntry& operator[](SizeType index);

    void ClearOnWrongPrediction();

private:
    void UpdateBusyState(const ReorderBuffer& reorderBuffer);

    void MemoryIO(Bus& bus);

    CircularQueue<LoadStoreEntry, 32> buffer_;

    mutable SizeType count_ = 0; // 2 - 0
};

#endif //RISC_V_SIMULATOR_INCLUDE_LOAD_STORE_BUFFER_H
