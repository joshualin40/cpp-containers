#include <gtest/gtest.h>
#include <iostream>
#include <concepts>
#include <string>
#include <vector>
#include "list.h"

// Turn on LeakSanitizer by default (it's off by default on macOS).
// Apple clang has no LeakSanitizer, so skip it there.
#if !defined(__apple_build_version__)
extern "C" const char* __asan_default_options() { return "detect_leaks=1"; }
#endif

namespace ft
{
    // Lets gtest print list contents in failure messages instead of raw bytes.
    template <typename T>
    void PrintTo(const List<T> &l, std::ostream *os)
    {
        *os << "{";
        for (auto it = l.cbegin(); it != l.cend(); ++it)
            *os << (it == l.cbegin() ? "" : ", ") << ::testing::PrintToString(*it);
        *os << "}";
    }
}

using namespace ft;
// Test: List(std::initializer_list<T> list);
TEST(list_test, initializer_list_constructor)
{
    List<int> myList{4, 5, 6};

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 6);
    EXPECT_EQ(myList.size(), 3);
    ASSERT_TRUE(myList.validate());

    // edge: empty braces -> default constructor
    List<int> empty{};
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), 0);
    EXPECT_TRUE(empty.begin() == empty.end());
    ASSERT_TRUE(empty.validate());

    // edge: explicitly empty initializer_list
    List<int> emptyIl(std::initializer_list<int>{});
    EXPECT_TRUE(emptyIl.empty());
    EXPECT_EQ(emptyIl.size(), 0);
    ASSERT_TRUE(emptyIl.validate());

    // edge: single element -> front and back are the same node
    List<int> single{42};
    EXPECT_EQ(single.size(), 1);
    EXPECT_EQ(single.front(), 42);
    EXPECT_EQ(single.back(), 42);
    EXPECT_EQ(&single.front(), &single.back());
    ASSERT_TRUE(single.validate());

    // edge: duplicates keep their order and count
    List<int> dups{7, 7, 7};
    EXPECT_EQ(dups.size(), 3);
    EXPECT_EQ(dups, (List<int>{7, 7, 7}));

    // edge: non-trivial element type
    List<std::string> strs{"a", "bb", ""};
    EXPECT_EQ(strs.size(), 3);
    EXPECT_EQ(strs.front(), "a");
    EXPECT_EQ(strs.back(), "");
    ASSERT_TRUE(strs.validate());
}
// void push_back(const T &val)
TEST(list_test, push_back)
{
    List<int> myList;
    myList.push_back(4);

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 4);
    EXPECT_EQ(myList.size(), 1);

    myList.push_back(5);

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 5);
    EXPECT_EQ(myList.size(), 2);

    myList.push_back(6);

    EXPECT_EQ(myList.front(), 4);
    EXPECT_EQ(myList.back(), 6);
    EXPECT_EQ(myList.size(), 3);

    ASSERT_TRUE(myList.validate());

    // edge: push_back a reference to an element already in the list
    myList.push_back(myList.front());
    EXPECT_EQ(myList, (List<int>{4, 5, 6, 4}));

    // edge: push_back after clear() reuses the empty list correctly
    myList.clear();
    myList.push_back(1);
    EXPECT_EQ(myList.size(), 1);
    EXPECT_EQ(myList.front(), 1);
    EXPECT_EQ(myList.back(), 1);
    ASSERT_TRUE(myList.validate());

    // edge: many elements
    List<int> big;
    const int n = 1000;
    for (int i = 0; i < n; ++i)
        big.push_back(i);
    EXPECT_EQ(big.size(), n);
    EXPECT_EQ(big.front(), 0);
    EXPECT_EQ(big.back(), n - 1);
    long sum = 0;
    for (int x : big)
        sum += x;
    EXPECT_EQ(sum, static_cast<long>(n) * (n - 1) / 2);

    // edge: non-trivial element type (ASan catches leaks/double frees)
    List<std::string> strs;
    strs.push_back("hello");
    strs.push_back(std::string(100, 'x'));
    EXPECT_EQ(strs.size(), 2);
    EXPECT_EQ(strs.back().size(), 100);
}

