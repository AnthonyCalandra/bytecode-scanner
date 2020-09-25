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

#include <memory>
#include <vector>

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "exceptions_attribute.hh"
#include "util.hh"

std::unique_ptr<attribute_info> parse_exceptions_attribute(std::ifstream& file,
    const constant_pool& cp)
{
    READ_U2_FIELD(number_of_exceptions, "Failed to parse line number of exceptions of method.");
    std::vector<constant_pool_entry_id> exception_index_table;
    for (uint16_t curr_et_idx = 0; curr_et_idx < number_of_exceptions; curr_et_idx++)
    {
        READ_U2_FIELD(exception_table_entry, "Failed to parse exception table entry of method.");
        exception_index_table.emplace_back(exception_table_entry);
    }

    return std::make_unique<exceptions_attribute>(std::move(exception_index_table));
}
