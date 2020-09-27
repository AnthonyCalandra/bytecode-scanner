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
#include <optional>

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "field_info.hh"
#include "method_info.hh"

enum class classfile_access_flag : uint16_t
{
    Public = 0x1, Final = 0x10, Super = 0x20, Interface = 0x200, Abstract = 0x400,
    Synthetic = 0x1000, Annotation = 0x2000, Enum = 0x4000
};

class java_class
{
    constant_pool cp;
    classfile_access_flag access_flags;
    constant_pool_entry_id this_index;
    constant_pool_entry_id super_index;
    std::vector<constant_pool_entry_id> interfaces_ids;
    std::vector<field_info> fields;
    std::vector<method_info> methods;
    entry_attributes attributes;

public:
    explicit java_class(constant_pool cp, classfile_access_flag access_flags,
        constant_pool_entry_id this_index, constant_pool_entry_id super_index,
        std::vector<constant_pool_entry_id> interfaces_ids, std::vector<field_info> fields,
        std::vector<method_info> methods, entry_attributes attributes);

    explicit java_class(constant_pool cp, classfile_access_flag access_flags,
        constant_pool_entry_id this_index, constant_pool_entry_id super_index,
        std::vector<constant_pool_entry_id> interfaces_ids);

    const constant_pool& get_class_constant_pool() const
    {
        return cp;
    }

    classfile_access_flag get_class_access_flags() const
    {
        return access_flags;
    }

    constant_pool_entry_id get_class_this_index() const
    {
        return this_index;
    }

    constant_pool_entry_id get_class_super_index() const
    {
        return super_index;
    }

    const std::vector<constant_pool_entry_id>& get_class_interfaces_ids() const
    {
        return interfaces_ids;
    }

    const std::vector<field_info>& get_class_fields() const
    {
        return fields;
    }

    const std::vector<method_info>& get_class_methods() const
    {
        return methods;
    }

    const entry_attributes& get_class_attributes() const
    {
        return attributes;
    }

    static java_class parse_class_file(const std::string& path);
};
