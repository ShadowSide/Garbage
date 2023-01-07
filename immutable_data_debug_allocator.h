#ifndef IMMUTABLE_DATA_DEBUG_ALLOCATOR_H_
#define IMMUTABLE_DATA_DEBUG_ALLOCATOR_H_

#include <component/eka/error_handling/assert.h>
#include <windows.h>
#include <assert.h>
#include <deque>
#include <algorithm>
#include <mutex>

#define ALLOCATOR_ASSERT(expr) EKA_ASSERT(expr)

class ImmutableDataDebugAllocatorController
{
public:
    void* allocate(std::size_t size)
    {
        try
        {
            std::lock_guard<std::mutex> lock(m_guard);
            m_blocks.emplace_back();
            void* result = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
            if (!result)
            {
                m_blocks.pop_back();
                return nullptr;
            }
            auto& block = m_blocks.back();
            block.first = result;
            block.second = size;
            return result;
        }
        catch(...)
        {
            return nullptr;
        }
    }

    void deallocate(void* memory_ptr, std::size_t size)
    {
        std::lock_guard<std::mutex> lock(m_guard);
        auto blockIterator = std::find_if(m_blocks.cbegin(), m_blocks.cend(), [&](const auto& block) { return memory_ptr >= block.first && memory_ptr <= static_cast<uint8_t*>(block.first) + block.second; });
        if (blockIterator == m_blocks.cend())
        {
            ALLOCATOR_ASSERT(!"Can't deallocate. Invalid pointer");
            return;
        }
        ALLOCATOR_ASSERT(size <= blockIterator->second && "Wrong deallocation size");
        const bool result = VirtualFree(blockIterator->first, 0, MEM_RELEASE);
        ALLOCATOR_ASSERT(result && "Failed to deallocate");
        m_blocks.erase(blockIterator);
    }

    void Protect() const
    {
        std::lock_guard<std::mutex> lock(m_guard);
        for (auto block : m_blocks)
        {
            DWORD dummy = 0;
            const bool result = VirtualProtect(block.first, block.second, PAGE_READONLY, &dummy);
            ALLOCATOR_ASSERT(result && "Failed to VirtualProtect");
        }
    }

    void UnProtect() const
    {
        std::lock_guard<std::mutex> lock(m_guard);
        for (auto block : m_blocks)
        {
            DWORD dummy = 0;
            const bool result = VirtualProtect(block.first, block.second, PAGE_READWRITE, &dummy);
            ALLOCATOR_ASSERT(result && "Failed to VirtualProtect");
        }
    }

    virtual ~ImmutableDataDebugAllocatorController() { ALLOCATOR_ASSERT(m_blocks.empty() && "Leak detected"); }

private:
    mutable std::mutex m_guard;
    std::deque<std::pair<void*, std::size_t>> m_blocks;
};

class ImmutableDataDebugAllocatorUnProtector
{
public:
    explicit ImmutableDataDebugAllocatorUnProtector(std::shared_ptr<ImmutableDataDebugAllocatorController> controller) : m_controller(controller) {}
    ~ImmutableDataDebugAllocatorUnProtector() { m_controller->UnProtect(); }

private:
    const std::shared_ptr<ImmutableDataDebugAllocatorController> m_controller;
};

template<typename T>
class ImmutableDataDebugAllocator;

template <> class ImmutableDataDebugAllocator<void> {
public:
  typedef void* pointer;
  typedef const void* const_pointer;
  typedef void value_type;
  template <class U> struct rebind { typedef ImmutableDataDebugAllocator<U> other; };
};

template<typename T>
class ImmutableDataDebugAllocator
{
public:
    using value_type = T;

    template<typename U>
    friend class ImmutableDataDebugAllocator;
    
    template<typename U>
    struct rebind
    {
        using other = ImmutableDataDebugAllocator<U>;
    };
public:
    ImmutableDataDebugAllocator() = default;
    
    explicit ImmutableDataDebugAllocator(std::shared_ptr<ImmutableDataDebugAllocatorController> controller) noexcept
        : m_allocation_strategy(controller) {}
    
    ImmutableDataDebugAllocator(const ImmutableDataDebugAllocator& other) noexcept
        : m_allocation_strategy(other.m_allocation_strategy) {}
    
    template<typename U>
    ImmutableDataDebugAllocator(const ImmutableDataDebugAllocator<U>& other) noexcept
        : m_allocation_strategy(other.m_allocation_strategy) {}
    
    T* allocate(std::size_t count_objects)
    {
        ALLOCATOR_ASSERT(m_allocation_strategy && "Not initialized allocation strategy");
        return static_cast<T*>(m_allocation_strategy->allocate(count_objects * sizeof(T)));
    }
    
    void deallocate(void* memory_ptr, std::size_t count_objects)
    {
        ALLOCATOR_ASSERT(m_allocation_strategy && "Not initialized allocation strategy");
        m_allocation_strategy->deallocate(memory_ptr, count_objects * sizeof(T));
    }
    
    template<typename U, typename... Args>
    void construct(U* ptr, Args&&... args)
    {
        new (reinterpret_cast<void*>(ptr)) U { std::forward<Args>(args)... };
    }
    
    template<typename U>
    void destroy(U* ptr)
    {
        ptr->~U();
    }
private:
    std::shared_ptr<ImmutableDataDebugAllocatorController> m_allocation_strategy;
};

template<typename T, typename U>
bool operator==(const ImmutableDataDebugAllocator<T>& lhs, const ImmutableDataDebugAllocator<U>& rhs)
{
    return lhs.m_allocation_strategy == rhs.m_allocation_strategy;
}

template<typename T, typename U>
bool operator!=(const ImmutableDataDebugAllocator<T>& lhs, const ImmutableDataDebugAllocator<U>& rhs)
{
    return !(lhs == rhs);
}

#undef ALLOCATOR_ASSERT

#endif