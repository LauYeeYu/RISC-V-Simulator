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

#ifndef RISC_V_SIMULATOR_INCLUDE_PREDICTOR_H
#define RISC_V_SIMULATOR_INCLUDE_PREDICTOR_H

#include "type.h"

class Predictor {
public:
    Predictor() = default;
    Predictor(const Predictor&) = default;
    Predictor(Predictor&&) = default;

    Predictor& operator=(const Predictor&) = default;
    Predictor& operator=(Predictor&&) = default;

    ~Predictor() = default;

    [[nodiscard]] bool Predict(/*TODO*/) const;

    [[nodiscard]] float GetAccuracy() const;

    void PredictWrong();

private:
    SizeType predictCount = 0;
    SizeType predictWrong = 0;
};

#endif //RISC_V_SIMULATOR_INCLUDE_PREDICTOR_H