// 1st overload: iterator insert(const_iterator pos, const T &value);
TEST(list_test, insert_1)
{
    List<int> myList{4, 5, 6};

    auto it = myList.cbegin();
    EXPECT_EQ(*it, 4);

    // test insert into filled list
    auto ret = myList.insert(it, 3);
    EXPECT_EQ(myList.front(), 3);
    EXPECT_EQ(*ret, 3);
    EXPECT_TRUE(ret == myList.begin());

    // edge: the iterator passed in still refers to the same element
    EXPECT_EQ(*it, 4);

    auto it2 = std::next(myList.cbegin());
    myList.insert(it2, 9); // 3 9 4 5 6
    EXPECT_EQ(*(std::prev(it2)), 9);
    EXPECT_EQ(myList, (List<int>{3, 9, 4, 5, 6}));

    // edge: insert at end() of a non-empty list appends
    auto retEnd = myList.insert(myList.cend(), 7);
    EXPECT_EQ(*retEnd, 7);
    EXPECT_EQ(myList.back(), 7);
    EXPECT_EQ(myList.size(), 6);
    EXPECT_EQ(myList, (List<int>{3, 9, 4, 5, 6, 7}));

    // edge: insert before the last element
    myList.insert(std::prev(myList.cend()), 8);
    EXPECT_EQ(myList, (List<int>{3, 9, 4, 5, 6, 8, 7}));

    // test insert into empty list
    List<int> myList2;
    auto ret2 = myList2.insert(myList2.cend(), 4);
    EXPECT_EQ(*ret2, 4);
    EXPECT_EQ(myList2.front(), 4);
    EXPECT_EQ(myList2.back(), 4);
    EXPECT_EQ(myList2.size(), 1);

    // edge: insert at begin() and end() of a single-element list
    myList2.insert(myList2.cbegin(), 3);
    myList2.insert(myList2.cend(), 5);
    EXPECT_EQ(myList2, (List<int>{3, 4, 5}));

    // edge: insert into empty list via cbegin() (== cend())
    List<int> myList3;
    myList3.insert(myList3.cbegin(), 1);
    EXPECT_EQ(myList3, (List<int>{1}));

    ASSERT_TRUE(myList.validate());
    ASSERT_TRUE(myList2.validate());
    ASSERT_TRUE(myList3.validate());
}
// 2nd overload: iterator insert(const_iterator pos, size_type count, const T &value)
TEST(list_test, insert_2)
{
    List<int> myList{4, 5, 6};

    auto it = myList.cbegin();
    auto res = myList.insert(it, 5, 3);
    EXPECT_EQ(myList.front(), (*res));
    EXPECT_TRUE(res == myList.begin());
    EXPECT_EQ(myList.size(), 8);
    EXPECT_EQ(myList, (List<int>{3, 3, 3, 3, 3, 4, 5, 6}));

    ASSERT_TRUE(myList.validate());

    // edge: count == 0 is a no-op and returns pos
    List<int> l{1, 2, 3};
    auto pos = std::next(l.cbegin());
    auto res0 = l.insert(pos, 0, 99);
    EXPECT_EQ(*res0, 2);
    EXPECT_EQ(l.size(), 3);
    EXPECT_EQ(l, (List<int>{1, 2, 3}));

    // edge: count == 1 behaves like single insert
    auto res1 = l.insert(pos, 1, 99);
    EXPECT_EQ(*res1, 99);
    EXPECT_EQ(l, (List<int>{1, 99, 2, 3}));

    // edge: insert at end(); returned iterator is the first inserted
    auto resEnd = l.insert(l.cend(), 3, 0);
    EXPECT_EQ(*resEnd, 0);
    EXPECT_TRUE(std::prev(resEnd) == std::prev(l.end(), 4));
    EXPECT_EQ(l, (List<int>{1, 99, 2, 3, 0, 0, 0}));
    ASSERT_TRUE(l.validate());

    // edge: count insert into an empty list
    List<int> empty;
    auto resEmpty = empty.insert(empty.cend(), 4, 8);
    EXPECT_TRUE(resEmpty == empty.begin());
    EXPECT_EQ(empty.size(), 4);
    EXPECT_EQ(empty, (List<int>{8, 8, 8, 8}));
    ASSERT_TRUE(empty.validate());

    // edge: count == 0 on an empty list returns end()
    List<int> empty2;
    auto resEmpty2 = empty2.insert(empty2.cend(), 0, 8);
    EXPECT_TRUE(resEmpty2 == empty2.end());
    EXPECT_TRUE(empty2.empty());
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
    // source range is untouched
    ASSERT_EQ(toInsert, (List<int>{1,2,3,4}));

    List<int> toInsert2{0,1,2,3};
    auto it2 = l.insert(std::next(l.cbegin()), toInsert2.begin(), std::next(toInsert2.begin()));
    ASSERT_EQ((*it2), 0);
    ASSERT_EQ(l, (List<int>{1,0,2,3,4,5,6,7,8}));

    ASSERT_TRUE(l.validate());

    // edge: empty range is a no-op and returns pos
    auto pos = std::next(l.cbegin(), 2);
    auto itEmpty = l.insert(pos, toInsert2.begin(), toInsert2.begin());
    ASSERT_EQ(*itEmpty, 2);
    ASSERT_EQ(l.size(), 9);
    ASSERT_EQ(l, (List<int>{1,0,2,3,4,5,6,7,8}));

    // edge: insert range at end()
    List<int> tail{9, 10};
    auto itEnd = l.insert(l.cend(), tail.begin(), tail.end());
    ASSERT_EQ(*itEnd, 9);
    ASSERT_EQ(l.back(), 10);
    ASSERT_EQ(l.size(), 11);
    ASSERT_TRUE(l.validate());

    // edge: insert range into an empty list
    List<int> empty;
    auto itE = empty.insert(empty.cend(), toInsert.begin(), toInsert.end());
    ASSERT_TRUE(itE == empty.begin());
    ASSERT_EQ(empty, (List<int>{1,2,3,4}));
    ASSERT_TRUE(empty.validate());

    // edge: range from a different container type
    std::vector<int> v{100, 200, 300};
    List<int> fromVec{1, 2};
    auto itV = fromVec.insert(std::next(fromVec.cbegin()), v.begin(), v.end());
    ASSERT_EQ(*itV, 100);
    ASSERT_EQ(fromVec, (List<int>{1, 100, 200, 300, 2}));
    ASSERT_TRUE(fromVec.validate());

    // edge: empty range into an empty list returns end()
    List<int> empty2;
    std::vector<int> ev;
    auto itEE = empty2.insert(empty2.cend(), ev.begin(), ev.end());
    ASSERT_TRUE(itEE == empty2.end());
    ASSERT_TRUE(empty2.empty());
}

