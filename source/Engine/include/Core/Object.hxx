#ifndef INCLUDED_CORE_OBJECT_HXX
#define INCLUDED_CORE_OBJECT_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core
{
    class ReferenceCounterTrait final
    {
    public:
        using Type = int32_t;

        static __forceinline Type Increment(Type& value) noexcept
        {
            return ++value;
        }

        static __forceinline Type Decrement(Type& value) noexcept
        {
            return --value;
        }
    };

    class ThreadsafeCounterTrait final
    {
    public:
        using Type = int32_t;

        static __forceinline Type Increment(Type& value) noexcept
        {
            return ::InterlockedIncrement(reinterpret_cast<::LONG volatile*>(&value));
        }

        static __forceinline Type Decrement(Type& value) noexcept
        {
            return ::InterlockedDecrement(reinterpret_cast<::LONG volatile*>(&value));
        }
    };
}

namespace Core
{
    class Object
    {
    private:
        ThreadsafeCounterTrait::Type m_ReferenceCount{ 0 };

    public:
        Object() noexcept = default;
        virtual ~Object() noexcept = default;

    public:
        __forceinline ThreadsafeCounterTrait::Type AddRef() noexcept
        {
            return ThreadsafeCounterTrait::Increment(this->m_ReferenceCount);
        }

        __forceinline ThreadsafeCounterTrait::Type ReleaseRef() noexcept
        {
            if (ThreadsafeCounterTrait::Decrement(this->m_ReferenceCount) == ThreadsafeCounterTrait::Type{ 0 })
            {
                delete this;
                return ThreadsafeCounterTrait::Type{ 0 };
            }

            return this->m_ReferenceCount;
        }

        __forceinline ThreadsafeCounterTrait::Type GetReferenceCount() noexcept
        {
            return this->m_ReferenceCount;
        }
    };
}

#endif // INCLUDED_CORE_OBJECT_HXX
