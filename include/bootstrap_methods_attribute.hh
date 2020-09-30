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

struct bootstrap_method_entry
{
    uint16_t bootstrap_method_ref;
    std::vector<uint16_t> bootstrap_arguments;

    explicit bootstrap_method_entry(uint16_t bootstrap_method_ref,
        std::vector<uint16_t> bootstrap_arguments) :
            bootstrap_method_ref{bootstrap_method_ref},
            bootstrap_arguments{bootstrap_arguments}
    {}
};

class bootstrap_methods_attribute: public attribute_info
{
    std::vector<bootstrap_method_entry> bootstrap_methods;

public:
    explicit bootstrap_methods_attribute(std::vector<bootstrap_method_entry> bootstrap_methods) :
        bootstrap_methods{std::move(bootstrap_methods)}
    {}

    virtual attribute_info_type get_type() const
    {
        return attribute_info_type::bootstrap_methods;
    }
};

std::unique_ptr<attribute_info> parse_bootstrap_methods_attribute(std::ifstream& file,
const constant_pool& cp);
