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

#include <fstream>
#include <map>
#include <optional>
#include <utility>
#include <variant>

using constant_pool_entry_id = uint16_t;

#include "constant_pool_entry_parser.hh"

enum class constant_pool_type : uint8_t {
  Utf8 = 1, Integer = 3, Float, Long, Double, Class, String, FieldRef, MethodRef,
  InterfaceMethodRef, NameAndType, MethodHandle = 15, MethodType = 16, InvokeDynamic = 18
};

using constant_pool_entry = std::variant<cp_utf8_entry, cp_integer_entry, cp_float_entry,
  cp_long_entry, cp_double_entry, cp_index_entry, cp_double_index_entry,
  cp_methodhandle_info_entry>;

struct constant_pool_entry_info {
  constant_pool_type type;
  constant_pool_entry entry;
};

using constant_pool_entries = std::map<constant_pool_entry_id, constant_pool_entry_info>;

class constant_pool {
  const constant_pool_entries entries;
public:
  explicit constant_pool(constant_pool_entries entries);
  constant_pool() = default;

  size_t size() const {
    return entries.size();
  }

  auto begin() const {
    return entries.begin();
  }

  auto end() const {
    return entries.end();
  }

  auto cbegin() const {
    return entries.cbegin();
  }

  auto cend() const {
    return entries.cend();
  }

  std::optional<constant_pool_entry_info> get_entry(constant_pool_entry_id index) const;
  static constant_pool parse_constant_pool(std::ifstream& file);
};
