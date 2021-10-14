//! @file benchmark.cpp
//! @date 16/9/21
//! @brief Benchmarking for the `ds::table<T>` type.
//! @author David Spry

#include <random>
#include <benchmark/benchmark.h>

#include "../include/table.hpp"

constexpr static int Rows = 10;
constexpr static int Cols = 10;
constexpr static int EmptyRow = Rows - 1;

template <typename T>
class TableFixture : public ds::table<T>,
                     public benchmark::Fixture {
public:
    explicit TableFixture() : ds::table<T>(Rows, Cols) {
    }

    [[nodiscard]] inline std::mt19937 mersenne() const {
        static std::random_device device;
        static std::mt19937 mersenne(device());
        return mersenne;
    }

    [[nodiscard]] inline int rand_int(int const lower_bound, int const upper_bound) const {
        auto engine = mersenne();
        return std::uniform_int_distribution<int>(lower_bound, upper_bound)(engine);
    }

    void SetUp(benchmark::State const& state) override {
        auto const rows = this->dimensions().first;
        auto const cols = this->dimensions().second;
        for (auto row = 0; row < rows - 1; ++row) {
            for (auto col = 0; col < cols; ++col) {
                auto value = row * cols + col;
                this->emplace(row, col, value);
            }
        }
    }
};

BENCHMARK_TEMPLATE_F(TableFixture, BM_At, int)(benchmark::State& state) {
    ds::index const r = rand_int(0, EmptyRow - 1);
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        benchmark::DoNotOptimize(at(r, c));
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_AtElse_Succeed, int)(benchmark::State& state) {
    ds::index const r = rand_int(0, EmptyRow - 1);
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        benchmark::DoNotOptimize(at_else(r, c, 0));
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_AtElse_Fail, int)(benchmark::State& state) {
    ds::index const r = EmptyRow;
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        benchmark::DoNotOptimize(at_else(r, c, 0));
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_Get, int)(benchmark::State& state) {
    ds::index const r = rand_int(0, Rows - 1);
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        benchmark::DoNotOptimize(get(r, c));
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_Set, int)(benchmark::State& state) {
    int const element = 0xf;
    ds::table<int> empty_table(Rows, Cols);
    ds::index const r = rand_int(0, Rows - 1);
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        empty_table.set(r, c, element);
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_Emplace, int)(benchmark::State& state) {
    ds::table<int> empty_table(Rows, Cols);
    ds::index const r = rand_int(0, Rows - 1);
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        empty_table.emplace(r, c, 0xf);
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_EraseAndEmplace, int)(benchmark::State& state) {
    ds::index const r = rand_int(0, EmptyRow - 1);
    ds::index const c = rand_int(0, Cols - 1);
    for (auto _: state) {
        erase(r, c);
        emplace(r, c, 0xf);
    }
}

BENCHMARK_TEMPLATE_F(TableFixture, BM_SetSize, int)(benchmark::State& state) {
    for (auto _: state) {
        ds::dim_t const r = rand_int(Rows, Rows << 6);
        ds::dim_t const c = rand_int(Rows, Rows << 6);
        set_size(r, c);
    }
}

BENCHMARK_MAIN();