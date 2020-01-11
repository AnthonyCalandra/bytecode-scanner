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

#include <fstream>
#include <functional>
#include <optional>
#include <unordered_map>
#include <utility>
#include <variant>

#include "constant_pool.hh"
#include "constant_pool_entry_parser.hh"
#include "invalid_class_format_exception.hh"
#include "util.hh"

using cp_parser_fn = std::function<constant_pool_entry(std::ifstream&)>;
using cp_parser_table = std::unordered_map<constant_pool_type, cp_parser_fn>;

cp_parser_table build_cp_entry_parser_table() {
  cp_parser_table table{};
  table[constant_pool_type::Utf8] = parse_cp_utf8_entry;
  table[constant_pool_type::Integer] = parse_cp_integer_entry;
  table[constant_pool_type::Float] = parse_cp_float_entry;
  table[constant_pool_type::Long] = parse_cp_long_entry;
  table[constant_pool_type::Double] = parse_cp_double_entry;
  table[constant_pool_type::Class] = parse_cp_index_entry;
  table[constant_pool_type::String] = parse_cp_index_entry;
  table[constant_pool_type::FieldRef] = parse_cp_double_index_entry;
  table[constant_pool_type::MethodRef] = parse_cp_double_index_entry;
  table[constant_pool_type::InterfaceMethodRef] = parse_cp_double_index_entry;
  table[constant_pool_type::NameAndType] = parse_cp_double_index_entry;
  table[constant_pool_type::MethodType] = parse_cp_index_entry;
  table[constant_pool_type::MethodHandle] = parse_cp_methodhandle_info_entry;
  table[constant_pool_type::InvokeDynamic] = parse_cp_double_index_entry;
  return table;
}

static const cp_parser_table cp_entry_parser_table = build_cp_entry_parser_table();

constant_pool constant_pool::parse_constant_pool(std::ifstream& file) {
  READ_U2_FIELD(constant_pool_count, "Failed to parse constant pool count.");
  if (constant_pool_count > 0) {
    constant_pool_count--;
  }

  constant_pool_entries entries{};
  // Read in each constant pool entry. Since we don't know what entry we are looking at until
  // we see the tag, we visit (by tag) and construct an entry.
  for (constant_pool_entry_id curr_idx = 0; curr_idx < constant_pool_count;) {
    // Read the tag.
    READ_U1_FIELD(current_entry_tag_bytes, "Failed to parse entry tag length.");
    auto current_entry_tag = constant_pool_type{current_entry_tag_bytes};

    // Now that we know what this next chunk is, construct it.
    auto parser_it = cp_entry_parser_table.find(current_entry_tag);
    if (parser_it == cp_entry_parser_table.cend()) {
      throw invalid_class_format{"Unknown constant pool tag."};
    }

    // Lookup the parser function for the given tag and emplace the object it parses into our
    // constant pool.
    const auto& parse_entry_fn = parser_it->second;
    constant_pool_entry parse_entry = parse_entry_fn(file);
    entries.emplace(curr_idx + 1,
      constant_pool_entry_info{current_entry_tag, std::move(parse_entry)});
    // Doubles and Longs increment the constant pool index by 2.
    curr_idx += (current_entry_tag == constant_pool_type::Double || current_entry_tag ==
      constant_pool_type::Long) ? 2 : 1;
  }

  return constant_pool{std::move(entries)};
}

constant_pool::constant_pool(constant_pool_entries entries) : entries{std::move(entries)} {}

std::optional<constant_pool_entry_info> constant_pool::get_entry(constant_pool_entry_id index)
    const {
  auto entries_it = entries.find(index);
  if (entries_it == entries.cend()) {
    return std::nullopt;
  } else {
    return entries_it->second;
  }
}
