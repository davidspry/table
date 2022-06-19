//! @file test.cpp
//! @date 15/9/21
//! @brief Tests for the `ds::table<T>` type.
//! @author David Spry

#include <random>
#include <vector>
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
    std::size_t const rows = 5;
    std::size_t const cols = 10;
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

TEST(Table, EmplaceItem) {
    using element_type = std::vector<int>;
    ds::table<element_type> table;

    /* Emplace new element */
    table.emplace(0, 0, 1, 1);
    EXPECT_EQ(table.count(), 1);
    EXPECT_EQ(table.at(0, 0), element_type(1, 1));

    /* Emplace new element */
    table.emplace(0, 1, 2, 1);
    EXPECT_EQ(table.count(), 2);
    EXPECT_EQ(table.at(0, 1), element_type(2, 1));

    /* Replace existing element */
    table.emplace(0, 0, 3, 1);
    EXPECT_EQ(table.count(), 2);
    EXPECT_EQ(table.at(0, 0), element_type(3, 1));
}

TEST(Table, EmplaceNoncopyableItem) {
    ds::table<ds::noncopyable> table;

    /* Emplace new element */
    table.emplace(0, 0);
    EXPECT_EQ(table.count(), 1);

    /* Emplace new element */
    table.emplace(0, 1);
    EXPECT_EQ(table.count(), 2);

    /* Replace existing element */
    table.emplace(0, 0);
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
    ds::table<int> table {3, 3};

    auto value = 10;
    auto fallback = 100;
    table.set(2, 2, value);

    EXPECT_NO_THROW(table.at(2, 2));
    EXPECT_ANY_THROW(table.at(0, 0));
    EXPECT_EQ(table.at_else(2, 2, fallback), value);
    EXPECT_EQ(table.at_else(0, 0, fallback), fallback);

    auto& mutable_ref = fallback;
    EXPECT_ANY_THROW(table.at(0, 1));
    EXPECT_EQ(table.at_else(0, 1, mutable_ref), mutable_ref);

    auto const& immutable_ref = fallback;
    EXPECT_ANY_THROW(table.at(0, 2));
    EXPECT_EQ(table.at_else(0, 2, immutable_ref), immutable_ref);
}

TEST(Table, Get) {
    ds::table<int> table;

    ASSERT_TRUE(table.empty());
    EXPECT_EQ(table.get(0, 0), nullptr);

    auto& a = table.emplace(2, 2, 5);
    auto* b = table.get(2, 2);

    EXPECT_EQ(&a, b);
}

TEST(Table, Contains) {
    auto runs = 96;
    auto size = std::size_t {10};

    ds::table<int> table(size, size);

    std::random_device device;
    std::mt19937 mersenne(device());
    std::uniform_int_distribution<int> randInt(0, size - 1);

    auto value {2};
    auto error {8};

    for (auto _ = 0; _ < runs; ++_) {
        auto const row = randInt(mersenne);
        auto const col = randInt(mersenne);

        if (table.contains(row, col)) {
            EXPECT_NE(table.at_else(row, col, error), error);
            continue;
        }

        table.emplace(row, col, value);
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
    std::size_t const rows = 10;
    std::size_t const cols = 5;
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
    std::size_t rows = 10;
    std::size_t cols = 10;
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

    for_each([&](unsigned int const row, unsigned int const col) {
        auto const r = rows >> 3;
        auto const c = cols >> 3;
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

