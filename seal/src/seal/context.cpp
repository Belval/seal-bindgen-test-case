// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "seal/context.h"
#include "seal/util/pointer.h"
#include <utility>
#include <stdexcept>

using namespace std;
using namespace seal::util;

namespace seal
{
    SEALContext::SEALContext(EncryptionParameters parms, bool expand_mod_chain,
        MemoryPoolHandle pool) : pool_(move(pool))
    {
        if (!pool_)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Set random generator
        if (!parms.random_generator())
        {
            parms.set_random_generator(
                UniformRandomGeneratorFactory::default_factory());
        }
    }
}