TEST(list_test, emplace_back)
{
    List<int> l{1, 2};

    l.emplace_back(3);
    EXPECT_EQ(l.back(), 3);
    EXPECT_EQ(l.size(), 3);
    EXPECT_EQ(l, (List<int>{1, 2, 3}));

    // edge: returned reference refers to the stored element
    int &ref = l.emplace_back(4);
    EXPECT_EQ(&ref, &l.back());
    ref = 40;
    EXPECT_EQ(l.back(), 40);

    // edge: emplace_back into an empty list
    List<int> empty;
    empty.emplace_back(7);
    EXPECT_EQ(empty.size(), 1);
    EXPECT_EQ(empty.front(), 7);
    EXPECT_EQ(empty.back(), 7);
    ASSERT_TRUE(empty.validate());

    // edge: multiple constructor arguments are forwarded
    List<std::string> strs;
    strs.emplace_back(3, 'z');
    strs.emplace_back("abc");
    EXPECT_EQ(strs.front(), "zzz");
    EXPECT_EQ(strs.back(), "abc");
    EXPECT_EQ(strs.size(), 2);
    ASSERT_TRUE(strs.validate());

    ASSERT_TRUE(l.validate());
}

TEST(list_test, pop_back)
{
    List<int> l{1, 2, 3};

    l.pop_back();
    EXPECT_EQ(l, (List<int>{1,2}));
    EXPECT_EQ(l.size(), 2);
    l.pop_back();
    EXPECT_EQ(l, (List<int>{1}));
    EXPECT_EQ(l.front(), l.back());

    ASSERT_TRUE(l.validate());

    // edge: pop the last remaining element -> empty
    l.pop_back();
    EXPECT_TRUE(l.empty());
    EXPECT_EQ(l.size(), 0);
    EXPECT_TRUE(l.begin() == l.end());
    ASSERT_TRUE(l.validate());

    // edge: list is reusable after being popped empty
    l.push_back(5);
    l.push_front(4);
    EXPECT_EQ(l, (List<int>{4, 5}));
    ASSERT_TRUE(l.validate());

    // edge: pop_back on non-trivial type frees memory (ASan)
    List<std::string> strs{"a", std::string(64, 'b')};
    strs.pop_back();
    EXPECT_EQ(strs.size(), 1);
    EXPECT_EQ(strs.back(), "a");
}

