//! @file table.hpp
//! @date 15/9/21
//! @brief The ds::table<T> class.
//! @author David Spry

#ifndef TABLE_HPP
#define TABLE_HPP

#include <vector>
#include <utility>
#include <algorithm>

namespace ds {

using index = int;
using dim_t = unsigned int;

/// @class An array type that provides a virtual grid topology.

template <typename T>
class table {
public:
    /// @brief Construct an empty table.

    table() :
            rows(default_size),
            cols(default_size) {
        table_indices.assign(rows * cols, none);
    }

    /// @brief Construct an empty table with the given dimensions.
    /// @param number_of_rows The desired number of rows.
    /// @param number_of_cols The desired number of columns.

    table(dim_t const number_of_rows, dim_t const number_of_cols) :
            rows(number_of_rows),
            cols(number_of_cols) {
        table_indices.assign(rows * cols, none);
    }

public:
    /// @brief Get the total number of cells of the table.
    /// @return The size of the table, rows * cols.

    [[nodiscard]] inline int size() const noexcept {
        return rows * cols;
    }

    /// @brief Get the dimensions of the table.
    /// @return The dimensions of the table, (rows, cols).

    [[nodiscard]] inline std::pair<dim_t, dim_t> dimensions() const noexcept {
        return {rows, cols};
    }

    /// @brief Get the number of data items stored in the table.
    /// @return The number of data items stored in the table.

    [[nodiscard]] inline int count() const noexcept {
        return cells.size();
    }

    /// @brief Indicate whether the table contains any data items or not.
    /// @return Whether the table is empty or not.

    [[nodiscard]] inline int empty() const noexcept {
        return cells.empty();
    }

    /// @brief Read the table cell at the given position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @throw An exception will be thrown if the table cell is empty or
    /// if the given position is out of range.

    inline T& at(index const row, index const column) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return cells.at(i);
    }

    /// @brief Read the table cell at the given position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @throw An exception will be thrown if the table cell is empty or
    /// if the given position is out of range.

    inline T const& at(index const row, index const column) const noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        return cells.at(i);
    }

    /// @brief Try to read the table cell at the given position but return the given fallback element if it's empty.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @param otherwise The fallback element to return if the requested cell is empty.
    /// @throw An exception will be thrown if the given position is out of range.

    inline T& at_else(index const row, index const column, T& otherwise) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        if (i == none) return otherwise;
        return cells.at(i);
    }

    /// @brief Try to read the table cell at the given position but return the given fallback element if it's empty.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @param otherwise The fallback element to return if the requested cell is empty.
    /// @throw An exception will be thrown if the given position is out of range.

    inline T& at_else(index const row, index const column, T&& otherwise) noexcept(false) {
        return at_else(row, column, otherwise);
    }

    /// @brief Try to read the table cell at the given position but return the given fallback element if it's empty.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @param otherwise The fallback element to return if the requested cell is empty.
    /// @throw An exception will be thrown if the given position is out of range.

    inline T const& at_else(index const row, index const column, T const& otherwise) const noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        if (i == none) return otherwise;
        return cells.at(i);
    }

    /// @brief Get a pointer to the contents of the table cell at the given position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @return A pointer to the underlying contents of the table cell or `nullptr` if the cell is empty.

    inline T* get(index const row, index const column) {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        if (i == none) return nullptr;
        return & (cells.at(i));
    }

    /// @brief Get a pointer to the contents of the table cell at the given position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @return A pointer to the underlying contents of the table cell or `nullptr` if the cell is empty.

    inline T const* get(index const row, index const column) const {
        auto const t = get_table_index(row, column);
        auto const i = table_indices.at(t);
        if (i == none) return nullptr;
        return & (cells.at(i));
    }

    /// @brief Indicate whether the table cell at the given position contains an element or not.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.

    [[nodiscard]] inline bool contains(index const row, index const column) const noexcept {
        auto const t = get_table_index(row, column);
        return table_indices.at(t) != none;
    }

