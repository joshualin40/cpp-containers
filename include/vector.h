#include <cstddef>
#include <exception> 

namespace ft
{
    template <typename T>
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
            return m_last - m_first;
        }
        std::size_t capacity() const
        {
            return m_end - m_first;
        }

        // Modifiers
        void push_back(const T& value)
        {
            if (size() == capacity())
                size() == 0 ? reallocate(1) : reallocate(size() * 2);

            new (m_last) T(value);
            m_last++;
        }
        void push_back(T&& value) {
            if (size() == capacity())
                size() == 0 ? reallocate(1) : reallocate(size() * 2);
        
            new (m_last) T(std::move(value));
            m_last++;
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
            m_first = static_cast<T*>(malloc(mem * sizeof(T)));
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
            std::free(prev_buffer);
        }
        T *m_first = nullptr;
        T *m_last = nullptr; // points one past the last element
        T *m_end = nullptr; // points one past the total allocated memory 
    };
}