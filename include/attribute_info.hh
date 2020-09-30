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
#include <memory>
#include <vector>

#include "constant_pool.hh"

enum class attribute_info_type : uint8_t
{
    constant_value, code, stack_map_table, exceptions, inner_classes, enclosing_method, synthetic,
    signature, source_file, source_debug_extension, line_number_table, local_variable_table,
    local_variable_type_table, deprecated, runtime_visible_annotations, runtime_invisible_annotations,
    runtime_visible_parameter_annotations, runtime_invisible_parameter_annotations, annotation_default,
    bootstrap_methods
};

class attribute_info
{
public:
    virtual attribute_info_type get_type() const = 0;
    virtual ~attribute_info() = default;
};

using entry_attributes = std::vector<std::unique_ptr<attribute_info>>;

entry_attributes parse_attributes(std::ifstream& file, const constant_pool& cp);
void skip_element_value_field(std::ifstream& file);
void skip_annotations(std::ifstream& file);
