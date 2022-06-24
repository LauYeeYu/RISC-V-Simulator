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

#ifndef RISC_V_SIMULATOR_INCLUDE_REGISTER_H
#define RISC_V_SIMULATOR_INCLUDE_REGISTER_H

#include <cstddef>
#include <cstdint>

#include "type.h"

class Register {
public:
    Register() = default;
    Register(const Register&) = default;
    Register(Register&&) = default;
    Register& operator=(const Register&) = default;
    Register& operator=(WordType);
    Register& operator=(Register&&) = default;
    ~Register() = default;

    /**
     * To user the register as if it is a plain WordType.
     */
    operator WordType&();

    [[nodiscard]] bool Dirty() const;

    void SetDependency(SizeType index);

    void TryResetWithIndex(SizeType index);

    void ResetDependency();

private:
    WordType value_      = 0;
    bool     dirty_      = false;
    WordType dependency_ = 0;
};

class RegisterFile {
public:
    RegisterFile() = default;
    RegisterFile(const RegisterFile&) = default;
    RegisterFile(RegisterFile&&) = default;
    RegisterFile& operator=(const RegisterFile&) = default;
    RegisterFile& operator=(RegisterFile&&) = default;
    ~RegisterFile() = default;

    /**
     * Get the register by index.
     * @param index
     * @return the register value
     */
    [[nodiscard]] WordType Read(SizeType index);

    void Write(SizeType index, WordType value, SizeType dependency);

    void AboutToWrite(SizeType index, SizeType dependency);

    void ResetDependency();

private:
    Register registers_[32];
    Register nextRegisters_[32];
};

#endif //RISC_V_SIMULATOR_INCLUDE_REGISTER_H
