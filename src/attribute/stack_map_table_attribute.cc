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
#include "stack_map_table_attribute.hh"
#include "util.hh"

static void skip_verification_type_info(std::ifstream& file, uint16_t length)
{
    for (uint16_t current = 0; current < length; current++)
    {
        READ_U1_FIELD(tag, "Failed to parse tag for current verification type info entry of "
            "StackMapTable attribute.");
        if (tag == 7 || tag == 8)
        {
            READ_U2_FIELD(tmp, "Failed to parse cp info/offset for current verification type info "
                "entry of StackMapTable attribute.");
        }
    }
}

std::unique_ptr<attribute_info> parse_stack_map_table_attribute(std::ifstream& file,
    const constant_pool& cp)
{
    READ_U2_FIELD(number_of_entries, "Failed to parse number of stack map table entries of "
        "StackMapTable attribute.");
    for (uint16_t curr_entry_idx = 0; curr_entry_idx < number_of_entries; curr_entry_idx++)
    {
        READ_U1_FIELD(frame_type, "Failed to parse frame type for current entry of StackMapTable "
            "attribute.");
        if (frame_type >= 64 && frame_type <= 127)
        {
            skip_verification_type_info(file, 1);
        }
        else if (frame_type == 247)
        {
            READ_U2_FIELD(offset_delta, "Failed to parse offset delta for current entry of "
                "StackMapTable attribute.");
            skip_verification_type_info(file, 1);
        }
        else if (frame_type >= 248 && frame_type <= 251)
        {
            READ_U2_FIELD(offset_delta, "Failed to parse offset delta for current entry of "
                "StackMapTable attribute.");
        }
        else if (frame_type >= 252 && frame_type <= 254)
        {
            READ_U2_FIELD(offset_delta, "Failed to parse offset delta for current entry of "
                "StackMapTable attribute.");
            skip_verification_type_info(file, frame_type - 251);
        }
        else if (frame_type == 255)
        {
            READ_U2_FIELD(offset_delta, "Failed to parse offset delta for current entry of "
                "StackMapTable attribute.");
            READ_U2_FIELD(number_of_locals, "Failed to parse number of locals for current entry of "
                "StackMapTable attribute.");
            skip_verification_type_info(file, number_of_locals);
            READ_U2_FIELD(number_of_stack_items, "Failed to parse number of stack items for "
                "current entry of StackMapTable attribute.");
            skip_verification_type_info(file, number_of_stack_items);
        }
    }

    return std::make_unique<stack_map_table_attribute>();
}
