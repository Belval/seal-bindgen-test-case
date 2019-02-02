// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include <functional>
#include <memory>
#include "seal/encryptionparams.h"
#include "seal/memorymanager.h"
#include "seal/util/pointer.h"

namespace seal
{
    class SEALContext
    {
    public:
        SEALContext() = delete;

        static auto Create(const EncryptionParameters &parms, 
            bool expand_mod_chain = true)
        {
            return std::shared_ptr<SEALContext>(
                new SEALContext(parms, expand_mod_chain, 
                MemoryManager::GetPool()));
        }

    private:
        SEALContext(const SEALContext &copy) = delete;

        SEALContext(SEALContext &&source) = delete;

        SEALContext &operator =(const SEALContext &assign) = delete;

        SEALContext &operator =(SEALContext &&assign) = delete;

        SEALContext(EncryptionParameters parms, bool expand_mod_chain,
            MemoryPoolHandle pool);

        MemoryPoolHandle pool_;
    };
}
