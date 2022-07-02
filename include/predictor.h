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

/**
 * @class Predictor
 * A 4-bit two-level adaptive predictor
 */
class Predictor {
public:
    constexpr static SizeType kBitCount = 4; // the number of bit that the predictor uses
    constexpr static SizeType kBucketSize = 4096; // the number of buckets to hold the data
    constexpr static WordType kAnd = 0b1111'1111'1111;
    struct PatternHistoryTable {
        bool prediction[1 << kBitCount] = {false};
    };

    Predictor();
    Predictor(const Predictor&) = default;
    Predictor(Predictor&&) = default;

    Predictor& operator=(const Predictor&) = default;
    Predictor& operator=(Predictor&&) = default;

    ~Predictor() = default;

    [[nodiscard]] bool Predict(WordType instructionAddress);

    void Update(WordType instructionAddress, bool answer);

    [[nodiscard]] float GetAccuracy() const;

private:
    PatternHistoryTable patternHistoryTable_[kBucketSize];
    ByteType history_[kBucketSize] = {0};
    SizeType totalWrong_ = 0;
    SizeType totalCorrect_ = 0;
};

#endif //RISC_V_SIMULATOR_INCLUDE_PREDICTOR_H
