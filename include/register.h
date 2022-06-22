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

    /**
     * Get the stall status of the register.
     * @return the stall status of the register.
     */
    [[nodiscard]] bool Stall() const;

    /**
     * Set the stall status of the register.
     * @param stall
     * @return
     */
    bool SetStall(bool stall);

private:
    WordType value_ = 0;
    bool     stall_ = false;
};

class RegisterSet {
public:
    RegisterSet() = default;
    RegisterSet(const RegisterSet&) = default;
    RegisterSet(RegisterSet&&) = default;
    RegisterSet& operator=(const RegisterSet&) = default;
    RegisterSet& operator=(RegisterSet&&) = default;
    ~RegisterSet() = default;

    /**
     * Get the register by index.
     * @param index
     * @return the register value
     */
    [[nodiscard]] WordType Read(SizeType index);

    WordType Write(SizeType index, WordType value);

private:
    Register registers_[32];
};

#endif //RISC_V_SIMULATOR_INCLUDE_REGISTER_H