TEST(list_test, push_front)
{
    List<int> l{1, 2, 3};

    l.push_front(0);
    EXPECT_EQ(l.front(), 0);
    EXPECT_EQ(l.size(), 4);
    EXPECT_EQ(l, (List<int>{0, 1, 2, 3}));

    // empty list
    List<int> l2;

    l2.push_front(0);
    EXPECT_EQ(l2.front(), 0);
    EXPECT_EQ(l2.back(), 0);
    EXPECT_EQ(l2.size(), 1);

    // edge: repeated push_front reverses insertion order
    l2.push_front(-1);
    l2.push_front(-2);
    EXPECT_EQ(l2, (List<int>{-2, -1, 0}));
    EXPECT_EQ(l2.back(), 0);

    // edge: interleaving push_front and push_back
    List<int> l3;
    l3.push_back(2);
    l3.push_front(1);
    l3.push_back(3);
    l3.push_front(0);
    EXPECT_EQ(l3, (List<int>{0, 1, 2, 3}));

    // edge: push_front a reference to an element already in the list
    l3.push_front(l3.back());
    EXPECT_EQ(l3, (List<int>{3, 0, 1, 2, 3}));

    ASSERT_TRUE(l.validate());
    ASSERT_TRUE(l2.validate());
    ASSERT_TRUE(l3.validate());
}

TEST(list_test, erase)
{
    List<int> l{1, 2, 3, 4, 5};
    auto ret = l.erase(l.cbegin());
    EXPECT_EQ(l, (List<int>{2,3,4,5}));
    EXPECT_EQ(*ret, 2);
    EXPECT_TRUE(ret == l.begin());
    EXPECT_EQ(l.size(), 4);

    // edge: erase from the middle returns the following element
    auto mid = std::next(l.cbegin()); // 3
    auto retMid = l.erase(mid);
    EXPECT_EQ(*retMid, 4);
    EXPECT_EQ(l, (List<int>{2, 4, 5}));

    // edge: erase the last element returns end()
    auto retLast = l.erase(std::prev(l.cend()));
    EXPECT_TRUE(retLast == l.end());
    EXPECT_EQ(l, (List<int>{2, 4}));
    EXPECT_EQ(l.back(), 4);
    ASSERT_TRUE(l.validate());

    // edge: erase the only element -> empty, returns end()
    List<int> single{9};
    auto retOnly = single.erase(single.cbegin());
    EXPECT_TRUE(retOnly == single.end());
    EXPECT_TRUE(single.empty());
    EXPECT_EQ(single.size(), 0);
    ASSERT_TRUE(single.validate());

    // edge: erase everything one at a time using the returned iterator
    List<int> all{1, 2, 3, 4};
    auto cur = all.cbegin();
    while (cur != all.cend())
    {
        auto next = all.erase(cur);
        cur = all.cbegin(); // next == begin() after erasing the front
        EXPECT_TRUE(next == all.begin());
    }
    EXPECT_TRUE(all.empty());
    ASSERT_TRUE(all.validate());

    // edge: other iterators stay valid after an erase
    List<int> stable{1, 2, 3};
    auto keep = std::next(stable.cbegin(), 2); // 3
    stable.erase(stable.cbegin());
    EXPECT_EQ(*keep, 3);

    // edge: erasing a non-trivial type must run its destructor (ASan leak check)
    List<std::string> strs{std::string(64, 'a'), std::string(64, 'b'), "c"};
    strs.erase(strs.cbegin());
    strs.erase(strs.cbegin(), std::prev(strs.cend()));
    EXPECT_EQ(strs, (List<std::string>{"c"}));

}
TEST(list_test, range_erase) {
    List<int> l{1, 2, 3, 4, 5};
    auto first = l.cbegin();
    auto second = std::next(l.cbegin());
    auto ret = l.erase(l.cbegin(), (l.cbegin() + 3));
    ASSERT_EQ(l, (List<int>{4,5}));
    ASSERT_EQ(*ret, 4);
    ASSERT_EQ(l.size(), 2);
    ASSERT_TRUE(l.validate());

    // edge: empty range (first == last) is a no-op and returns last
    List<int> l2{1, 2, 3};
    auto p = std::next(l2.cbegin());
    auto retEmpty = l2.erase(p, p);
    ASSERT_EQ(*retEmpty, 2);
    ASSERT_EQ(l2, (List<int>{1, 2, 3}));
    ASSERT_EQ(l2.size(), 3);

    // edge: erase a single-element range
    auto retOne = l2.erase(p, std::next(p));
    ASSERT_EQ(*retOne, 3);
    ASSERT_EQ(l2, (List<int>{1, 3}));

    // edge: erase up to end() returns end()
    List<int> l3{1, 2, 3, 4};
    auto retTail = l3.erase(std::next(l3.cbegin()), l3.cend());
    ASSERT_TRUE(retTail == l3.end());
    ASSERT_EQ(l3, (List<int>{1}));
    ASSERT_EQ(l3.back(), 1);
    ASSERT_TRUE(l3.validate());

    // edge: erase the whole list
    List<int> l4{1, 2, 3, 4};
    auto retAll = l4.erase(l4.cbegin(), l4.cend());
    ASSERT_TRUE(retAll == l4.end());
    ASSERT_TRUE(l4.empty());
    ASSERT_EQ(l4.size(), 0);
    ASSERT_TRUE(l4.validate());

    // edge: empty range on an empty list
    List<int> l5;
    auto retE = l5.erase(l5.cbegin(), l5.cend());
    ASSERT_TRUE(retE == l5.end());
    ASSERT_TRUE(l5.empty());

    // edge: erased list is reusable
    l4.push_back(7);
    ASSERT_EQ(l4, (List<int>{7}));
    ASSERT_TRUE(l4.validate());
}

