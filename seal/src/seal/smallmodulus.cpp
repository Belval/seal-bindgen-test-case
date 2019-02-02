// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "seal/smallmodulus.h"
#include "seal/util/common.h"
#include <stdexcept>

using namespace seal::util;
using namespace std;

namespace seal
{
    void SmallModulus::set_value(uint64_t value)
    {
        if (value == 0)
        {
            // Zero settings
            bit_count_ = 0;
            uint64_count_ = 1;
            value_ = 0;
            const_ratio_ = { { 0, 0, 0 } };
        }
        else if ((value >> 62 != 0) || (value == uint64_t(0x4000000000000000)) || 
            (value == 1))
        {
            throw invalid_argument("value can be at most 62 bits and cannot be 1");
        }
        else
        {
            // All normal, compute const_ratio and set everything
            value_ = value;
            bit_count_ = get_significant_bit_count(value_);

            // Compute Barrett ratios for 64-bit words (barrett_reduce_128)
            uint64_t numerator[3]{ 0, 0, 1 };
            uint64_t quotient[3]{ 0, 0, 0 };

            // Use a special method to avoid using memory pool
            //divide_uint192_uint64_inplace(numerator, value_, quotient);

            const_ratio_[0] = quotient[0];
            const_ratio_[1] = quotient[1];

            // We store also the remainder
            const_ratio_[2] = numerator[0];

            uint64_count_ = 1;
        }
    }
}