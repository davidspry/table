//! @file test.cpp
//! @date 15/9/21
//! @brief Tests for the `ds::table<T>` type.
//! @author David Spry

#include <random>
#include <functional>
#include <gtest/gtest.h>

#include "../include/table.hpp"

namespace ds {
struct noncopyable {
    noncopyable() = default;
    noncopyable(noncopyable&&) noexcept = default;
    noncopyable(noncopyable const&) = delete;
    noncopyable& operator=(noncopyable const&) = delete;
    noncopyable& operator=(noncopyable&&) noexcept = default;
};
}

TEST(Table, DefaultConstructor) {
    ds::table<int> table;

    EXPECT_EQ(table.count(), 0);
    EXPECT_EQ(table.size(), table.default_size * table.default_size);
    EXPECT_EQ(table.dimensions(), std::pair(table.default_size, table.default_size));
}

TEST(Table, NonDefaultConstructor) {
    ds::dim_t const rows = 5;
    ds::dim_t const cols = 10;
    ds::table<int> table(rows, cols);

    EXPECT_EQ(table.count(), 0);
    EXPECT_EQ(table.size(), rows * cols);
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));
}

TEST(Table, SetItemCopyable) {
    ds::table<int> table;

    int& a = table.set(0, 0, 5);
    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at(0, 0), 5);

    int& b = table.set(0, 0, 2);
    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at(0, 0), 2);

    int& c = table.set(0, 1, b);
    EXPECT_EQ(table.count(), 2);
    EXPECT_EQ(table.at(0, 1), 2);
}

TEST(Table, EmplaceItemCopyable) {
    ds::table<int> table;

    int& a = table.emplace(0, 0, 5);
    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at(0, 0), 5);

    int& b = table.emplace(0, 0, 2);
    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at(0, 0), 2);

    int& c = table.emplace(0, 1, -5);
    EXPECT_EQ(table.count(), 2);
    EXPECT_EQ(table.at(0, 1), -5);
}

TEST(Table, EmplaceItemNoncopyable) {
    ds::table<ds::noncopyable> table;

    table.emplace(0, 0);
    EXPECT_EQ(table.count(), 1);

    table.emplace(0, 0);
    EXPECT_EQ(table.count(), 1);

    table.emplace(0, 1);
    EXPECT_EQ(table.count(), 2);
}

TEST(Table, At) {
    ds::table<int> table;

    EXPECT_ANY_THROW(table.at(0, 0));

    int a = table.emplace(0, 0, 6);
    int b = table.at(0, 0);
    int c = table.set(2, 2, b);

    EXPECT_EQ(a, b);
    EXPECT_EQ(b, c);
    EXPECT_ANY_THROW(table.at(-5, -5));
}

TEST(Table, AtElse) {
    ds::table<int> table;

    auto fallback = 100;
    EXPECT_ANY_THROW(table.at(0, 0));
    EXPECT_EQ(table.at_else(0, 0, 5), 5);

    auto& mutable_ref = fallback;
    EXPECT_ANY_THROW(table.at(0, 1));
    EXPECT_EQ(table.at_else(0, 1, mutable_ref), mutable_ref);

    auto const& immutable_ref = fallback;
    EXPECT_ANY_THROW(table.at(0, 2));
    EXPECT_EQ(table.at_else(0, 2, immutable_ref), immutable_ref);
}

TEST(Table, Get) {
    ds::table<int> table;
    ds::table<int> const immutable;

    EXPECT_EQ(table.get(0, 0), nullptr);
    ASSERT_TRUE(table.empty());

    int& value = table.emplace(2, 2, 5);
    int* point = table.get(2, 2);

    EXPECT_EQ(&value, point);
    EXPECT_NO_THROW(immutable.get(0, 0));
}

TEST(Table, Contains) {
    ds::table<int> table(10, 10);

    std::random_device device;
    std::mt19937 mersenne(device());
    std::uniform_int_distribution<int> randInt(0, 9);

    for (auto i = 0; i < 50; ++i) {
        int const row = randInt(mersenne);
        int const col = randInt(mersenne);

        if (table.contains(row, col)) {
            EXPECT_NE(table.at_else(row, col, 5), 5);
            continue;
        }

        table.emplace(row, col, 1);
        EXPECT_TRUE(table.contains(row, col));
    }
}

