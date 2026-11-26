#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>
#include "vector.h"

using namespace ft;

namespace {
    // Counts live instances and copies so tests can check construction/destruction.
    struct Counted {
        static int alive, copies;
        int value;
        explicit Counted(int v) : value(v) { ++alive; }
        Counted(const Counted& o) : value(o.value) { ++alive; ++copies; }
        Counted(Counted&& o) noexcept : value(o.value) { ++alive; }
        ~Counted() { --alive; }
    };
    int Counted::alive = 0;
    int Counted::copies = 0;
}

TEST(vector_test, default_constructor) {
    Vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_THROW(v.at(0), std::out_of_range);
}

TEST(vector_test, push_back) {
    Vector<int> v;
    const std::size_t caps[] = {1, 2, 4, 4, 8};
    for (int i = 0; i < 5; ++i) {
        v.push_back(i);
        EXPECT_EQ(v.size(), i + 1u);
        EXPECT_EQ(v.capacity(), caps[i]);
    }
    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(v[i], i); // values survive reallocation
}

TEST(vector_test, push_back_rvalue_moves) {
    Counted::copies = 0;
    Vector<Counted> v;
    v.push_back(Counted(1));
    v.push_back(Counted(2)); // includes a reallocation
    EXPECT_EQ(Counted::copies, 0);
}

TEST(vector_test, push_back_own_element_when_full) {
    // the reference points into the buffer that's about to be reallocated
    Vector<std::string> v;
    v.push_back(std::string(64, 'a'));
    v.push_back(std::string(64, 'b'));
    ASSERT_EQ(v.size(), v.capacity());
    v.push_back(v[0]); // v[0] is a const_reference, ie const T& 
                    // if we need to resize, first need to save element 
    EXPECT_EQ(v[2], std::string(64, 'a'));
}

TEST(vector_test, at_and_subscript) {
    Vector<std::string> v;
    for (int i = 0; i < 5; ++i)
        v.push_back("item" + std::to_string(i));

    EXPECT_EQ(v[0], "item0");
    EXPECT_EQ(v.at(4), "item4");
    EXPECT_EQ(&v[2], &v.at(2));
    EXPECT_THROW(v.at(5), std::out_of_range); // past size, though within capacity
    EXPECT_THROW(v.at(static_cast<std::size_t>(-1)), std::out_of_range);

    v[1] = "x";
    v.at(3) = "y";
    EXPECT_EQ(v.at(1), "x");
    EXPECT_EQ(v[3], "y");

    const Vector<std::string>& cv = v;
    EXPECT_EQ(cv[1], "x");
    EXPECT_EQ(cv.at(3), "y");
    EXPECT_THROW(cv.at(5), std::out_of_range);
}

TEST(vector_test, front_and_back) {
    Vector<int> v;
    v.push_back(42);
    EXPECT_EQ(&v.front(), &v.back()); // single element

    v.push_back(1);
    v.push_back(2);
    EXPECT_EQ(v.front(), 42);
    EXPECT_EQ(v.back(), 2);
    EXPECT_EQ(v.back(), 2); // repeatable, doesn't modify
    EXPECT_EQ(v.size(), 3u);

    v.front() = 7;
    v.back() = 9;
    EXPECT_EQ(v[0], 7);
    EXPECT_EQ(v[2], 9);

    const Vector<int>& cv = v;
    EXPECT_EQ(cv.front(), 7);
    EXPECT_EQ(cv.back(), 9);
}

TEST(vector_test, pop_back) {
    Vector<int> v;
    for (int i = 0; i < 5; ++i)
        v.push_back(i);

    v.pop_back();
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v.back(), 3);
    EXPECT_EQ(v.capacity(), 8u); // never shrinks
    EXPECT_THROW(v.at(4), std::out_of_range);

    while (!v.empty())
        v.pop_back();
    EXPECT_EQ(v.capacity(), 8u);

    v.push_back(10); // reuses the buffer
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.capacity(), 8u);
}

TEST(vector_test, destroys_every_element_once) {
    Counted::alive = 0;
    {
        Vector<Counted> v;
        for (int i = 0; i < 5; ++i)
            v.push_back(Counted(i));
        EXPECT_EQ(Counted::alive, 5); // reallocation destroyed the old copies

        v.pop_back();
        EXPECT_EQ(Counted::alive, 4);
    }
    EXPECT_EQ(Counted::alive, 0);
}

TEST(vector_test, zero_size) {
    Vector<int> v;
    ASSERT_EQ(v.size(), 0);
    ASSERT_EQ(v.capacity(), 0); 
}

TEST(vector_test, push_back_lvalue_copies) {
    Counted::copies = 0;
    Vector<Counted> v;
    Counted c(5);
    v.push_back(c);
    EXPECT_EQ(Counted::copies, 1); // exactly one copy, none on reallocation
    EXPECT_EQ(c.value, 5);         // source untouched
    EXPECT_EQ(v[0].value, 5);
}

TEST(vector_test, push_back_move_only_type) {
    // must compile and work for types with no copy constructor
    Vector<std::unique_ptr<int>> v;
    for (int i = 0; i < 5; ++i)
        v.push_back(std::make_unique<int>(i));
    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(*v[i], i);
}

TEST(vector_test, push_back_many) {
    Vector<std::string> v;
    for (int i = 0; i < 1000; ++i)
        v.push_back(std::to_string(i));
    EXPECT_EQ(v.size(), 1000u);
    EXPECT_GE(v.capacity(), 1000u);
    for (int i = 0; i < 1000; ++i)
        EXPECT_EQ(v[i], std::to_string(i));
}

TEST(vector_test, elements_are_contiguous) {
    Vector<int> v;
    for (int i = 0; i < 10; ++i)
        v.push_back(i);
    for (std::size_t i = 0; i < v.size(); ++i)
        EXPECT_EQ(&v[i], &v[0] + i);
}

TEST(vector_test, no_reallocation_within_capacity) {
    Vector<int> v;
    for (int i = 0; i < 3; ++i)
        v.push_back(i);
    ASSERT_EQ(v.capacity(), 4u);
    int* before = &v[0];
    v.push_back(3); // fills the last free slot
    EXPECT_EQ(&v[0], before); // references stay valid
}

TEST(vector_test, empty_after_push_and_pop) {
    Vector<int> v;
    v.push_back(1);
    EXPECT_FALSE(v.empty());
    v.pop_back();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_THROW(v.at(0), std::out_of_range);
}

TEST(vector_test, const_capacity_queries) {
    Vector<int> v;
    v.push_back(1);
    const Vector<int>& cv = v;
    EXPECT_FALSE(cv.empty());
    EXPECT_EQ(cv.size(), 1u);
    EXPECT_EQ(cv.capacity(), 1u);
}
