#ifndef INCLUDED_CORE_REFERENCE_HXX
#define INCLUDED_CORE_REFERENCE_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Object.hxx>

namespace Core
{
    template <typename T>
    class Reference final
    {
    public:
        using ReferencedType = T;

        // Be friend with other type references.
        template <typename U> friend class Reference;

    private:
        ReferencedType* m_Reference = nullptr;

    private:
        void InternalAddRef() const noexcept
        {
            if (this->m_Reference != nullptr)
            {
                this->m_Reference->AddRef();
            }
        }

        ThreadsafeCounterTrait::Type InternalReleaseRef() noexcept
        {
            ThreadsafeCounterTrait::Type ref_count{ 0 };

            auto* temp = this->m_Reference;

            if (temp != nullptr)
            {
                this->m_Reference = nullptr;
                ref_count = temp->ReleaseRef();
            }

            return ref_count;
        }

        void Swap(Reference<T>&& other) noexcept
        {
            auto* temp = this->m_Reference;
            this->m_Reference = other.m_Reference;
            other.m_Reference = temp;
        }

        void Swap(Reference<T>& other) noexcept
        {
            auto* temp = this->m_Reference;
            this->m_Reference = other.m_Reference;
            other.m_Reference = temp;
        }

    public:
        Reference() noexcept
            : m_Reference{ nullptr }
        {
        }
        Reference(std::nullptr_t) noexcept
            : m_Reference{ nullptr }
        {
        }

        template <typename U>
        Reference(U* other) noexcept
            : m_Reference{ other }
        {
            this->InternalAddRef();
        }

        Reference(const Reference<T>& other) noexcept
            : m_Reference{ other.m_Reference }
        {
            this->InternalAddRef();
        }

        template <typename U>
        Reference(const Reference<U>& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept
            : m_Reference{ other.m_Reference }
        {
            this->InternalAddRef();
        }

        Reference(Reference<T>&& other) noexcept
            : m_Reference{ nullptr }
        {
            if (this != reinterpret_cast<Reference*>(&reinterpret_cast<uint8_t&>(other)))
            {
                this->Swap(other);
            }
        }

        template <typename U>
        Reference(Reference<U>&& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept
            : m_Reference{ other.m_Reference }
        {
            other.m_Reference = nullptr;
        }

    public:
        ~Reference() noexcept
        {
            this->InternalReleaseRef();
        }

    public:
        Reference& operator = (std::nullptr_t) noexcept
        {
            this->InternalReleaseRef();
            return (*this);
        }

        Reference& operator = (T* other) noexcept
        {
            if (this->m_Reference != other)
            {
                Reference(other).Swap(*this);
            }

            return (*this);
        }

        template <typename U>
        Reference& operator = (U* other) noexcept
        {
            Reference(other).Swap(*this);
            return (*this);
        }

        Reference& operator = (const Reference<T>& other) noexcept
        {
            if (this->m_Reference != other.m_Reference)
            {
                Reference(other).Swap(*this);
            }

            return (*this);
        }

        template <typename U>
        Reference& operator = (const Reference<U>& other) noexcept
        {
            Reference(other).Swap(*this);
            return (*this);
        }

        Reference& operator = (Reference<T>&& other) noexcept
        {
            Reference(static_cast<Reference&&>(other)).Swap(*this);
            return (*this);
        }

        template <typename U>
        Reference& operator = (Reference<U>&& other) noexcept
        {
            Reference(static_cast<Reference<U>&&>(other)).Swap(*this);
            return (*this);
        }

    public:
        explicit operator bool() const noexcept
        {
            return this->m_Reference != nullptr;
        }

    public:
        operator T* () const noexcept
        {
            return this->m_Reference;
        }

        operator const T* () const noexcept
        {
            return this->m_Reference;
        }

        T& operator * () const noexcept
        {
            return *this->m_Reference;
        }

        T** operator & () noexcept
        {
            return &this->m_Reference;
        }

        T* operator -> () const noexcept
        {
            return this->m_Reference;
        }

    public:
        T* Get() const noexcept
        {
            return this->m_Reference;
        }

        T* const* GetAddressOf() const noexcept
        {
            return &this->m_Reference;
        }

        T** GetAddressOf() noexcept
        {
            return &this->m_Reference;
        }

        T** ReleaseAndGetAddressOf() noexcept
        {
            this->InternalReleaseRef();
            return &this->m_Reference;
        }

        T* Detach() noexcept
        {
            auto* reference = this->m_Reference;
            this->m_Reference = nullptr;
            return reference;
        }

        void Attach(T* other) noexcept
        {
            if (this->m_Reference != nullptr)
            {
                auto refcount = this->m_Reference->release_ref();
                (void)refcount;
                CORE_ASSERT(refcount != 0 || this->m_Reference != other);
            }

            this->m_Reference = other;
        }

        ThreadsafeCounterTrait::Type Reset() noexcept
        {
            return this->InternalReleaseRef();
        }

        bool CopyTo(T** reference) const noexcept
        {
            if (reference != nullptr)
            {
                this->InternalAddRef();
                (*reference) = this->m_Reference;
                return true;
            }

            return false;
        }

        bool IsValid() const noexcept
        {
            return this->m_Reference != nullptr;
        }

        template <typename U>
        bool As(Reference<U>& result) noexcept
        {
            result = static_cast<U*>(this->m_Reference);
            return true;
        }
    };

    template <typename T, typename U>
    inline bool operator == (const Reference<T>& l, const Reference<U>& r) noexcept
    {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "T and U pointers must be compatible");
        return l.Get() == r.Get();
    }