public:
    /// @brief Set the value of the table cell with the given position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @param element The element that should be copied into the table cell.

    inline T& set(index const row, index const column, T const& element) noexcept(false) {
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
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.
    /// @param parameters The parameters that should be used to construct the new data item.

    template <typename ...K>
    inline T& emplace(index const row, index const column, K&& ... parameters) noexcept(false) {
        auto const t = get_table_index(row, column);
        auto const n = static_cast<int>(cells.size());

        if (contains(t)) {
            return modify(table_indices.at(t), T(std::forward<K>(parameters)...));
        }

        cells.emplace_back(parameters...);
        cells_indices.push_back(t);
        table_indices.at(t) = n;

        return cells.back();
    }

    /// @brief Erase the contents of the table cell at the given position.
    /// @param row The row index of the desired table cell.
    /// @param column The column index of the desired table cell.

    inline void erase(index const row, index const column) noexcept(false) {
        auto const table_index = get_table_index(row, column);
        auto const cells_index = table_indices.at(table_index);
        auto const updatable = swap_and_erase(cells_index);
        table_indices.at(table_index) = none;

        if (updatable != none) {
            table_indices.at(updatable) = cells_index;
        }
    }

public:
    /// @brief Set the size of the table.
    /// @param number_of_rows The desired number of rows.
    /// @param number_of_columns The desired number of columns.
    /// @note Shrinking the size of the table may delete some number of its elements.
    /// @note The amount of time required is linear in the number of data items,
    /// but the number of items that are copied is bounded by the given dimensions.

    void set_size(dim_t const number_of_rows, dim_t const number_of_columns) {
        ds::table<T> new_table(number_of_rows, number_of_columns);

        for (auto i = 0; i < cells_indices.size(); ++i) {
            auto const& index = cells_indices.at(i);
            auto const& datum = cells.at(i);

            auto const row = index / cols;
            auto const col = index % cols;

            if (row < number_of_rows && col < number_of_columns) {
                new_table.set(row, col, datum);
            }
        }

        * this = std::move(new_table);
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

    [[nodiscard]] inline index get_table_index(index const row, index const column) const noexcept {
        return row * cols + column;
    }

    /// @brief Indicate whether the element at the given 1d table index is `none` or not.
    /// @param table_index The 1d index of the desired element.
    /// @return Whether the specified element is a cell (`true`) or `none` (`false`).

    [[nodiscard]] inline bool contains(index const table_index) const noexcept {
        return table_indices.at(table_index) != none;
    }

    /// @brief Set the value of the `cells` element with the given index.
    /// @param cells_index The index of the element to be modified.
    /// @param new_value The new value to be set.

    inline T& modify(index const cells_index, T const& new_value) noexcept(false) {
        cells.at(cells_index) = new_value;
        return cells.at(cells_index);
    }

    /// @brief Set the value of the `cells` element with the given index.
    /// @param cells_index The index of the element to be modified.
    /// @param new_value The new value to be set.

    inline T& modify(index const cells_index, T&& new_value) noexcept(false) {
        cells.at(cells_index) = std::move(new_value);
        return cells.at(cells_index);
    }

    /// @brief Swap the element at the given index with the last element in the
    /// given container and erase it.
    /// @param container The container from which the selected element should be erased.
    /// @param container_index The index of the element to be erased from the container.

    template <typename K>
    inline void swap_and_erase(std::vector<K>& container, index const container_index) {
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

    inline index swap_and_erase(index const cells_index) {
        swap_and_erase(cells_indices, cells_index);
        swap_and_erase(cells, cells_index);

        return cells_index == cells.size() ? none : cells_indices.at(cells_index);
    }

public:
    /// @brief The default number of rows and columns.

    dim_t constexpr static default_size = 4;

private:
    /// @brief The data items that comprise the table's contents.

    std::vector<T> cells;

    /// @brief The table indices that correspond to the data items in the `cells` array.
    /// @example `cells_indices.at(i)` maps the element `cells.at(i)` to a position in the `table_indices` array.

    std::vector<index> cells_indices;

    /// @brief A lookup table of indices to table cells, which may contain `none` elements.

    std::vector<index> table_indices;

private:
    dim_t rows;
    dim_t cols;

    /// @brief The value reserved to represent the absence of data in the table.

    index constexpr inline static none {-INT_MAX};
};

}

#endif
