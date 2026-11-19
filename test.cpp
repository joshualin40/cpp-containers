#include <gtest/gtest.h>
#include <print>
#include "list.h"

using namespace ft;
// Test: List(std::initializer_list<T> list);
TEST(list_test, initializer_list_constructor)
{
    List<int> myList{4, 5, 6};

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 6);
    ASSERT_TRUE(myList.validate());
}
// void push_back(const T &val)
TEST(list_test, push_back)
{
    List<int> myList;
    myList.push_back(4);

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 4);

    myList.push_back(5);

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 5);

    myList.push_back(6);

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 6);

    ASSERT_TRUE(myList.validate());
}

// 1st overload: iterator insert(const_iterator pos, const T &value);
TEST(list_test, insert_1)
{
    List<int> myList{4, 5, 6};

    auto it = myList.cbegin();
    EXPECT_EQ(*it, 4);

    // test insert into filled list
    myList.insert(it, 3);
    EXPECT_EQ(myList.front(), 3);

    auto it2 = ++myList.cbegin();
    myList.insert(it2, 9); // 3 9 4 5 6
    EXPECT_EQ(*(--it2), 9);

    // test insert into empty list
    List<int> myList2;
    myList2.insert(myList2.cend(), 4);
    EXPECT_EQ(myList2.front(), 4);
    EXPECT_EQ(myList2.back(), 4);

    ASSERT_TRUE(myList.validate());
    ASSERT_TRUE(myList2.validate());
}
// 2nd overload: iterator insert(const_iterator pos, size_type count, const T &value)
TEST(list_test, insert_2)
{
    List<int> myList{4, 5, 6};

    auto it = myList.cbegin();
    auto res = myList.insert(it, 5, 3);
    EXPECT_EQ(myList.front(), (*res));

    ASSERT_TRUE(myList.validate());
}

// 3rd overload:
//  Inserts count copies of value before pos.
TEST(list_test, insert_3)
{
    List<int> l{1, 2, 6, 7};

    auto pos = std::next(l.cbegin(), 2);
    auto it = l.insert(pos, 3, 9);

    // EXPECT_EQ(l, (List<int>{1, 2, 9, 9, 9, 6, 7}));
    EXPECT_EQ(*it, 9);
    EXPECT_EQ(*pos, 6);

    ASSERT_TRUE(l.validate());
}

TEST(list_test, emplace_back)
{
    List<int> l{1, 2};

    l.emplace_back(3);
    EXPECT_EQ(l.back(), 3);
}

TEST(list_test, pop_back)
{
    List<int> l{1, 2, 3};

    l.pop_back();
    EXPECT_EQ(l.back(), 2);
    l.pop_back();
    EXPECT_EQ(l.back(), 1);

    ASSERT_TRUE(l.validate());
}

TEST(list_test, push_front)
{
    List<int> l{1, 2, 3};

    l.push_front(0);
    EXPECT_EQ(l.front(), 0);

    // empty list
    List<int> l2;

    l2.push_front(0);
    EXPECT_EQ(l2.front(), 0);
}

TEST(list_test, erase)
{
    List<int> l{1, 2, 3, 4, 5};
    l.erase(l.cbegin()); 
    EXPECT_EQ(l.front(), 2); 
}