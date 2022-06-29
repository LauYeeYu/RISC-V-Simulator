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

#include "reservation_station.h"

#include <cassert>

#include "reorder_buffer.h"

void ReservationStation::Flush() {
    for (SizeType i = 0; i < kEntryNumber_; ++i) {
        entries_[i] = nextEntries_[i];
    }
    for (auto& alu : addALU_) alu.Flush();
    for (auto& alu : shiftALU_) alu.Flush();
    for (auto& alu : logicALU_) alu.Flush();
    for (auto& alu : setALU_) alu.Flush();
}

void ReservationStation::Execute(ReorderBuffer& reorderBuffer) {
    this->FetchResult(reorderBuffer);
    this->PushDataIntoALU();
    this->UpdateBusyState(reorderBuffer);
}

void ReservationStation::FetchResult(ReorderBuffer& reorderBuffer) {
    for (auto& alu : addALU_) {
        if (alu.Finished()) {
            reorderBuffer[entries_[alu.Index()].RoBIndex].value = alu.Result();
            reorderBuffer[entries_[alu.Index()].RoBIndex].ready = true;
            nextEntries_[alu.Index()].empty = true;
        }
    }
    for (auto& alu : shiftALU_) {
        if (alu.Finished()) {
            reorderBuffer[entries_[alu.Index()].RoBIndex].value = alu.Result();
            reorderBuffer[entries_[alu.Index()].RoBIndex].ready = true;
            nextEntries_[alu.Index()].empty = true;
        }
    }
    for (auto& alu : setALU_) {
        if (alu.Finished()) {
            reorderBuffer[entries_[alu.Index()].RoBIndex].value = alu.Result();
            reorderBuffer[entries_[alu.Index()].RoBIndex].ready = true;
            nextEntries_[alu.Index()].empty = true;
        }
    }
    for (auto& alu : logicALU_) {
        if (alu.Finished()) {
            reorderBuffer[entries_[alu.Index()].RoBIndex].value = alu.Result();
            reorderBuffer[entries_[alu.Index()].RoBIndex].ready = true;
            nextEntries_[alu.Index()].empty = true;
        }
    }
}

void ReservationStation::PushDataIntoALU() {
    for (SizeType i = 0; i < kEntryNumber_; ++i) {
        if (!entries_[i].empty && !entries_[i].busy) {
            switch (entries_[i].instruction) {
                case Instruction::ADD:
                case Instruction::SUB:
                case Instruction::ADDI:
                    for (auto& alu : addALU_) {
                        if (!alu.Busy()) {
                            alu.Execute(entries_[i].Value1, entries_[i].Value2, i, entries_[i].instruction);
                            break;
                        }
                    }
                    break;
                case Instruction::SLL:
                case Instruction::SLLI:
                case Instruction::SRL:
                case Instruction::SRLI:
                case Instruction::SRA:
                case Instruction::SRAI:
                    for (auto& alu : shiftALU_) {
                        if (!alu.Busy()) {
                            alu.Execute(entries_[i].Value1, entries_[i].Value2, i, entries_[i].instruction);
                            break;
                        }
                    }
                    break;
                case Instruction::SLT:
                case Instruction::SLTI:
                case Instruction::SLTU:
                case Instruction::SLTIU:
                case Instruction::BEQ:
                case Instruction::BNE:
                case Instruction::BLT:
                case Instruction::BGE:
                case Instruction::BLTU:
                case Instruction::BGEU:
                    for (auto& alu : setALU_) {
                        if (!alu.Busy()) {
                            alu.Execute(entries_[i].Value1, entries_[i].Value2, i, entries_[i].instruction);
                            break;
                        }
                    }
                    break;
                case Instruction::XOR:
                case Instruction::XORI:
                case Instruction::OR:
                case Instruction::ORI:
                case Instruction::AND:
                case Instruction::ANDI:
                    for (auto& alu : logicALU_) {
                        if (!alu.Busy()) {
                            alu.Execute(entries_[i].Value1, entries_[i].Value2, i, entries_[i].instruction);
                            break;
                        }
                    }
                    break;
                default:
                    assert(false);
            }
        }
    }
}

void ReservationStation::UpdateBusyState(const ReorderBuffer& reorderBuffer) {
    for (SizeType i = 0; i < kEntryNumber_; ++i) {
        if (entries_[i].empty) continue;
        if (entries_[i].Q1Constraint) {
            if (reorderBuffer[entries_[i].Q1].ready) {
                nextEntries_[i].Q1 = reorderBuffer[entries_[i].Q1].value;
                nextEntries_[i].Q1Constraint = false;
            }
        }
        if (entries_[i].Q2Constraint) {
            if (reorderBuffer[entries_[i].Q2].ready) {
                nextEntries_[i].Q2 = reorderBuffer[entries_[i].Q2].value;
                nextEntries_[i].Q2Constraint = false;
            }
        }
        if (!nextEntries_[i].Q1Constraint && !nextEntries_[i].Q2Constraint) {
            nextEntries_[i].busy = false;
        }
    }
}

bool ReservationStation::Add(const RSEntry& entry) {
    for (SizeType i = 0; i < kEntryNumber_; ++i) {
        if (entries_[i].empty) {
            nextEntries_[i] = entry;
            nextEntries_[i].empty = false;
            return true;
        }
    }
    return false;
}

void ReservationStation::Clear() {
    for (auto& i : nextEntries_) i.empty = true;
    for (auto& alu : addALU_) alu.Clear();
    for (auto& alu : shiftALU_) alu.Clear();
    for (auto& alu : setALU_) alu.Clear();
    for (auto& alu : logicALU_) alu.Clear();
}
