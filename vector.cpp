
template <typename T>
class CustomVector {
    public:
        void push_back(const T& val) {
            if (size() == capacity()) 
                reallocate(capacity() * 2);
            
            *(buffer + m_index) = val;
            m_index++; 
        } 
        size_t size() const {
            return m_index;
        }
        size_t capacity() const {
            return m_capacity; 
        }
    private:
        void reallocate(size_t mem) {
            T* prev_buffer = buffer; 
            buffer = static_cast<T*>(std::malloc(mem * sizeof(T)));

            if (buffer == NULL)
                throw std::runtime_error("Not enough memory");

            for (int i = 0; i < size(); ++i) {
                *(buffer + i) = *(prev_buffer + i);
            }
            m_capacity = mem; 
            delete prev_buffer; 
        } 

        T* buffer = nullptr; 
        size_t m_index = 0; 
        size_t m_capacity = 0; 
};