TEST(list_test, destructor)
{
    for (int i = 0; i < 100; ++i) { // build with ASanitizer
        List<int> l{1,2,3,4};
    }

    // edge: destroying an empty list
    for (int i = 0; i < 100; ++i) {
        List<int> l;
    }

    // edge: destroying a single-element list
    {
        List<int> l{1};
    }

    // edge: destroying a list that was cleared / emptied by pops
    {
        List<int> l{1, 2, 3};
        l.clear();
    }
    {
        List<int> l{1, 2};
        l.pop_back();
        l.pop_front();
    }

    // edge: destroying a large list
    {
        List<int> l;
        for (int i = 0; i < 10000; ++i)
            l.push_back(i);
    }

    // edge: non-trivial element type must have its destructor run
    for (int i = 0; i < 100; ++i) {
        List<std::string> l{std::string(64, 'a'), std::string(64, 'b')};
        l.emplace_back(64, 'c');
    }

    // edge: both lists destroyed after a splice (no double free / leak)
    {
        List<int> a{1, 2};
        List<int> b{3, 4};
        a.splice(a.cend(), b);
    }

    // edge: list destroyed after reverse()
    {
        List<int> l{1, 2, 3};
        l.reverse();
    }
}

TEST(list_test, copy_operation) {
    List<int> l1{1,2,3,4};
    List<int> l2;
    l2 = l1;
    ASSERT_EQ(l2, l1);
    ASSERT_EQ(l2.size(), 4);
    ASSERT_TRUE(l2.validate());

    // edge: copy is deep -- changing one does not affect the other
    l1.front() = 100;
    l1.push_back(5);
    ASSERT_EQ(l2, (List<int>{1,2,3,4}));
    l2.pop_front();
    ASSERT_EQ(l1, (List<int>{100,2,3,4,5}));

    // edge: copying an empty list into an empty list
    List<int> e1, e2;
    e2 = e1;
    ASSERT_TRUE(e2.empty());
    ASSERT_EQ(e2.size(), 0);
    ASSERT_TRUE(e2.validate());

    // edge: assigning to a non-empty list replaces its contents
    List<int> src{7, 8};
    List<int> dst{1, 2, 3};
    dst = src;
    ASSERT_EQ(dst, src);
    ASSERT_EQ(dst.size(), 2);
    ASSERT_TRUE(dst.validate());

    // edge: assigning an empty list clears the destination
    List<int> dst2{1, 2, 3};
    List<int> emptySrc;
    dst2 = emptySrc;
    ASSERT_TRUE(dst2.empty());
    ASSERT_EQ(dst2.size(), 0);

    // edge: self-assignment leaves the list unchanged
    List<int> self{1, 2, 3};
    List<int> &selfRef = self; // avoid -Wself-assign-overloaded
    self = selfRef;
    ASSERT_EQ(self, (List<int>{1, 2, 3}));
    ASSERT_EQ(self.size(), 3);
    ASSERT_TRUE(self.validate());

    // edge: chained assignment
    List<int> a{1, 2}, b, c;
    c = b = a;
    ASSERT_EQ(b, a);
    ASSERT_EQ(c, a);

    // edge: non-trivial element type
    List<std::string> s1{"x", "yy"};
    List<std::string> s2;
    s2 = s1;
    s1.front() = "changed";
    ASSERT_EQ(s2, (List<std::string>{"x", "yy"}));
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

    // edge: comparison is symmetric
    ASSERT_FALSE(l1 == l2);

    // edge: a list equals itself
    ASSERT_TRUE(l1 == l1);

    // edge: two empty lists are equal
    List<int> e1, e2;
    ASSERT_TRUE(e1 == e2);

    // edge: empty vs non-empty
    ASSERT_FALSE(e1 == l1);
    ASSERT_FALSE(l1 == e1);

    // edge: same size, differing only in the first element
    ASSERT_FALSE((List<int>{0,2,3}) == (List<int>{1,2,3}));

    // edge: same size, differing only in the last element
    ASSERT_FALSE((List<int>{1,2,3}) == (List<int>{1,2,4}));

    // edge: same elements, different order
    ASSERT_FALSE((List<int>{1,2,3}) == (List<int>{3,2,1}));

    // edge: one list is a prefix of the other
    ASSERT_FALSE((List<int>{1,2}) == (List<int>{1,2,3}));

    // edge: works on const lists
    const List<int> c1{1, 2};
    const List<int> c2{1, 2};
    ASSERT_TRUE(c1 == c2);

    // edge: non-trivial element type
    ASSERT_TRUE((List<std::string>{"a", "b"}) == (List<std::string>{"a", "b"}));
    ASSERT_FALSE((List<std::string>{"a", "b"}) == (List<std::string>{"a", "c"}));
}

