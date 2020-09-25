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

#include <cmath>
#include <fstream>
#include <limits>

#include "constant_pool.hh"
#include "constant_pool_entry_parser.hh"
#include "invalid_class_format_exception.hh"
#include "util.hh"

cp_utf8_entry parse_cp_utf8_entry(std::ifstream& file)
{
    READ_U2_FIELD(utf8_length, "Failed to parse constant pool utf8 string entry.");
    std::string utf8_string(utf8_length, '\0');
    if (!file.read(&utf8_string[0], utf8_length))
    {
        throw invalid_class_format{"Failed to parse constant pool utf8 string entry."};
    }

    return cp_utf8_entry{std::move(utf8_string)};
}

cp_integer_entry parse_cp_integer_entry(std::ifstream& file)
{
    READ_U4_FIELD(number, "Failed to parse constant pool integer entry.");
    return cp_integer_entry{static_cast<int32_t>(number)};
}

cp_float_entry parse_cp_float_entry(std::ifstream& file)
{
    READ_U4_FIELD(bits, "Failed to parse constant pool float entry.");
    float number = 0;
    if (bits == 0x7F800000)
    {
        number = std::numeric_limits<float>::infinity();
    }
    else if (bits == 0xFF800000)
    {
        number = -std::numeric_limits<float>::infinity();
    }
    else if ((bits >= 0x7F800001 && bits <= 0x7FFFFFFF) ||
             (bits >= 0xFF800001 && bits <= 0xFFFFFFFF))
    {
        number = std::numeric_limits<float>::quiet_NaN();
    }
    else
    {
        int32_t s = ((bits >> 31) == 0) ? 1 : -1;
        int32_t e = ((bits >> 23) & 0xFF);
        int32_t m = (e == 0) ? (bits & 0x7FFFFF) << 1 : (bits & 0x7FFFFF) | 0x800000;
        number = s * m * std::powf(2.f, e - 150);
    }

    return cp_float_entry{number};
}

cp_long_entry parse_cp_long_entry(std::ifstream& file)
{
    constexpr size_t LONG_ENTRY_LENGTH = 8;
    uint64_t number = 0;
    if (!file.read(reinterpret_cast<char*>(&number), LONG_ENTRY_LENGTH))
    {
        throw invalid_class_format{"Failed to parse constant pool long entry."};
    }

    uint32_t high_bytes = (number >> 32) & 0xFFFFFFFF;
    uint32_t low_bytes = number & 0xFFFFFFFF;
    number = to_little_endian_long((static_cast<uint64_t>(high_bytes) << 32) + low_bytes);
    return cp_long_entry{static_cast<int64_t>(number)};
}

cp_double_entry parse_cp_double_entry(std::ifstream& file)
{
    constexpr size_t DOUBLE_ENTRY_LENGTH = 8;
    uint64_t bits = 0;
    if (!file.read(reinterpret_cast<char*>(&bits), DOUBLE_ENTRY_LENGTH))
    {
        throw invalid_class_format{"Failed to parse constant pool double entry."};
    }

    uint32_t high_bytes = (bits >> 32) & 0xFFFFFFFF;
    uint32_t low_bytes = bits & 0xFFFFFFFF;
    bits = to_little_endian_long((static_cast<uint64_t>(high_bytes) << 32) + low_bytes);
    double number = 0;
    if (bits == 0x7FF0000000000000L)
    {
        number = std::numeric_limits<float>::infinity();
    }
    else if (bits == 0xFFF0000000000000L)
    {
        number = -std::numeric_limits<float>::infinity();
    }
    else if ((bits >= 0x7FF0000000000001L && bits <= 0x7FFFFFFFFFFFFFFFL) ||
             (bits >= 0xFFF0000000000001L && bits <= 0xFFFFFFFFFFFFFFFFL))
    {
        number = std::numeric_limits<float>::quiet_NaN();
    }
    else
    {
        int32_t s = ((bits >> 63) == 0) ? 1 : -1;
        int32_t e = static_cast<int32_t>((bits >> 52) & 0x7FFL);
        int64_t m = (e == 0)
            ? (bits & 0xFFFFFFFFFFFFFL) << 1
            : (bits & 0xFFFFFFFFFFFFFL) | 0x10000000000000L;
        number = s * m * std::pow(2., e - 1075);
    }

    return cp_double_entry{number};
}

cp_index_entry parse_cp_index_entry(std::ifstream& file)
{
    READ_U2_FIELD(cp_index, "Failed to parse constant pool index entry.");
    return cp_index_entry{cp_index};
}

cp_double_index_entry parse_cp_double_index_entry(std::ifstream& file)
{
    READ_U2_FIELD(cp_index, "Failed to parse constant pool double index entry.");
    READ_U2_FIELD(cp_index2, "Failed to parse constant pool double index entry.");
    return cp_double_index_entry{cp_index, cp_index2};
}

cp_methodhandle_info_entry parse_cp_methodhandle_info_entry(std::ifstream& file)
{
    READ_U1_FIELD(reference_kind, "Failed to parse constant pool method handle entry.");
    READ_U2_FIELD(reference_info, "Failed to parse constant pool method handle entry.");
    return cp_methodhandle_info_entry{reference_kind, reference_info};
}
