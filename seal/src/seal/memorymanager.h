// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include <memory>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include "seal/util/mempool.h"
#include "seal/util/globals.h"

#ifndef _M_CEE
#include <thread>
#include <mutex>
#endif

namespace seal
{
    class MemoryPoolHandle
    {
    public:
        MemoryPoolHandle() = default;

        MemoryPoolHandle(std::shared_ptr<util::MemoryPool> pool) noexcept :
            pool_(std::move(pool))
        {
        }

        MemoryPoolHandle(const MemoryPoolHandle &copy) noexcept
        {
            operator =(copy);
        }

        MemoryPoolHandle(MemoryPoolHandle &&source) noexcept
        {
            operator =(std::move(source));
        }

        inline MemoryPoolHandle &operator =(const MemoryPoolHandle &assign) noexcept
        {
            pool_ = assign.pool_;
            return *this;
        }

        inline MemoryPoolHandle &operator =(MemoryPoolHandle &&assign) noexcept
        {
            pool_ = std::move(assign.pool_);
            return *this;
        }

        inline static MemoryPoolHandle Global()
        {
            return MemoryPoolHandle(
                std::shared_ptr<util::MemoryPool>(std::shared_ptr<util::MemoryPool>(), 
                util::global_variables::global_memory_pool.get()));
        }
#ifndef _M_CEE
        inline static MemoryPoolHandle ThreadLocal()
        {
            return MemoryPoolHandle(
                std::shared_ptr<util::MemoryPool>(std::shared_ptr<util::MemoryPool>(), 
                util::global_variables::tls_memory_pool.get()));
        }
#endif
        inline static MemoryPoolHandle New(bool clear_on_destruction = false) 
        {
            return MemoryPoolHandle(
                std::make_shared<util::MemoryPoolMT>(clear_on_destruction));
        }

        inline operator util::MemoryPool &() const
        {
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
            return *pool_.get();
        }

        inline std::size_t pool_count() const
        {
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
            return pool_->pool_count();
        }

        inline std::size_t alloc_byte_count() const
        {
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
            return pool_->alloc_byte_count();
        }

        inline operator bool () const
        {
            return pool_.operator bool();
        }

        inline bool operator ==(const MemoryPoolHandle &compare) noexcept
        {
            return pool_ == compare.pool_;
        }

        inline bool operator !=(const MemoryPoolHandle &compare) noexcept
        {
            return pool_ != compare.pool_;
        }

    private:
        std::shared_ptr<util::MemoryPool> pool_ = nullptr;
    };

    using mm_prof_opt_t = std::uint64_t;

    enum mm_prof_opt : mm_prof_opt_t
    {
        DEFAULT = 0x0,
        FORCE_GLOBAL = 0x1,
        FORCE_NEW = 0x2,
        FORCE_THREAD_LOCAL = 0x4
    };

    /*
    The MMProf is a pure virtual class that every profile for the MemoryManager 
    should inherit from. The only functionality this class implements is the 
    get_pool(mm_prof_opt_t) function that returns a MemoryPoolHandle pointing 
    to a pool selected by internal logic optionally using the input parameter 
    of type mm_prof_opt_t. The returned MemoryPoolHandle must point to a valid 
    memory pool. 
    */
    class MMProf
    {
    public:
        /**
        Creates a new MMProf.
        */
        MMProf() = default;

        /**
        Destroys the MMProf.
        */
        virtual ~MMProf() noexcept
        {
        }

        /**
        Returns a MemoryPoolHandle pointing to a pool selected by internal logic 
        in a derived class and by the mm_prof_opt_t input parameter.
        
        */
        virtual MemoryPoolHandle get_pool(mm_prof_opt_t) = 0;

    private:
    };

    /**
    A memory manager profile that always returns a MemoryPoolHandle pointing to 
    the global memory pool. SEAL uses this memory manager profile by default.
    */
    class MMProfGlobal : public MMProf
    {
    public:
        /**
        Creates a new MMProfGlobal.
        */
        MMProfGlobal() = default;

        /**
        Destroys the MMProfGlobal.
        */
        virtual ~MMProfGlobal() noexcept override
        {
        }

        /**
        Returns a MemoryPoolHandle pointing to the global memory pool. The 
        mm_prof_opt_t input parameter has no effect.
        */
        inline virtual MemoryPoolHandle 
            get_pool(mm_prof_opt_t) override
        {
            return MemoryPoolHandle::Global();
        }

    private:
    };