TEST(list_test, iterator_addition) {
    List<int> l1{1,2,3,4};

    auto end = l1.begin() + 3;
    ASSERT_EQ(*(l1.end() - 1), *end);

    ASSERT_TRUE(l1.validate());

    // edge: adding / subtracting zero is the identity
    ASSERT_TRUE(l1.begin() + 0 == l1.begin());
    ASSERT_TRUE(l1.end() - 0 == l1.end());

    // edge: begin() + size() == end(), end() - size() == begin()
    ASSERT_TRUE(l1.begin() + 4 == l1.end());
    ASSERT_TRUE(l1.end() - 4 == l1.begin());

    // edge: round trip
    ASSERT_TRUE((l1.begin() + 2) - 2 == l1.begin());

    // edge: arithmetic does not modify the original iterator
    auto b = l1.begin();
    auto moved = b + 2;
    ASSERT_EQ(*b, 1);
    ASSERT_EQ(*moved, 3);

    // edge: const_iterator arithmetic
    ASSERT_EQ(*(l1.cbegin() + 1), 2);
    ASSERT_EQ(*(l1.cend() - 1), 4);

    // edge: pre vs post increment / decrement semantics
    auto it = l1.begin();
    auto old = it++;
    ASSERT_EQ(*old, 1);
    ASSERT_EQ(*it, 2);
    ASSERT_EQ(*(++it), 3);
    auto old2 = it--;
    ASSERT_EQ(*old2, 3);
    ASSERT_EQ(*it, 2);
    ASSERT_EQ(*(--it), 1);

    // edge: decrementing end() reaches the last element
    auto e = l1.end();
    --e;
    ASSERT_EQ(*e, 4);

    // edge: full forward and backward traversal
    std::vector<int> fwd, bwd;
    for (auto i = l1.begin(); i != l1.end(); ++i)
        fwd.push_back(*i);
    for (auto i = l1.end(); i != l1.begin();)
        bwd.push_back(*--i);
    ASSERT_EQ(fwd, (std::vector<int>{1,2,3,4}));
    ASSERT_EQ(bwd, (std::vector<int>{4,3,2,1}));

    // edge: writing through a non-const iterator
    *(l1.begin() + 1) = 20;
    ASSERT_EQ(l1, (List<int>{1,20,3,4}));

    // edge: operator-> on a non-trivial type
    List<std::string> s{"abc", "de"};
    ASSERT_EQ(s.begin()->size(), 3);
    ASSERT_EQ((s.begin() + 1)->size(), 2);

    // edge: single-element list
    List<int> one{5};
    ASSERT_TRUE(one.begin() + 1 == one.end());
    ASSERT_TRUE(one.end() - 1 == one.begin());
}

