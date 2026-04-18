// Copyright 2023 Hyojin Bak (@eerraa)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"

bool rgb_matrix_indicators_kb(void) {
    if (!rgb_matrix_indicators_user()) {
         return false;
    }
    
    return true;
}