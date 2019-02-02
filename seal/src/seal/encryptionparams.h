// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include <iostream>
#include <numeric>
#include <memory>
#include "seal/util/defines.h"
#include "seal/util/globals.h"
#include "seal/randomgen.h"
#include "seal/memorymanager.h"

namespace seal
{
    enum class scheme_type : std::uint8_t
    {
        BFV = 0x1,
        CKKS = 0x2
    };

    inline bool is_valid_scheme(scheme_type scheme) noexcept
    {
        return (scheme == scheme_type::BFV) || 
            (scheme == scheme_type::CKKS); 
    }

    class EncryptionParameters
    {
    public:
        EncryptionParameters(scheme_type scheme)
        {
            // Check that a valid scheme is given
            if (!is_valid_scheme(scheme))
            {
                throw std::invalid_argument("unsupported scheme");
            }

            scheme_ = scheme;
        }

        inline void set_random_generator(
            std::shared_ptr<UniformRandomGeneratorFactory> random_generator)
        {
            random_generator_ = std::move(random_generator);
        }

        inline scheme_type scheme() const
        {
            return scheme_;
        }

        /**
        Returns a pointer to the random number generator factory to use for encryption.
        */
        inline std::shared_ptr<UniformRandomGeneratorFactory> random_generator() const
        {
            return random_generator_;
        }

    private:
        MemoryPoolHandle pool_ = MemoryManager::GetPool();

        scheme_type scheme_;

        std::shared_ptr<UniformRandomGeneratorFactory> random_generator_{ nullptr };
    };
}

