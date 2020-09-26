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

#include <memory>
#include <vector>

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "util.hh"

struct inner_class_entry
{
    constant_pool_entry_id inner_class_info_index;
    constant_pool_entry_id outer_class_info_index;
    constant_pool_entry_id inner_name_index;
    uint16_t inner_class_access_flags;

    explicit inner_class_entry(constant_pool_entry_id inner_class_info_index,
        constant_pool_entry_id outer_class_info_index, constant_pool_entry_id inner_name_index,
        uint16_t inner_class_access_flags) :
            inner_class_info_index{inner_class_info_index},
            outer_class_info_index{outer_class_info_index},
            inner_name_index{inner_name_index},
            inner_class_access_flags{inner_class_access_flags}
    {}
};

class inner_classes_attribute: public attribute_info
{
    std::vector<inner_class_entry> inner_classes;

public:
    explicit inner_classes_attribute(std::vector<inner_class_entry> inner_classes) :
        inner_classes{std::move(inner_classes)}
    {}

    virtual attribute_info_type get_type() const
    {
        return attribute_info_type::InnerClasses;
    }
};

std::unique_ptr<attribute_info> parse_inner_classes_attribute(std::ifstream& file,
    const constant_pool& cp);
