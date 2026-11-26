#include <cstddef>
#include <exception> 
#include <limits> 
#include <utility> 
#include "allocator.h"

namespace ft
{
    template <typename T, typename Allocator = ft::Allocator<T>>
    class Vector
    {
    public:
        using reference = T&; 
        using const_reference = const T&; 
        Vector() = default;
        // explicit Vector(size_type count,
        //                 const Allocator &alloc = Allocator());
        // Vector(size_type count, const T &value,
        //        const Allocator &alloc = Allocator());

        // implement copy/move operations
        
        // Element access
        reference at(std::size_t pos ) {
            if (pos >= size()) // remember it is zero indexed
                throw std::out_of_range("Out of range"); 
            return *(m_first + pos); 
        }
        const_reference at(std::size_t pos) const {
            if (pos >= size()) // remember it is zero indexed
                throw std::out_of_range("Out of range"); 
            return *(m_first + pos); 
        }
        reference operator[]( std::size_t pos ) {
            return *(m_first + pos); 
        }
        const_reference operator[](std::size_t pos) const {
            return *(m_first + pos); 
        }
        reference front() {
            return *(m_first); 
        }
        const_reference front() const {
            return *(m_first); 
        }
        reference back() {
            return *(m_last - 1); 
        }
        const_reference back() const {
            return *(m_last - 1); 
        }

        // Capacity
        bool empty() const
        {
            return m_first == m_last;
        }
        std::size_t size() const
        {
            if (m_first == nullptr) return 0;
            return m_last - m_first;
        }
        std::size_t capacity() const
        {
            if (m_first == nullptr) return 0;
            return m_end - m_first;
        }

        // Modifiers
        void push_back(const T& value)
        {
            emplace_back(value); 
        }
        void push_back(T&& value) {
            emplace_back(std::move(value)); 
        }

        template <typename... Args>
        T& emplace_back(Args&&... args) {
            if (size() == capacity()) {
                T temp(std::forward<Args>(args)...); //perfectly forward args that were passed in
                                                    //  to invoke correct constructor
                size() == 0 ? reallocate(1) : reallocate(size() * 2);
                m_allocator.construct(m_last++,  std::move(temp)); 
            }
            else {
                m_allocator.construct(m_last++,  std::forward<Args>(args)...); // preserves value category what we're emplacing
            }
            return *std::prev(m_last);
        }


        void pop_back() {
            m_last--; 
            m_last->~T(); 
        }
        
        ~Vector() {
            for (int i = 0; i < size(); ++i)
            {
                m_first[i].~T(); 
            }
            std::free(m_first); 
        }
    private:
        void reallocate(size_t mem) 
        {       
            T* prev_buffer = m_first;
            size_t prev_size = size(); 

            m_first = m_allocator.allocate(mem); // using ft::Allocator
            m_last = m_first + prev_size; // size stays the same
            m_end = m_first + mem;

            for (int i = 0; i < prev_size; ++i)
            {
                new (m_first + i) T(std::move(prev_buffer[i])); // move old contents into new buffer
            }

            if (prev_buffer == nullptr) // there were no elements previously
                return;

            for (int i = 0; i < prev_size; ++i)
            {
                (prev_buffer + i)->~T(); // destroy all previous elements
            }
            m_allocator.deallocate(prev_buffer, prev_size * sizeof(T)); 
        }
        T *m_first = nullptr;
        T *m_last = nullptr; // points one past the last element
        T *m_end = nullptr; // points one past the total allocated memory 
        [[no_unique_address]] Allocator m_allocator; 
    };
} 