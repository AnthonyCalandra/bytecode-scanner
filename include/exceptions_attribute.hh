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

class exceptions_attribute: public attribute_info
{
    std::vector<constant_pool_entry_id> exception_index_table;

public:
    explicit exceptions_attribute(std::vector<constant_pool_entry_id> exception_index_table) :
        exception_index_table{std::move(exception_index_table)}
    {}

    virtual attribute_info_type get_type() const
    {
        return attribute_info_type::exceptions;
    }
};

std::unique_ptr<attribute_info> parse_exceptions_attribute(std::ifstream& file,
    const constant_pool& cp);
