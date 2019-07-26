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
#include <vector>

#include "attribute_info.hh"
#include "constant_pool.hh"

enum class field_access_flags : uint16_t {
  Public = 0x1, Private = 0x2, Protected = 0x4, Static = 0x8, Final = 0x10,
  Volatile = 0x40, Transient = 0x80, Synthetic = 0x1000, Enum = 0x4000
};

class field_info {
  std::reference_wrapper<const constant_pool> cp;
  field_access_flags access_flags;
  constant_pool_entry_id name_index;
  constant_pool_entry_id descriptor_index;
  entry_attributes field_attributes;
  field_info(const constant_pool& cp, field_access_flags access_flags,
    constant_pool_entry_id name_index, constant_pool_entry_id descriptor_index,
    entry_attributes field_attributes);
public:
  static field_info parse_field(std::ifstream& file, const constant_pool& cp);
};

std::vector<field_info> parse_fields(std::ifstream& file, const constant_pool& cp);
