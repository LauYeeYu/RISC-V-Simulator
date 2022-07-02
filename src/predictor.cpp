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

#include "predictor.h"

#include <utility>
#include <cmath>

Predictor::Predictor() : totalCorrect_(0), totalWrong_(0) {}

bool Predictor::Predict(WordType instructionAddress) {
    ByteType index = instructionAddress & kAnd;
    return patternHistoryTable_[index].prediction[history_[index]];
}

void Predictor::Update(WordType instructionAddress, bool answer) {
    ByteType index = instructionAddress & kAnd;
    if (patternHistoryTable_[index].prediction[history_[index]] == answer) {
        totalCorrect_++;
    } else {
        totalWrong_++;
    }
    patternHistoryTable_[index].prediction[history_[index]] = answer;
    history_[index] = history_[index] >> 1 | (answer ? 0b1000 : 0);
}

float Predictor::GetAccuracy() const {
    if (totalCorrect_ + totalWrong_ == 0) {
        return NAN;
    }
    return static_cast<float>(totalCorrect_) / static_cast<float>(totalCorrect_ + totalWrong_);
}