TEST(list_test, pop_front) {
    List<int> l{1,2,3,4};

    l.pop_front();
    ASSERT_EQ(l, (List<int>{2,3,4}));
    ASSERT_EQ(l.size(), 3);
    ASSERT_EQ(l.front(), 2);
    ASSERT_EQ(l.back(), 4);
    ASSERT_TRUE(l.validate());

    // edge: pop until empty
    l.pop_front();
    l.pop_front();
    ASSERT_EQ(l, (List<int>{4}));
    ASSERT_EQ(l.front(), l.back());
    l.pop_front();
    ASSERT_TRUE(l.empty());
    ASSERT_EQ(l.size(), 0);
    ASSERT_TRUE(l.begin() == l.end());
    ASSERT_TRUE(l.validate());

    // edge: reusable after being emptied
    l.push_front(1);
    l.push_back(2);
    ASSERT_EQ(l, (List<int>{1, 2}));
    ASSERT_TRUE(l.validate());

    // edge: alternating pop_front / pop_back meet in the middle
    List<int> l2{1, 2, 3, 4, 5};
    l2.pop_front();
    l2.pop_back();
    l2.pop_front();
    l2.pop_back();
    ASSERT_EQ(l2, (List<int>{3}));
    ASSERT_TRUE(l2.validate());

    // edge: non-trivial type
    List<std::string> s{std::string(64, 'a'), "b"};
    s.pop_front();
    ASSERT_EQ(s.front(), "b");
}


TEST(list_test, clear) {
    List<int> l{1,2,3,4};
    l.clear();
    ASSERT_TRUE(l.empty());
    ASSERT_EQ(l, (List<int>{}));
    ASSERT_EQ(l.size(), 0);
    ASSERT_TRUE(l.begin() == l.end());
    ASSERT_TRUE(l.validate());

    // edge: clearing an already-empty list
    l.clear();
    ASSERT_TRUE(l.empty());
    ASSERT_EQ(l.size(), 0);
    ASSERT_TRUE(l.validate());

    // edge: clearing a never-filled list
    List<int> fresh;
    fresh.clear();
    ASSERT_TRUE(fresh.empty());

    // edge: list is fully usable after clear
    l.push_back(2);
    l.push_front(1);
    l.insert(l.cend(), 3);
    ASSERT_EQ(l, (List<int>{1, 2, 3}));
    ASSERT_EQ(l.size(), 3);
    ASSERT_TRUE(l.validate());

    // edge: single-element list
    List<int> one{1};
    one.clear();
    ASSERT_TRUE(one.empty());

    // edge: non-trivial type (ASan catches leaks)
    List<std::string> s{std::string(64, 'a'), std::string(64, 'b')};
    s.clear();
    ASSERT_TRUE(s.empty());
    s.push_back("again");
    ASSERT_EQ(s.front(), "again");
}

