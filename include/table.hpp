#ifndef TABLE_HPP
#define TABLE_HPP

#include <vector>
#include <cstddef>
#include <climits>
#include <utility>
#include <algorithm>

namespace ds {

/// @class Table
/// @brief An array type that provides a virtual grid topology.

template<typename value_type>
class table {
public:
    /// @brief Construct an empty table.

    table():
            rows(default_size),
            cols(default_size) {
        table_indices.assign(rows * cols, none);
    }

    /// @brief Construct an empty table with the given dimensions.
    /// @param number_of_rows The desired number of rows.
    /// @param number_of_cols The desired number of columns.

    table(std::size_t number_of_rows, std::size_t number_of_cols):
            rows(number_of_rows),
            cols(number_of_cols) {
        table_indices.assign(rows * cols, none);
    }

public:
    /// @brief Get the total number of cells of the table.
    /// @return The size of the table, rows * cols.

    [[nodiscard]]
    inline int size() const noexcept {
        return rows * cols;
    }

    /// @brief Get the dimensions of the table.
    /// @return The dimensions of the table, (rows, cols).

    [[nodiscard]]
    inline std::pair<std::size_t, std::size_t> dimensions() const noexcept {
        return {rows, cols};
    }

    /// @brief Get the number of data items stored in the table.
    /// @return The number of data items stored in the table.

    [[nodiscard]]
    inline auto count() const noexcept {
        return cells.size();
    }

    /// @brief Indicate whether the table contains any data items or not.
    /// @return Whether the table is empty or not.

    [[nodiscard]]
    inline bool empty() const noexcept {
        return cells.empty();
    }

    /// @brief Read the table cell at the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.

    inline value_type& at(int row, int column) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return cells.at(i);
    }

    /// @brief Read the table cell at the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.

    inline value_type const& at(int row, int column) const noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return cells.at(i);
    }

    /// @brief Try to read the table cell at the given position but return the given fallback element if it's empty.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.
    /// @param otherwise The fallback element to return if the requested cell is empty.

    inline value_type& at_else(int row, int column, value_type& otherwise) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return (i == none) ? otherwise : cells.at(i);
    }

    /// @brief Try to read the table cell at the given position but return the given fallback element if it's empty.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.
    /// @param otherwise The fallback element to return if the requested cell is empty.

    inline value_type const& at_else(int row, int column, value_type const& otherwise) const noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return (i == none) ? otherwise : cells.at(i);
    }

    /// @brief Get a pointer to the contents of the table cell at the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.
    /// @return A pointer to the underlying contents of the table cell or `nullptr` if the cell is empty.

    inline value_type* get(int row, int column) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return (i == none) ? nullptr : &(cells.at(i));
    }

    /// @brief Get a pointer to the contents of the table cell at the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.
    /// @return A pointer to the underlying contents of the table cell or `nullptr` if the cell is empty.

    inline value_type const* get(int row, int column) const {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return (i == none) ? nullptr : &(cells.at(i));
    }

    /// @brief Indicate whether the table cell at the given position contains an element or not.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.

    [[nodiscard]]
    inline bool contains(int row, int column) const noexcept {
        auto const t = get_table_index(row, column);
        return table_indices.at(t) != none;
    }

public:
    /// @brief Set the value of the table cell with the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.
    /// @param element The element that should be copied into the table cell.

    inline value_type& set(int row, int column, value_type element) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const n = static_cast<int>(cells.size());

        if (contains(t)) {
            return modify(table_indices.at(t), element);
        }

        cells.push_back(element);
        cells_indices.push_back(t);
        table_indices.at(t) = n;

        return cells.back();
    }

    /// @brief Set the value of the table cell at the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.
    /// @param arguments The parameters that should be used to construct the new data item.

    template<typename ...Arguments>
    inline value_type& emplace(int row, int column, Arguments&& ... arguments) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const n = static_cast<int>(cells.size());

        if (contains(t)) {
            return modify(table_indices.at(t), value_type(std::forward<Arguments>(arguments)...));
        }

        cells.emplace_back(arguments...);
        cells_indices.push_back(t);
        table_indices.at(t) = n;

        return cells.back();
    }

    /// @brief Erase the contents of the table cell at the given position.
    /// @param row The row int of the desired table cell.
    /// @param column The column int of the desired table cell.

    inline void erase(int const row, int const column) noexcept(false) {
        auto const table_int = get_table_index(row, column);
        auto const cells_int = table_indices.at(table_int);
        auto const updatable = swap_and_erase(cells_int);
        table_indices.at(table_int) = none;

        if (updatable != none) {
            table_indices.at(updatable) = cells_int;
        }
    }

    /// @brief Reset the state of the table at the current size.

    inline void reset() {
        *this = table<value_type>(rows, cols);
    }