    template <typename T>
    inline bool operator == (const Reference<T>& l, std::nullptr_t) noexcept
    {
        return l.Get() == nullptr;
    }

    template <typename T>
    inline bool operator == (std::nullptr_t, const Reference<T>& r) noexcept
    {
        return nullptr == r.Get();
    }

    template <typename T, typename U>
    inline bool operator != (const Reference<T>& l, const Reference<U>& r) noexcept
    {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "T and U pointers must be compatible");
        return l.Get() != r.Get();
    }

    template <typename T>
    inline bool operator != (const Reference<T>& l, std::nullptr_t) noexcept
    {
        return l.Get() != nullptr;
    }

    template <typename T>
    inline bool operator != (std::nullptr_t, const Reference<T>& r) noexcept
    {
        return nullptr != r.Get();
    }

    template <typename T, typename U>
    inline bool operator > (const Reference<T>& l, const Reference<U>& r) noexcept
    {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "T and U pointers must be compatible");
        return l.Get() > r.Get();
    }

    template <typename T>
    inline bool operator > (const Reference<T>& l, std::nullptr_t) noexcept
    {
        return l.Get() > nullptr;
    }

    template <typename T>
    inline bool operator > (std::nullptr_t, const Reference<T>& r) noexcept
    {
        return nullptr > r.Get();
    }

    template <typename T, typename U>
    inline bool operator >= (const Reference<T>& l, const Reference<U>& r) noexcept
    {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "T and U pointers must be compatible");
        return l.Get() >= r.Get();
    }

    template <typename T>
    inline bool operator >= (const Reference<T>& l, std::nullptr_t) noexcept
    {
        return l.Get() >= nullptr;
    }

    template <typename T>
    inline bool operator >= (std::nullptr_t, const Reference<T>& r) noexcept
    {
        return nullptr >= r.Get();
    }

    template <typename T, typename U>
    inline bool operator < (const Reference<T>& l, const Reference<U>& r) noexcept
    {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "T and U pointers must be compatible");
        return l.Get() < r.Get();
    }

    template <typename T>
    inline bool operator < (const Reference<T>& l, std::nullptr_t) noexcept
    {
        return l.Get() < nullptr;
    }

    template <typename T>
    inline bool operator < (std::nullptr_t, const Reference<T>& r) noexcept
    {
        return nullptr < r.Get();
    }

    template <typename T, typename U>
    inline bool operator <= (const Reference<T>& l, const Reference<U>& r) noexcept
    {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "T and U pointers must be compatible");
        return l.Get() <= r.Get();
    }

    template <typename T>
    inline bool operator <= (const Reference<T>& l, std::nullptr_t) noexcept
    {
        return l.Get() <= nullptr;
    }

    template <typename T>
    inline bool operator <= (std::nullptr_t, const Reference<T>& r) noexcept
    {
        return nullptr <= r.Get();
    }

    template <typename T, typename ...Args>
    Reference<T> MakeRef(Args&&... args)
    {
        return Reference<T>{ new T(std::forward<Args>(args)...) };
    }
}

#endif // INCLUDED_CORE_REFERENCE_HXX
