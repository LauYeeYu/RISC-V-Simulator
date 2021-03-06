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
#include <iomanip>
#include <iostream>

#include "bus.h"

ReorderBufferEntry& ReorderBuffer::operator[](SizeType index) {
    return nextBuffer_[index];
}

const ReorderBufferEntry& ReorderBuffer::operator[](SizeType index) const {
    return buffer_[index];
}

void ReorderBuffer::TryCommit(Bus& bus) {
    if (buffer_.Empty()) return;
    if (!buffer_.Front().ready) return;
    switch (buffer_.Front().type) {
        case ReorderType::registerWrite:
            // Note that the register can only be updated in the reorder buffer
            bus.RegisterCommit(buffer_.Front().index,
                               buffer_.Front().value,
                               buffer_.HeadIndex());
            break;
        case ReorderType::memoryWrite:
            bus.GetLoadStoreBuffer()[buffer_.Front().index].ready = true;
            break;
        case ReorderType::branch: {
            bool realAnswer = static_cast<bool>(buffer_.Front().value);
            bus.UpdatePredictor(buffer_.Front().address, realAnswer);
            if (buffer_.Front().predictedAnswer != realAnswer) {
                bus.ClearPipeline();
                bus.SetPC(buffer_.Front().index);
                return; // skip the pop
            }
            break;
        }
        case ReorderType::end:
            std::cout << (static_cast<HalfWordType>(bus.GetRegisterFile().Read(10)) & 255u)
                      << std::endl;
#ifdef LAU_TEST
            std::cerr << "Terminated at " << bus.Clock() << "." << std::endl;
            if (bus.PredictorAccuracy() == bus.PredictorAccuracy()) {
                std::cerr << "Predictor accuracy: "
                          << std::fixed << std::setprecision(2)
                          << bus.PredictorAccuracy() * 100  << "%." << std::endl;
            } else {
                std::cerr << "Predictor accuracy: N/A (no prediction in this testcase)"
                          << std::endl;
            }
#endif
            exit(0);
        default:
            assert(false); // should never happen
    }
    nextBuffer_.Pop();
}

void ReorderBuffer::Flush() { buffer_ = nextBuffer_; }
void ReorderBuffer::Clear() { nextBuffer_.Clear(); }

bool ReorderBuffer::Full() const { return buffer_.Full(); }

SizeType ReorderBuffer::Add(const ReorderBufferEntry& entry, Bus& bus) {
    nextBuffer_.Push(entry);
    if (entry.type == ReorderType::registerWrite) {
        bus.GetRegisterFile().AboutToWrite(entry.index, nextBuffer_.TailIndex());
    }
    return nextBuffer_.TailIndex();
}

const ReorderBufferEntry& ReorderBuffer::GetEntry(SizeType index) const {
    return buffer_[index];
}

ReorderBufferEntry& ReorderBuffer::WriteEntry(SizeType index) {
    return nextBuffer_[index];
}
