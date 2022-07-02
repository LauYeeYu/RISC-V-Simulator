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

#include "register.h"

#ifdef LAU_SHOW_REGISTER_DETAILS
#include <iostream>
#endif

Register::operator WordType&() {
    return value_;
}

Register& Register::operator=(WordType data) {
    value_ = data;
    return *this;
}

bool Register::Dirty() const { return dirty_; }

void Register::SetDependency(SizeType index) {
    dependency_ = index;
    dirty_ = true;
}

void Register::TryResetWithIndex(SizeType index) {
    if (index == dependency_) {
        dirty_ = false;
    }
}

void Register::ResetDependency() {
    dirty_ = false;
}

SizeType Register::Dependency() const {
    return dependency_;
}

Register& Register::operator+=(SignedWordType rhs) {
    value_ += rhs;
    return *this;
}

WordType RegisterFile::Read(SizeType index) {
    return static_cast<WordType>(registers_[index]);
}

void RegisterFile::Write(SizeType index, WordType value, SizeType dependency) {
    if (index != 0) {
#ifdef LAU_SHOW_REGISTER_DETAILS
        std::cerr << "Register " << index << "\t<- " << value << std::endl;
#endif
        nextRegisters_[index] = value;
        nextRegisters_[index].TryResetWithIndex(dependency);
    }
}

void RegisterFile::AboutToWrite(SizeType index, SizeType dependency) {
    if (index == 0) return;
    nextRegisters_[index].SetDependency(dependency);
}

void RegisterFile::ResetDependency() {
    for (auto& register_ : nextRegisters_) {
        register_.ResetDependency();
    }
}

void RegisterFile::Flush() {
    for (SizeType i = 0; i < kRegisterCount; ++i) {
        registers_[i] = nextRegisters_[i];
    }
}

bool RegisterFile::Dirty(SizeType index) const {
    return registers_[index].Dirty();
}

SizeType RegisterFile::Dependency(SizeType index) const {
    return registers_[index].Dependency();
}