    /**
    A memory manager profile that always returns a MemoryPoolHandle pointing to 
    the new thread-safe memory pool. This profile should not be used except in 
    special circumstances, as it does not result in any reuse of allocated memory.
    */
    class MMProfNew : public MMProf
    {
    public:
        /**
        Creates a new MMProfNew.
        */
        MMProfNew() = default;

        /**
        Destroys the MMProfNew.
        */
        virtual ~MMProfNew() noexcept override
        {
        }

        /**
        Returns a MemoryPoolHandle pointing to a new thread-safe memory pool. The 
        mm_prof_opt_t input parameter has no effect.
        */
        inline virtual MemoryPoolHandle 
            get_pool(mm_prof_opt_t) override
        {
            return MemoryPoolHandle::New();
        }

    private:
    };

    /**
    A memory manager profile that always returns a MemoryPoolHandle pointing to 
    specific memory pool.
    */
    class MMProfFixed : public MMProf
    {
    public:
        /**
        Creates a new MMProfFixed. The MemoryPoolHandle given as argument is returned 
        by every call to get_pool(mm_prof_opt_t).

        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if pool is uninitialized 
        */
        MMProfFixed(MemoryPoolHandle pool) : pool_(std::move(pool))
        {
            if (!pool_)
            {
                throw std::invalid_argument("pool is uninitialized");
            }
        }

        /**
        Destroys the MMProfFixed.
        */
        virtual ~MMProfFixed() noexcept override
        {
        }

        /**
        Returns a MemoryPoolHandle pointing to the stored memory pool. The 
        mm_prof_opt_t input parameter has no effect.
        */
        inline virtual MemoryPoolHandle
            get_pool(mm_prof_opt_t) override
        {
            return pool_;
        }

    private:
        MemoryPoolHandle pool_;
    };
#ifndef _M_CEE
    /**
    A memory manager profile that always returns a MemoryPoolHandle pointing to 
    the thread-local memory pool. This profile should be used with care, as any 
    memory allocated by it will be released once the thread exits. In other words, 
    the thread-local memory pool cannot be used to share memory across different 
    threads. On the other hand, this profile can be useful when a very high number 
    of threads doing simultaneous allocations would cause contention in the 
    global memory pool.
    */
    class MMProfThreadLocal : public MMProf
    {
    public:
        /**
        Creates a new MMProfThreadLocal.
        */
        MMProfThreadLocal() = default;

        /**
        Destroys the MMProfThreadLocal.
        */
        virtual ~MMProfThreadLocal() noexcept override
        {
        }

        /**
        Returns a MemoryPoolHandle pointing to the thread-local memory pool. The 
        mm_prof_opt_t input parameter has no effect.
        */
        inline virtual MemoryPoolHandle 
            get_pool(mm_prof_opt_t) override
        {
            return MemoryPoolHandle::ThreadLocal();
        }

    private:
    };
#endif
    /**
    The MemoryManager class can be used to create instances of MemoryPoolHandle 
    based on a given "profile". A profile is implemented by inheriting from the 
    MMProf class (pure virtual) and encapsulates internal logic for deciding which 
    memory pool to use.
    */
    class MemoryManager
    {
        friend class MMProfGuard;

    public:
        MemoryManager() = delete;

        /**
        Sets the current profile to a given one and returns a unique_ptr pointing 
        to the previously set profile.

        @param[in] mm_prof Pointer to a new memory manager profile
        @throws std::invalid_argument if mm_prof is nullptr
        */
        static inline std::unique_ptr<MMProf>
            SwitchProfile(MMProf* &&mm_prof) noexcept
        {
#ifndef _M_CEE
            std::lock_guard<std::mutex> switching_lock(switch_mutex_);
#endif
            return SwitchProfileThreadUnsafe(std::move(mm_prof));
        }

        /**
        Sets the current profile to a given one and returns a unique_ptr pointing 
        to the previously set profile.

        @param[in] mm_prof Pointer to a new memory manager profile
        @throws std::invalid_argument if mm_prof is nullptr
        */
        static inline std::unique_ptr<MMProf> SwitchProfile(
            std::unique_ptr<MMProf> &&mm_prof) noexcept
        {
#ifndef _M_CEE
            std::lock_guard<std::mutex> switch_lock(switch_mutex_);
#endif
            return SwitchProfileThreadUnsafe(std::move(mm_prof));
        }

