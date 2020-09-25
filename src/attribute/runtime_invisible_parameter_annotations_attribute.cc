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

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "runtime_invisible_parameter_annotations_attribute.hh"
#include "util.hh"

std::unique_ptr<attribute_info> parse_runtime_invisible_parameter_annotations_attribute(
    std::ifstream& file, const constant_pool& cp)
{
    READ_U1_FIELD(num_parameters, "Failed to parse number of parameter annotations of "
        "RuntimeInvisibleParameterAnnotations attribute.");
    for (uint8_t curr_param_idx = 0; curr_param_idx < num_parameters; curr_param_idx++)
    {
        skip_annotations(file);
    }

    return std::make_unique<runtime_invisible_parameter_annotations_attribute>();
}
