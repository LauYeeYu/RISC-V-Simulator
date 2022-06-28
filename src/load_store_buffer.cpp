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
            MemoryIO(bus);
        }
    } else if (buffer_.Front().ready) {
        count_ = 2;
    }
    this->UpdateBusyState(bus.ReorderBuffer());
}

void LoadStoreBuffer::UpdateBusyState(const ReorderBuffer& reorderBuffer) {
    for (auto& entry : buffer_) {
        if (entry.baseConstraint || reorderBuffer[entry.baseConstraintIndex].ready) {
            entry.base = reorderBuffer[entry.baseConstraintIndex].value;
            entry.baseConstraint = true;
        }
        if (entry.type == Instruction::SW || entry.type == Instruction::SH ||
            entry.type == Instruction::SB) {
            if (entry.valueConstraint || reorderBuffer[entry.valueConstraintIndex].ready) {
                entry.value = reorderBuffer[entry.valueConstraintIndex].value;
                entry.valueConstraint = true;
            }
        } else {
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
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.Memory().ReadWord(buffer_.Front().base + buffer_.Front().offset);
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LH:
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.Memory().ReadSignedHalfWord(buffer_.Front().base + buffer_.Front().offset);
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LHU:
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.Memory().ReadHalfWord(buffer_.Front().base + buffer_.Front().offset);
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LB:
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.Memory().ReadSignedByte(buffer_.Front().base + buffer_.Front().offset);
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::LBU:
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].value =
                bus.Memory().ReadByte(buffer_.Front().base + buffer_.Front().offset);
            bus.ReorderBuffer()[buffer_.Front().RoBIndex].ready = true;
            break;
        case Instruction::SW:
            bus.Memory().StoreWord(buffer_.Front().base + buffer_.Front().offset,
                                   static_cast<WordType>(buffer_.Front().value));
            break;
        case Instruction::SH:
            bus.Memory().StoreHalfWord(buffer_.Front().base + buffer_.Front().offset,
                                       static_cast<HalfWordType>(buffer_.Front().value));
            break;
        case Instruction::SB:
            bus.Memory().StoreByte(buffer_.Front().base + buffer_.Front().offset,
                                   static_cast<ByteType>(buffer_.Front().value));
            break;
        default:
            assert(false);
    }
}

