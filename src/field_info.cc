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
#include <vector>

#include "attribute_info.hh"
#include "field_info.hh"
#include "util.hh"

std::vector<field_info> parse_fields(std::ifstream& file, const constant_pool& cp)
{
    std::vector<field_info> fields;
    READ_U2_FIELD(fields_count, "Failed to parse fields count of class file.");
    for (uint16_t curr_field_idx = 0; curr_field_idx < fields_count; curr_field_idx++)
    {
        fields.emplace_back(field_info::parse_field(file, cp));
    }

    return fields;
}

field_info field_info::parse_field(std::ifstream& file, const constant_pool& cp)
{
    READ_U2_FIELD(access_flag_bytes, "Failed to parse access flags of field.");

    auto access_flags = field_access_flags{access_flag_bytes};
    READ_U2_FIELD(name_index, "Failed to parse name index of field.");
    READ_U2_FIELD(descriptor_index, "Failed to parse descriptor index of field.");
    return field_info{cp, access_flags, name_index, descriptor_index, parse_attributes(file, cp)};
}

field_info::field_info(const constant_pool& cp, field_access_flags access_flags,
    constant_pool_entry_id name_index, constant_pool_entry_id descriptor_index,
    entry_attributes field_attributes) :
        cp{cp},
        access_flags{access_flags},
        name_index{name_index},
        descriptor_index{descriptor_index},
        field_attributes{std::move(field_attributes)}
{}
