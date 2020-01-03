/**
 * Copyright 2019 Anthony Calandra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "util.hh"

struct line_number_table_entry {
  uint16_t start_pc;
  uint16_t line_number;

  explicit line_number_table_entry(uint16_t start_pc, uint16_t line_number) :
    start_pc{start_pc}, line_number{line_number}
  {}
};

class line_number_table_attribute: public attribute_info {
  std::vector<line_number_table_entry> line_number_table;
public:
  explicit line_number_table_attribute(std::vector<line_number_table_entry> line_number_table) :
    line_number_table{std::move(line_number_table)} {}

  std::optional<uint16_t> find_line_number_from_pc(uint16_t pc) const {
    if (line_number_table.empty()) {
      return std::nullopt;
    }

    auto it = std::adjacent_find(line_number_table.cbegin(), line_number_table.cend(),
      [pc](const auto& curr_entry, const auto& next_entry) {
        return next_entry.start_pc > pc;
      });
    // Past the end, so use the last element.
    if (it == line_number_table.cend()) {
      return line_number_table.back().line_number;
    }

    return it->line_number;
  }

  virtual attribute_info_type get_type() const {
    return attribute_info_type::LineNumberTable;
  }
};

std::unique_ptr<attribute_info> parse_line_number_table_attribute(std::ifstream& file,
    const constant_pool& cp);
