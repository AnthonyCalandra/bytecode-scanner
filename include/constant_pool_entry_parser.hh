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

#include "constant_pool.hh"

template <typename T>
struct cp_value_entry {
  T value;
};
using cp_utf8_entry = cp_value_entry<std::string>;
using cp_integer_entry = cp_value_entry<int32_t>;
using cp_float_entry = cp_value_entry<float>;
using cp_long_entry = cp_value_entry<int64_t>;
using cp_double_entry = cp_value_entry<double>;

struct cp_index_entry {
  constant_pool_entry_id cp_index;
};
using cp_class_info_entry = cp_index_entry;
using cp_string_info_entry = cp_index_entry;
using cp_methodtype_info_entry = cp_index_entry;

struct cp_double_index_entry {
  constant_pool_entry_id cp_index;
  constant_pool_entry_id cp_index2;
};
using cp_fieldref_info_entry = cp_double_index_entry;
using cp_methodref_info_entry = cp_double_index_entry;
using cp_interfacemethodref_info_entry = cp_double_index_entry;
using cp_name_and_type_index_entry = cp_double_index_entry;
using cp_invokedynamic_info_entry = cp_double_index_entry;

struct cp_methodhandle_info_entry {
  uint8_t reference_kind;
  constant_pool_entry_id reference_index;
};

cp_utf8_entry parse_cp_utf8_entry(std::ifstream& file);
cp_integer_entry parse_cp_integer_entry(std::ifstream& file);
cp_float_entry parse_cp_float_entry(std::ifstream& file);
cp_long_entry parse_cp_long_entry(std::ifstream& file);
cp_double_entry parse_cp_double_entry(std::ifstream& file);
cp_index_entry parse_cp_index_entry(std::ifstream& file);
cp_double_index_entry parse_cp_double_index_entry(std::ifstream& file);
cp_methodhandle_info_entry parse_cp_methodhandle_info_entry(std::ifstream& file);