        /**
        Returns a MemoryPoolHandle according to the currently set memory manager 
        profile and prof_opt. The following values for prof_opt have an effect 
        independent of the current profile:


            mm_prof_opt::FORCE_NEW: return MemoryPoolHandle::New()
            mm_prof_opt::FORCE_GLOBAL: return MemoryPoolHandle::Global()
            mm_prof_opt::FORCE_THREAD_LOCAL: return MemoryPoolHandle::ThreadLocal()

        Other values for prof_opt are forwarded to the current profile and, depending 
        on the profile, may or may not have an effect. The value mm_prof_opt::DEFAULT
        will always invoke a default behavior for the current profile.

        @param[in] prof_opt A mm_prof_opt_t parameter used to provide additional
        instructions to the memory manager profile for internal logic.
        */
        template<typename... Args>
        static inline MemoryPoolHandle GetPool(mm_prof_opt_t prof_opt, Args &&...args)
        {
            switch (prof_opt)
            {
            case mm_prof_opt::FORCE_GLOBAL:
                return MemoryPoolHandle::Global();

            case mm_prof_opt::FORCE_NEW:
                    return MemoryPoolHandle::New(std::forward<Args>(args)...);
#ifndef _M_CEE
            case mm_prof_opt::FORCE_THREAD_LOCAL:
                    return MemoryPoolHandle::ThreadLocal();
#endif
            default:
#ifdef SEAL_DEBUG
                {
                    auto pool = mm_prof_->get_pool(prof_opt);
                    if (!pool)
                    {
                        throw std::logic_error("cannot return uninitialized pool");
                    }
                    return pool;
                }
#endif
                return mm_prof_->get_pool(prof_opt);
            }
        }

        static inline MemoryPoolHandle GetPool()
        {
            return GetPool(mm_prof_opt::DEFAULT);
        }

    private:
        static inline std::unique_ptr<MMProf>
            SwitchProfileThreadUnsafe(
                MMProf* &&mm_prof)
        {
            if (!mm_prof)
            {
                throw std::invalid_argument("mm_prof cannot be nullptr");
            }
            auto ret_mm_prof = std::move(mm_prof_);
            mm_prof_.reset(mm_prof);
            return ret_mm_prof;
        }

        static inline std::unique_ptr<MMProf>
            SwitchProfileThreadUnsafe(
                std::unique_ptr<MMProf> &&mm_prof)
        {
            if (!mm_prof)
            {
                throw std::invalid_argument("mm_prof cannot be nullptr");
            }
            std::swap(mm_prof_, mm_prof);
            return std::move(mm_prof);
        }
        
        static std::unique_ptr<MMProf> mm_prof_;
#ifndef _M_CEE
        static std::mutex switch_mutex_;
#endif
    };
#ifndef _M_CEE
    /**
    Class for a scoped switch of memory manager profile. This class acts as a scoped 
    "guard" for changing the memory manager profile so that the programmer does 
    not have to explicitly switch back afterwards and that other threads cannot 
    change the MMProf. It can also help with exception safety by guaranteeing that 
    the profile is switched back to the original if a function throws an exception 
    after changing the profile for local use.
    */
    class MMProfGuard
    {
    public:
        /**
        Creates a new MMProfGuard. If start_locked is true, this function will 
        attempt to lock the MemoryManager for profile switch to mm_prof, perform 
        the switch, and keep the lock until unlocked or destroyed. If start_lock 
        is false, mm_prof will be stored but the switch will not be performed and 
        a lock will not be obtained until lock() is explicitly called.

        @param[in] mm_prof Pointer to a new memory manager profile
        @param[in] start_locked Bool indicating whether the lock should be
        immediately obtained (true by default)
        */
        MMProfGuard(std::unique_ptr<MMProf> &&mm_prof,
            bool start_locked = true) noexcept :
            mm_switch_lock_(MemoryManager::switch_mutex_,std::defer_lock)
        {
            if (start_locked)
            {
                lock(std::move(mm_prof));
            }
            else
            {
                old_prof_ = std::move(mm_prof);
            }
        }

        /**
        Creates a new MMProfGuard. If start_locked is true, this function will 
        attempt to lock the MemoryManager for profile switch to mm_prof, perform 
        the switch, and keep the lock until unlocked or destroyed. If start_lock 
        is false, mm_prof will be stored but the switch will not be performed and 
        a lock will not be obtained until lock() is explicitly called.

        @param[in] mm_prof Pointer to a new memory manager profile
        @param[in] start_locked Bool indicating whether the lock should be
        immediately obtained (true by default)
        */
        MMProfGuard(MMProf* &&mm_prof,
            bool start_locked = true) noexcept :
            mm_switch_lock_(MemoryManager::switch_mutex_, std::defer_lock)
        {
            if (start_locked)
            {
                lock(std::move(mm_prof));
            }
            else
            {
                old_prof_.reset(std::move(mm_prof));
            }
        }

