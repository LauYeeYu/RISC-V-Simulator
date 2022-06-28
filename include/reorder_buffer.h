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

#ifndef RISC_V_SIMULATOR_INCLUDE_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_INCLUDE_REORDER_BUFFER_H

#include "circular_queue.h"
#include "register.h"
#include "type.h"

class Bus;

enum class ReorderType {
    registerWrite,
    memoryWrite,
    branch
};

struct ReorderBufferEntry {
    bool ready = false;
    bool predictedAnswer = false;
    ReorderType type;
    SizeType index;
    WordType value;
};

/**
 * @class ReorderBuffer
 * The ReorderBuffer class is used to store the instructions in the reorder
 * buffer (RoB).
 */
class ReorderBuffer {
public:
    ReorderBuffer() = default;
    ReorderBuffer(const ReorderBuffer&) = default;
    ReorderBuffer(ReorderBuffer&&) = default;

    ReorderBuffer& operator=(const ReorderBuffer&) = default;
    ReorderBuffer& operator=(ReorderBuffer&&) = default;

    ~ReorderBuffer() = default;

    void TryCommit(Bus& bus);

    ReorderBufferEntry& operator[](SizeType index);

    const ReorderBufferEntry& operator[](SizeType index) const;

    void Flush();

    [[nodiscard]] bool Full() const;

    [[nodiscard]] SizeType GetHead() const;

    SizeType Add(const ReorderBufferEntry& entry, Bus& bus);

private:
    CircularQueue<ReorderBufferEntry, 32> buffer_;
    CircularQueue<ReorderBufferEntry, 32> nextBuffer_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_REORDER_BUFFER_H
