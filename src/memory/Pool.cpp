//
// Created by luiz0tavio on 4/29/18.
//

#include "Pool.hpp"

void* Memory::Pool::get(std::size_t size)
{
    if (mem_pool_ == nullptr) {
        std::cerr << "Memory pool not allocated" << std::endl;
    }
    if (pos_+size > size_) {
        pos_ = 0;
    }
    void *pointer = (uint8_t*)mem_pool_+pos_;
    pos_ += size;
    return pointer;
}

Memory::Pool::Pool(PoolType type, std::size_t size) {
    size_     = size;
    type_     = type;
    mem_pool_ = nullptr;
    pos_      = 0;

    mem_pool_ = std::malloc(size);
    if (mem_pool_ == nullptr) {
        std::cerr << "Error malloc memory pool" << std::endl;
    }
}

void Memory::Pool::destroy()
{
    std::free(mem_pool_);
}