        /**
        Attempts to lock the MemoryManager for profile switch, perform the switch 
        to currently stored memory manager profile, store the previously held profile, 
        and keep the lock until unlocked or destroyed. If the lock cannot be obtained 
        on the first attempt, the function returns false; otherwise returns true.

        @throws std::runtime_error if the lock is already owned
        */
        inline bool try_lock()
        {
            if (mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is already owned");
            }
            if (!mm_switch_lock_.try_lock())
            {
                return false;
            }
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(old_prof_));
            return true;
        }

        /**
        Locks the MemoryManager for profile switch, performs the switch to currently 
        stored memory manager profile, stores the previously held profile, and 
        keep the lock until unlocked or destroyed. The calling thread will block 
        until the lock can be obtained.

        @throws std::runtime_error if the lock is already owned
        */
        inline void lock()
        {
            if (mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is already owned");
            }
            mm_switch_lock_.lock();
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(old_prof_));
        }

        /**
        Attempts to lock the MemoryManager for profile switch, perform the switch 
        to the given memory manager profile, store the previously held profile, 
        and keep the lock until unlocked or destroyed. If the lock cannot be 
        obtained on the first attempt, the function returns false; otherwise 
        returns true.

        @param[in] mm_prof Pointer to a new memory manager profile
        @throws std::runtime_error if the lock is already owned
        */
        inline bool try_lock(
            std::unique_ptr<MMProf> &&mm_prof)
        {
            if (mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is already owned");
            }
            if (!mm_switch_lock_.try_lock())
            {
                return false;
            }
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(mm_prof));
            return true;
        }

        /**
        Locks the MemoryManager for profile switch, performs the switch to the given 
        memory manager profile, stores the previously held profile, and keep the 
        lock until unlocked or destroyed. The calling thread will block until the 
        lock can be obtained.

        @param[in] mm_prof Pointer to a new memory manager profile
        @throws std::runtime_error if the lock is already owned
        */
        inline void lock(
            std::unique_ptr<MMProf> &&mm_prof)
        {
            if (mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is already owned");
            }
            mm_switch_lock_.lock();
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(mm_prof));
        }

        /**
        Attempts to lock the MemoryManager for profile switch, perform the switch 
        to the given memory manager profile, store the previously held profile, 
        and keep the lock until unlocked or destroyed. If the lock cannot be 
        obtained on the first attempt, the function returns false; otherwise returns 
        true.

        @param[in] mm_prof Pointer to a new memory manager profile
        @throws std::runtime_error if the lock is already owned
        */
        inline bool try_lock(MMProf* &&mm_prof)
        {
            if (mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is already owned");
            }
            if (!mm_switch_lock_.try_lock())
            {
                return false;
            }
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(mm_prof));
            return true;
        }

        /**
        Locks the MemoryManager for profile switch, performs the switch to the 
        given memory manager profile, stores the previously held profile, and keep 
        the lock until unlocked or destroyed. The calling thread will block until 
        the lock can be obtained.

        @param[in] mm_prof Pointer to a new memory manager profile
        @throws std::runtime_error if the lock is already owned
        */
        inline void lock(MMProf* &&mm_prof)
        {
            if (mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is already owned");
            }
            mm_switch_lock_.lock();
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(mm_prof));
        }

        /**
        Releases the memory manager profile switch lock for MemoryManager, stores 
        the current profile, and resets the profile to the one used before locking.

        @throw std::runtime_error if the lock is not owned
        */
        inline void unlock()
        {
            if (!mm_switch_lock_.owns_lock())
            {
                throw std::runtime_error("lock is not owned");
            }
            old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                std::move(old_prof_));
            mm_switch_lock_.unlock();
        }

        /**
        Destroys the MMProfGuard. If the memory manager profile switch lock is 
        owned, releases the lock, and resets the profile to the one used before 
        locking.
        */
        ~MMProfGuard()
        {
            if (mm_switch_lock_.owns_lock())
            {
                old_prof_ = MemoryManager::SwitchProfileThreadUnsafe(
                    std::move(old_prof_));
                mm_switch_lock_.unlock();
            }
        }

        /**
        Returns whether the current MMProfGuard owns the memory manager profile 
        switch lock.
        */
        inline bool owns_lock() noexcept
        {
            return mm_switch_lock_.owns_lock();
        }

    private:
        std::unique_ptr<MMProf> old_prof_;

        std::unique_lock<std::mutex> mm_switch_lock_;
    };
#endif
}
