//
// Created by luiz0tavio on 4/29/18.
//

#ifndef ONYX_PROVIDER_HPP
#define ONYX_PROVIDER_HPP

#include <map>
#include "Pool.hpp"

namespace Memory
{
    class Provider
    {

    private:

        /* Store memory pools by type.
         * This map perform a dynamic allocation. */
        static std::map <const PoolType,Pool*> _poolMap;

    public:

        /* Alloc memory pools.
         * Must call destroyPool() to avoid memory leak. */
        static void initPools();

        /* Destroy memory pool. */
        static void destroyPools() noexcept;

        /* Circular buffer-like memory gatherer by PoolType */
        static void* getMemory(PoolType type, std::size_t size);
    };
}

#endif //ONYX_PROVIDER_HPP
