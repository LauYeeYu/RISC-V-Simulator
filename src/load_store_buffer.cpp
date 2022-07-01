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

#include "load_store_buffer.h"

#include <cassert>
#include <iostream>

#include "bus.h"
#include "instructions.h"
#include "memory.h"
#include "register.h"
#include "reorder_buffer.h"

bool LoadStoreBuffer::Full() const {
    return buffer_.Full();
}

void LoadStoreBuffer::Add(const LoadStoreEntry& entry) {
    buffer_.Push(entry);
}

void LoadStoreBuffer::Execute(Bus& bus) {
    if (count_ > 0) {
        --count_;
        if (count_ == 0) {
            this->MemoryIO(bus);
            buffer_.Pop();
        }
    } else if (!buffer_.Empty() && buffer_.Front().ready) {
        count_ = 2;
    }
    this->UpdateBusyState(bus.GetReorderBuffer());
}

void LoadStoreBuffer::UpdateBusyState(ReorderBuffer& reorderBuffer) {
    for (auto& entry : buffer_) {
        if (entry.baseConstraint && reorderBuffer.GetEntry(entry.baseConstraintIndex).ready) {
            entry.base = reorderBuffer.GetEntry(entry.baseConstraintIndex).value;
            entry.baseConstraint = false;
        }
        if (entry.type == Instruction::SW || entry.type == Instruction::SH ||
            entry.type == Instruction::SB) {
            if (entry.valueConstraint && reorderBuffer.GetEntry(entry.valueConstraintIndex).ready) {
                entry.value = reorderBuffer.GetEntry(entry.valueConstraintIndex).value;
                entry.valueConstraint = false;
            }
            if (!entry.baseConstraint && !entry.valueConstraint) {
                    reorderBuffer.WriteEntry(entry.RoBIndex).ready = true;
            }
        } else { // Load
            if (!entry.baseConstraint && !entry.valueConstraint) {
                entry.ready = true;
            }
        }
    }
}

LoadStoreEntry& LoadStoreBuffer::operator[](SizeType index) {
    return buffer_[index];
}

void LoadStoreBuffer::MemoryIO(Bus& bus) {
    switch (buffer_.Front().type) {
        case Instruction::LW:
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.GetMemory().ReadWord(buffer_.Front().base + buffer_.Front().offset);
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LH:
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.GetMemory().ReadSignedHalfWord(buffer_.Front().base + buffer_.Front().offset);
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LHU:
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.GetMemory().ReadHalfWord(buffer_.Front().base + buffer_.Front().offset);
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LB:
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.GetMemory().ReadSignedByte(buffer_.Front().base + buffer_.Front().offset);
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LBU:
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.GetMemory().ReadByte(buffer_.Front().base + buffer_.Front().offset);
            bus.GetReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::SW:
            bus.GetMemory().StoreWord(buffer_.Front().base + buffer_.Front().offset,
                                      static_cast<WordType>(buffer_.Front().value));
#ifdef LAU_SHOW_ALL_DETAILS
            std::cerr << "Store Word: memory at "
                      << buffer_.Front().base + buffer_.Front().offset << " "
                      << buffer_.Front().value << std::endl;
#endif
            break;
        case Instruction::SH:
            bus.GetMemory().StoreHalfWord(buffer_.Front().base + buffer_.Front().offset,
                                          static_cast<HalfWordType>(buffer_.Front().value));
            break;
        case Instruction::SB:
            bus.GetMemory().StoreByte(buffer_.Front().base + buffer_.Front().offset,
                                      static_cast<ByteType>(buffer_.Front().value));
            break;
        default:
            assert(false);
    }
}

void LoadStoreBuffer::ClearOnWrongPrediction() {
    SizeType tail = (buffer_.TailIndex() + 1) % buffer_.MaxSize();
    for (SizeType i = buffer_.HeadIndex(); i != tail; i = (i + 1) % buffer_.MaxSize()) {
        if ((buffer_[i].type == Instruction::SB ||
             buffer_[i].type == Instruction::SH ||
             buffer_[i].type == Instruction::SW) && buffer_[i].ready) {
            continue;
        }
        buffer_.SetAsEnd(i);
        break;
    }
    if (buffer_.Empty()) {
        count_ = 0;
    }

}

SizeType LoadStoreBuffer::GetEndIndex() const {
    return (buffer_.TailIndex() + 1) % buffer_.MaxSize();
}