TEST(list_test, reverse) {
    List<int> l{1,2,3,4};
    l.reverse();

    ASSERT_EQ(l, (List<int>{4,3,2,1}));
    ASSERT_EQ(l.front(), 4);
    ASSERT_EQ(l.back(), 1);
    ASSERT_EQ(l.size(), 4);
    ASSERT_TRUE(l.validate());

    List<int> l2{1};
    l2.reverse();
    ASSERT_EQ(l2, (List<int>{1}));
    ASSERT_TRUE(l2.validate());

    // edge: empty list
    List<int> empty;
    empty.reverse();
    ASSERT_TRUE(empty.empty());
    ASSERT_TRUE(empty.validate());

    // edge: two elements
    List<int> two{1, 2};
    two.reverse();
    ASSERT_EQ(two, (List<int>{2, 1}));
    ASSERT_TRUE(two.validate());

    // edge: odd number of elements (middle stays put)
    List<int> odd{1, 2, 3, 4, 5};
    odd.reverse();
    ASSERT_EQ(odd, (List<int>{5, 4, 3, 2, 1}));
    ASSERT_TRUE(odd.validate());

    // edge: reversing twice restores the original
    odd.reverse();
    ASSERT_EQ(odd, (List<int>{1, 2, 3, 4, 5}));
    ASSERT_TRUE(odd.validate());

    // edge: backward traversal is consistent after reverse
    std::vector<int> bwd;
    for (auto i = l.end(); i != l.begin();)
        bwd.push_back(*--i);
    ASSERT_EQ(bwd, (std::vector<int>{1, 2, 3, 4}));

    // edge: list is still modifiable after reverse
    l.push_back(0);
    l.push_front(5);
    ASSERT_EQ(l, (List<int>{5, 4, 3, 2, 1, 0}));
    ASSERT_TRUE(l.validate());

    // edge: iterators keep pointing at the same element after reverse
    List<int> l3{1, 2, 3};
    auto it = l3.begin(); // 1
    l3.reverse();
    ASSERT_EQ(*it, 1);
    ASSERT_TRUE(std::next(it) == l3.end());
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
    ASSERT_TRUE(list1.validate());
    ASSERT_TRUE(list2.validate());

    // edge: the source list is reusable after being spliced from
    list2.push_back(1);
    ASSERT_EQ(list2, (List<int>{1}));
    ASSERT_TRUE(list2.validate());

    // edge: splicing an empty list is a no-op
    List<int> a{1, 2};
    List<int> emptySrc;
    a.splice(std::next(a.cbegin()), emptySrc);
    ASSERT_EQ(a, (List<int>{1, 2}));
    ASSERT_EQ(a.size(), 2);
    ASSERT_TRUE(emptySrc.empty());

    // edge: splicing a list into itself is a no-op
    a.splice(a.cbegin(), a);
    ASSERT_EQ(a, (List<int>{1, 2}));
    ASSERT_TRUE(a.validate());

    // edge: splice at begin()
    List<int> b{3, 4};
    List<int> b2{1, 2};
    b.splice(b.cbegin(), b2);
    ASSERT_EQ(b, (List<int>{1, 2, 3, 4}));
    ASSERT_EQ(b.front(), 1);
    ASSERT_TRUE(b.validate());

    // edge: splice at end()
    List<int> c{1, 2};
    List<int> c2{3, 4};
    c.splice(c.cend(), c2);
    ASSERT_EQ(c, (List<int>{1, 2, 3, 4}));
    ASSERT_EQ(c.back(), 4);
    ASSERT_TRUE(c.validate());
    ASSERT_TRUE(c2.validate());

    // edge: splice into an empty list
    List<int> d;
    List<int> d2{1, 2, 3};
    d.splice(d.cend(), d2);
    ASSERT_EQ(d, (List<int>{1, 2, 3}));
    ASSERT_EQ(d.size(), 3);
    ASSERT_TRUE(d2.empty());
    ASSERT_TRUE(d.validate());
    ASSERT_TRUE(d2.validate());

    // edge: splicing a single-element list
    List<int> e{1, 3};
    List<int> e2{2};
    e.splice(std::next(e.cbegin()), e2);
    ASSERT_EQ(e, (List<int>{1, 2, 3}));
    ASSERT_TRUE(e.validate());

    // edge: iterators into the source stay valid and now belong to the destination
    List<int> f{1, 4};
    List<int> f2{2, 3};
    auto srcIt = f2.begin(); // 2
    f.splice(std::next(f.cbegin()), f2);
    ASSERT_EQ(*srcIt, 2);
    ASSERT_EQ(*std::next(srcIt), 3);
    ASSERT_EQ(*std::next(srcIt, 2), 4);
    ASSERT_TRUE(std::next(srcIt, 3) == f.end());
}

TEST(list_test, test_validate) {
    List<int> list1{1,2,3,4,5};

    ASSERT_TRUE(list1.validate());

    // edge: empty list is valid
    List<int> empty;
    ASSERT_TRUE(empty.validate());

    // edge: single-element list is valid
    List<int> one{1};
    ASSERT_TRUE(one.validate());

    // edge: list stays valid across a sequence of mixed operations
    list1.push_front(0);
    list1.push_back(6);
    list1.insert(std::next(list1.cbegin(), 3), 99);
    list1.erase(list1.cbegin());
    list1.pop_back();
    list1.reverse();
    ASSERT_TRUE(list1.validate());
    ASSERT_EQ(list1, (List<int>{5, 4, 3, 99, 2, 1}));

    // edge: valid after being emptied every possible way
    List<int> x{1, 2, 3};
    x.erase(x.cbegin(), x.cend());
    ASSERT_TRUE(x.validate());
    x.push_back(1);
    x.pop_front();
    ASSERT_TRUE(x.validate());
    x.push_back(1);
    x.pop_back();
    ASSERT_TRUE(x.validate());
    x.push_back(1);
    x.clear();
    ASSERT_TRUE(x.validate());
}
// list1: 1 2 10 20 30 40 50 3 4 5
// list2:
