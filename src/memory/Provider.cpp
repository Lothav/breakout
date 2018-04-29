//
// Created by luiz0tavio on 4/29/18.
//

#include "Provider.hpp"

/* Init static member with a empty map */
std::map <const Memory::PoolType, Memory::Pool*> Memory::Provider::_poolMap = {};

void Memory::Provider::initPools()
{
    for (auto& poolInfo: PoolsInfo) {
        _poolMap[poolInfo.type] = new Memory::Pool(poolInfo.type, poolInfo.size);
    }
}

void* Memory::Provider::getMemory(Memory::PoolType type, std::size_t size)
{
    return _poolMap[type]->get(size);
}

void Memory::Provider::destroyPools () noexcept
{
    for(auto& pool : _poolMap) {
        pool.second->destroy();
    }
};