public:
    /// @brief Set the size of the table.
    /// @param number_of_rows The desired number of rows.
    /// @param number_of_columns The desired number of columns.
    /// @note Shrinking the size of the table may delete some number of its elements.
    /// @note The amount of time required is linear in the number of data items,
    /// but the number of items that are copied is bounded by the given dimensions.

    void set_size(std::size_t const number_of_rows, std::size_t const number_of_columns) {
        ds::table<value_type> new_table(number_of_rows, number_of_columns);

        for (auto i = 0; i < cells_indices.size(); ++i) {
            auto const& index = cells_indices.at(i);
            auto const& datum = cells.at(i);

            auto const row = index / cols;
            auto const col = index % cols;

            if (row < number_of_rows && col < number_of_columns) {
                new_table.set(row, col, datum);
            }
        }

        *this = std::move(new_table);
    }

public:
    /// @brief Get a pointer to the underlying array of data items.
    /// @note This pointer is read-only.

    inline auto data() const {
        return cells.data();
    }

    /// @brief Get a read-only 'begin' iterator for the underlying array of data items.
    /// @note This iterator is read-only.

    inline auto begin() const {
        return cells.begin();
    }

    /// @brief Get a read-only 'end' iterator for the underlying array of data items.
    /// @note This iterator is read-only.

    inline auto end() const {
        return cells.end();
    }

private:
    /// @brief Compute a 1d table index from the given 2d position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.

    [[nodiscard]]
    inline int get_table_index(int const row, int const column) const noexcept {
        return row * cols + column;
    }

    /// @brief Indicate whether the element at the given 1d table index is `none` or not.
    /// @param table_index The 1d index of the desired element.
    /// @return Whether the specified element is a cell (`true`) or `none` (`false`).

    [[nodiscard]]
    inline bool contains(int const table_index) const noexcept(false) {
        return table_indices.at(table_index) != none;
    }

    /// @brief Set the value of the `cells` element with the given index.
    /// @param cells_index The index of the element to be modified.
    /// @param new_value The new value to be set.

    inline value_type& modify(int const cells_index, value_type new_value) noexcept(false) {
        cells.at(cells_index) = std::move(new_value);
        return cells[cells_index];
    }

    /// @brief Swap the element at the given index with the last element in the
    /// given container and erase it.
    /// @param container The container from which the selected element should be erased.
    /// @param container_index The index of the element to be erased from the container.

    template<typename K>
    inline void swap_and_erase(std::vector<K>& container, int const container_index) {
        auto a = container.data() + container_index;
        auto b = container.end() - 1;
        std::iter_swap(a, b);
        container.pop_back();
    }

    /// @brief Swap the cell elements corresponding to the given index with the last elements
    /// of their respective containers and erase them.
    /// @param cells_index The index of the element to be erased from the `cells`
    /// and `cells_indices` containers.
    /// @return The table index of the element that was swapped into the position with the given index.

    inline int swap_and_erase(int const cells_index) {
        swap_and_erase(cells_indices, cells_index);
        swap_and_erase(cells, cells_index);

        return cells_index == cells.size() ? none : cells_indices.at(cells_index);
    }

public:
    /// @brief The default number of rows and columns.

    std::size_t static constexpr default_size {4};

private:
    /// @brief The data items that comprise the table's contents.

    std::vector<value_type> cells;

    /// @brief The table indices that correspond to the data items in the `cells` array.
    /// @example `cells_indices.at(i)` maps the element `cells.at(i)` to a position in the `table_indices` array.

    std::vector<int> cells_indices;

    /// @brief A lookup table of indices to table cells, which may contain `none` elements.

    std::vector<int> table_indices;

private:
    std::size_t rows;
    std::size_t cols;

    /// @brief The value reserved to represent the absence of data in the table.

    auto inline static constexpr none {-INT_MAX};
};

}

#endif
