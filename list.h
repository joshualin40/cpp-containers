#include <cstddef>
#include <iterator>
#include <utility>
#include <iostream> 
namespace ft
{
    struct NodeBase
    {
        NodeBase *prev;
        NodeBase *next;
    };

    template <typename T>
    struct Node : NodeBase // Node "is-a" NodeBase
    {
        T val;

        Node(const T &val, NodeBase *prev = nullptr, NodeBase *next = nullptr) : val(val), NodeBase(prev, next) {}
    };

    template <typename T>
    class List
    {
    public:
        using size_type = size_t;
        using reference = T &;

        template <typename U>
        class ListIterator
        {
        public:
            friend class List;
            // Required standard iterator type aliases (C++17+ compliant)
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = U;
            using difference_type = std::ptrdiff_t;
            using pointer = U *;
            using reference = U &;

            explicit ListIterator(NodeBase *node = nullptr) : m_node(node) {}

            reference operator*() const { return static_cast<Node<U> *>(m_node)->val; }
            pointer operator->() const { return &(static_cast<Node<U> *>(m_node)->val); }

            // Pre-increment & Post-increment
            ListIterator &operator++() // pre: returns reference to incremented object
            {
                m_node = static_cast<Node<U> *>(m_node)->next;
                return *this;
            }
            ListIterator operator++(int) // post: returns object
            {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            // Pre-decrement & Post-decrement
            ListIterator &operator--()
            {
                m_node = static_cast<Node<U> *>(m_node)->prev;
                return *this;
            }
            ListIterator operator--(int)
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator==(const ListIterator &other) const { return m_node == other.m_node; }
            bool operator!=(const ListIterator &other) const { return m_node != other.m_node; }

        private:
            NodeBase *m_node;
        };

        using iterator = ListIterator<T>;
        using const_iterator = ListIterator<const T>;
        // constructors
        List()
        {
            sentinel = new NodeBase();
            sentinel->prev = sentinel;
            sentinel->next = sentinel;
            m_size = 0; 
        }

        List(std::initializer_list<T> list) : List()
        {
            for (T val : list)
            {
                push_back(val);
            }
        }

        // ELEMENT ACCESS
        T &front() { return static_cast<Node<T> *>(sentinel->next)->val; }
        T &back() { return static_cast<Node<T> *>(sentinel->prev)->val; }
        iterator begin() { return iterator(sentinel->next); };
        const_iterator cbegin() const { return const_iterator(sentinel->next); };
        iterator end() { return iterator(sentinel->prev); };
        const_iterator cend() const { return const_iterator(sentinel->prev); };

        // CAPACITY
        bool empty() { return (sentinel->next == sentinel); }
        size_type size() const noexcept { return m_size; }

        // MODIFIERS
        //  Inserts a copy of value before pos, and returns an iterator to the newly inserted element
        iterator insert(const_iterator pos, const T &value)
        {
            if (empty()) {
                Node<T> *toInsert = new Node(value, sentinel, sentinel);
                sentinel->next = toInsert; 
                sentinel->prev = toInsert; 
                m_size++; 
                return iterator(toInsert); 
            }
            // nonempty list
            auto before = --pos;
            auto after = ++pos;

            Node<T> *toInsert = new Node(value);
            before.m_node->next = toInsert;
            after.m_node->prev = toInsert;

            toInsert->prev = before.m_node;
            toInsert->next = after.m_node;
            m_size++;
            return iterator(toInsert);
        }

        // inserting elems
        // should return the iterator furthest from pos
        iterator insert(const_iterator pos, size_type count, const T &value)
        {
            if (count == 0)
                return iterator(pos.m_node);

            auto it = insert(pos, value);
            for (size_type i = 0; i < count - 1; ++i)
            {
                insert(pos, value);
            }

            return it;
        }

        //  Inserts elements from range [first, last) before pos.
        // returns: Iterator pointing to the first element inserted, or pos if first == last
        template <std::input_iterator InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last)
        { // inserting range
            if (first == last)
                return iterator(pos.m_node);

            auto it = first;
            while (it != last)
            {
                insert(pos, *it);
                it++;
            }
            return iterator(pos.m_node);
        }

        // Removes the element at pos.
        // returns Iterator following the last removed element.
        // If pos refers to the last element, then the end() iterator is returned.
        iterator erase(const_iterator pos)
        {
            auto target = pos.m_node;
            auto before = target->prev;
            auto after = target->next;

            before->next = after;
            after->prev = before;

            delete target;
            m_size--; 

            return iterator(after);
        }

        // Removes the elements in the range [first, last).
        iterator erase(const_iterator first, const_iterator last)
        {
        }

        void push_back(const T &val)
        {
            if (empty())
            {
                auto toInsert = new Node(val, sentinel, sentinel);
                sentinel->next = toInsert;
                sentinel->prev = toInsert;
                m_size++;
                return;
            }

            // non-empty
            auto temp = sentinel->prev;
            sentinel->prev = new Node(val, temp, sentinel);
            temp->next = sentinel->prev;

            m_size++; 
        }

        template <class... Args>
        reference emplace_back(Args &&...args)
        {
            auto temp = sentinel->prev; // old last node (or sentinel if empty)
            auto toInsert = new Node<T>(T(std::forward<Args>(args)...), temp, sentinel);
            temp->next = toInsert;
            sentinel->prev = toInsert;

            m_size++;
            return toInsert->val;
        }

        void pop_back()
        {
            auto toDelete = sentinel->prev;
            toDelete->prev->next = toDelete->next;
            toDelete->next->prev = toDelete->prev;
            delete toDelete;

            m_size--; 
        }

        void push_front(const T &val)
        {
            if (empty())
            {
                auto toInsert = new Node(val, sentinel, sentinel);
                sentinel->next = toInsert;
                sentinel->prev = toInsert;
                m_size++;
                return;
            }

            // non-empty
            auto temp = sentinel->next;
            sentinel->next = new Node(val, sentinel, temp);
            temp->prev = sentinel->next;

            m_size++; 
        }

        bool validate()
        { // helper to validate entire linked list
            // walk the full ring (including the link back into sentinel),
            // checking every next->prev pair and counting real nodes
            NodeBase *curr = sentinel;
            size_t s = 0;
            do
            {
                if (curr->next->prev != curr)
                    return false;
                curr = curr->next;
                if (curr != sentinel)
                    s++;
            } while (curr != sentinel);

            if (size() != s)
            {
                std::cout << "size mismatch: size()=" << size() << " counted=" << s << std::endl;
                return false;
            }
            return true;
        }

    private:
        NodeBase *sentinel;
        size_type m_size{}; // element count
    };
}
