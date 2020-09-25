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

enum class method_access_flags : uint16_t
{
    Public = 0x1, Private = 0x2, Protected = 0x4, Static = 0x8, Final = 0x10,
    Synchronized = 0x20, Bridge = 0x40, Varargs = 0x80, Native = 0x100,
    Abstract = 0x400, Strict = 0x800, Synthetic = 0x1000
};

class method_info
{
    std::reference_wrapper<const constant_pool> cp;
    [[maybe_unused]] method_access_flags access_flags;
    constant_pool_entry_id name_index;
    [[maybe_unused]] constant_pool_entry_id descriptor_index;
    entry_attributes method_attributes;

    explicit method_info(const constant_pool& cp, method_access_flags access_flags,
        constant_pool_entry_id name_index, constant_pool_entry_id descriptor_index,
        entry_attributes method_attributes);

public:
    constant_pool_entry_id get_name_index() const
    {
        return name_index;
    }

    std::string get_name() const
    {
        const constant_pool& cp_ref = cp;
        const auto& method_name_utf8_ref = std::get<cp_utf8_entry>(
            cp_ref.get_entry(name_index)->entry);
        return method_name_utf8_ref.value;
    }

    const entry_attributes& get_method_attributes() const
    {
        return method_attributes;
    }

    static method_info parse_method_info(std::ifstream& file, const constant_pool& cp);
};

std::vector<method_info> parse_methods(std::ifstream& file, const constant_pool& cp);
