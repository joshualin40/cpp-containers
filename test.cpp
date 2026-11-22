#include <gtest/gtest.h>
#include <iostream>
#include <concepts>
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
// range insert
TEST(list_test, insert_3)
{
    List<int> l{5,6,7,8};

    List<int> toInsert{1,2,3,4}; 
    auto it = l.insert(l.cbegin(), toInsert.begin(), toInsert.end()); 
    ASSERT_EQ((*it), 1); 
    ASSERT_EQ(l, (List<int>{1,2,3,4,5,6,7,8})); 

    List<int> toInsert2{0,1,2,3}; 
    auto it2 = l.insert(++l.cbegin(), toInsert2.begin(), ++toInsert2.begin()); 
    ASSERT_EQ((*it2), 0); 
    ASSERT_EQ(l, (List<int>{1,0,2,3,4,5,6,7,8})); 
    
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
    EXPECT_EQ(l, (List<int>{1,2})); 
    l.pop_back();
    EXPECT_EQ(l, (List<int>{1})); 

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
    EXPECT_EQ(l, (List<int>{2,3,4,5})); 
}
TEST(list_test, range_erase) {
    List<int> l{1, 2, 3, 4, 5};
    auto first = l.cbegin(); 
    auto second = l.cbegin()++; 
    l.erase(l.cbegin(), (l.cbegin() + 3)); 
    ASSERT_EQ(l, (List<int>{4,5})); 
}

TEST(list_test, destructor) 
{
    for (int i = 0; i < 100; ++i) { // build with ASanitizer
        List<int> l{1,2,3,4}; 
    }
}

TEST(list_test, copy_operation) {
    List<int> l1{1,2,3,4}; 
    List<int> l2; 
    l2 = l1; 
    ASSERT_EQ(l2, l1); 
}
TEST(list_test, comparison_operator) {
    List<int> l1{1,2,3,4}; 
    List<int> l2; 
    l2 = l1; 
    ASSERT_EQ(l2, l1); 
    l1.pop_back(); 
    ASSERT_FALSE(l2 == l1); 

     ASSERT_TRUE(l1.validate()); 
    ASSERT_TRUE(l2.validate()); 
}

TEST(list_test, iterator_addition) {
    List<int> l1{1,2,3,4}; 

    auto end = l1.begin() + 3; 
    ASSERT_EQ(*(l1.end() - 1), *end); 

    ASSERT_TRUE(l1.validate()); 
}

TEST(list_test, pop_front) {
    List<int> l{1,2,3,4}; 

    l.pop_front(); 
    ASSERT_EQ(l, (List<int>{2,3,4})); 
    ASSERT_TRUE(l.validate()); 
}


TEST(list_test, clear) {
    List<int> l{1,2,3,4}; 
    l.clear(); 
    ASSERT_TRUE(l.empty()); 
    ASSERT_EQ(l, (List<int>{})); 
}

TEST(list_test, reverse) {
    List<int> l{1,2,3,4}; 
    l.reverse(); 

    ASSERT_EQ(l, (List<int>{4,3,2,1})); 

    List<int> l2{1}; 
    l2.reverse(); 
    ASSERT_EQ(l2, (List<int>{1})); 
}

TEST(list_test, splice) {
    List<int> list1{1, 2, 3, 4, 5};
    List<int> list2{10,20,30,40,50};
    
    auto it = list1.cbegin();
    std::advance(it, 2);
    
    list1.splice(it,list2 );
    ASSERT_EQ(list1, (List<int>{1,2,10,20,30,40,50,3,4,5}));

    ASSERT_EQ(list1.size(), 10);
    ASSERT_EQ(list2.size(), 0);
    ASSERT_TRUE(list2.empty()); 
}

// list1: 1 2 10 20 30 40 50 3 4 5
// list2: