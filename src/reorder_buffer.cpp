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

#include "reorder_buffer.h"

#include <cassert>

#include "bus.h"

ReorderBufferEntry& ReorderBuffer::operator[](SizeType index) {
    return buffer_[index];
}

const ReorderBufferEntry& ReorderBuffer::operator[](SizeType index) const {
    return buffer_[index];
}

void ReorderBuffer::TryCommit(Bus& bus) {
    if (!buffer_.Front().ready) return;
    switch (buffer_.Front().type) {
        case ReorderType::registerWrite:
            // Note that the register can only be updated in the reorder buffer
            bus.RegisterCommit(buffer_.Front().index,
                               buffer_.Front().value,
                               buffer_.HeadIndex());
            break;
        case ReorderType::wordMemoryWrite:
        case ReorderType::halfWordMemoryWrite:
        case ReorderType::byteMemoryWrite:
            bus.LoadStoreBuffer()[buffer_.Front().index].ready = true;
            break;
        case ReorderType::branch:
            if (buffer_.Front().predictedAnswer != static_cast<bool>(buffer_.Front().value)) {
                bus.ClearPipeline();
                bus.SetPC(buffer_.Front().index);
            }
            break;
        default:
            assert(false); // should never happen
    }
    nextBuffer_.Pop();
}

void ReorderBuffer::Flush() {
    buffer_ = nextBuffer_;
}

SizeType ReorderBuffer::GetHead() const {
    return buffer_.HeadIndex();
}