TEST(Table, Erase) {
    ds::table<int> table(2, 2);

    table.emplace(0, 0, 0);
    table.emplace(0, 1, 1);
    table.emplace(1, 0, 2);
    table.emplace(1, 1, 3);

    table.erase(0, 0);
    EXPECT_EQ(table.count(), 3);
    EXPECT_EQ(table.at_else(0, 0, 5), 5);

    table.erase(1, 0);
    EXPECT_EQ(table.count(), 2);
    EXPECT_EQ(table.at_else(1, 0, 5), 5);

    table.erase(1, 1);
    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at_else(1, 1, 5), 5);

    table.erase(0, 1);
    EXPECT_EQ(table.count(), 0);
    EXPECT_EQ(table.at_else(0, 1, 5), 5);

    EXPECT_TRUE(table.empty());
}

TEST(Table, Reset) {
    ds::dim_t const rows = 10;
    ds::dim_t const cols = 5;
    ds::table<int> table(rows, cols);

    for (auto row = 0; row < rows; ++row) {
        for (auto col = 0; col < cols; ++col) {
            table.emplace(row, col, 1 + row * 6 + col);
        }
    }

    ASSERT_EQ(table.count(), rows * cols);

    table.reset();
    EXPECT_EQ(table.count(), 0);
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));

    table.set_size(rows * 2, cols * 2);
    table.emplace(0, 0, 5);
    ASSERT_EQ(table.count(), 1);

    table.reset();
    EXPECT_EQ(table.count(), 0);
    EXPECT_EQ(table.dimensions(), std::pair(rows * 2, cols * 2));
}

TEST(Table, SetSize) {
    ds::dim_t rows = 10;
    ds::dim_t cols = 10;
    ds::table<int> table(rows, rows);

    auto const for_each = [&](auto const& task) {
        for (auto row = 0; row < rows; ++row) {
            for (auto col = 0; col < cols; ++col) {
                task(row, col);
            }
        }
    };

    for_each([&](int const row, int const col) {
        auto const value = table.emplace(row, col, row * cols + col);
        EXPECT_EQ(table.at(row, col), value);
    });

    ASSERT_EQ(table.count(), rows * cols);
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));

    rows = rows + 1;
    cols = cols + 1;
    table.set_size(rows, cols);

    EXPECT_EQ(table.count(), (rows - 1) * (cols - 1));
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));

    for_each([&](int const row, int const col) {
        if (row == rows - 1 || col == cols - 1)
            EXPECT_EQ(table.contains(row, col), false);
        else {
            EXPECT_EQ(table.contains(row, col), true);
            EXPECT_EQ(table.at(row, col), row * (cols - 1) + col);
        }
    });

    rows = (rows - 1) >> 1;
    cols = (cols - 1) >> 1;
    table.set_size(rows, cols);

    EXPECT_EQ(table.count(), rows * cols);
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));

    for_each([&](int const row, int const col) {
        EXPECT_EQ(table.at(row, col), row * (cols << 1) + col);
    });

    rows = rows << 3;
    cols = cols << 3;
    table.set_size(rows, cols);

    EXPECT_EQ(table.count(), (rows * cols) >> 6);
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));

    for_each([&](int const row, int const col) {
        int const r = rows >> 3;
        int const c = cols >> 3;
        if (row < r && col < c)
            EXPECT_EQ(table.at(row, col), row * (c << 1) + col);
        else
            EXPECT_EQ(table.contains(row, col), false);
    });

    rows = 6;
    cols = 6;
    table.set_size(rows, cols);

    for_each([&](int const row, int const col) {
        auto const value = table.emplace(row, col, row * cols + col);
        EXPECT_EQ(table.at(row, col), value);
    });

    ASSERT_EQ(table.count(), rows * cols);
    EXPECT_EQ(table.dimensions(), std::pair(rows, cols));

    table.set_size(1, 1);

    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at(0, 0), 0);
    EXPECT_EQ(table.dimensions().first, 1);
    EXPECT_EQ(table.dimensions().second, 1);

    EXPECT_NO_THROW(table.set_size(1, 1));
    EXPECT_NO_THROW(table.set_size(50, 50));
    EXPECT_NO_THROW(table.set_size(20, 20));
    EXPECT_NO_THROW(table.set_size(50, 50));
    EXPECT_NO_THROW(table.set_size(10, 10));
    EXPECT_NO_THROW(table.set_size(0, 0));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

