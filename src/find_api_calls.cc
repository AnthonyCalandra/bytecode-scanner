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

#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

#include "attribute_info.hh"
#include "code_attribute.hh"
#include "find_api_calls.hh"
#include "invalid_class_format_exception.hh"
#include "java_class.hh"
#include "line_number_table_attribute.hh"

std::optional<api_call_info> get_api_call_info(const constant_pool& cp, uint16_t pc, uint8_t high, uint8_t low,
    const std::vector<std::string>& apis)
{
    constant_pool_entry_id cp_method_ref = (high << 8) + low;
    const auto& method_ref = std::get<cp_methodref_info_entry>(cp.get_entry(cp_method_ref)->entry);
    // Extract the class name from the method reference.
    const auto& class_ref = std::get<cp_class_info_entry>(cp.get_entry(method_ref.cp_index)->entry);
    const auto& class_name_utf8_ref = std::get<cp_utf8_entry>(cp.get_entry(class_ref.cp_index)->entry);
    // If the class name matches ones we're looking for, get the method name too, and
    // return the API handle.
    auto apis_iter = std::find(apis.cbegin(), apis.cend(), class_name_utf8_ref.value);
    if (apis_iter != apis.cend())
    {
        // Extract the method name from the constant pool.
        const auto& name_and_type_ref = std::get<cp_name_and_type_index_entry>(
            cp.get_entry(method_ref.cp_index2)->entry);
        const auto& method_name_utf8_ref = std::get<cp_utf8_entry>(cp.get_entry(name_and_type_ref.cp_index)->entry);
        return std::make_optional<api_call_info>({
            // Store the pc instead of line number for now.
            pc, class_name_utf8_ref.value + "." + method_name_utf8_ref.value, ""
        });
    }

    return std::nullopt;
}

uint16_t get_line_number(const code_attribute& code, uint16_t pc)
{
    for (const auto& code_attr: code.get_code_attributes())
    {
        if (code_attr->get_type() == attribute_info_type::LineNumberTable)
        {
            const auto& lnt_attr = dynamic_cast<const line_number_table_attribute&>(*code_attr);
            return *lnt_attr.find_line_number_from_pc(pc);
        }
    }

    // Should never reach here.
    throw invalid_class_format{"Missing `LineNumberTable` attribute in `Code`."};
    return 0;
}

std::vector<api_call_info> find_api_calls(const java_class& clazz, const std::vector<std::string>& apis)
{
    std::vector<api_call_info> calls;
    const auto& cp = clazz.get_class_constant_pool();
    for (const method_info& method: clazz.get_class_methods())
    {
        for (const auto& attr: method.get_method_attributes())
        {
            // The `Code` attribute contains raw bytecode and line number information.
            if (attr->get_type() == attribute_info_type::Code)
            {
                const auto& code_attr = dynamic_cast<const code_attribute&>(*attr);
                const auto instruction_cb = [&](uint16_t pc, uint8_t high, uint8_t low)
                {
                    if (auto call = get_api_call_info(cp, pc, high, low, apis); call)
                    {
                        call->line_number = get_line_number(code_attr, call->line_number);
                        call->method = method.get_name();
                        calls.emplace_back(*call);
                    }
                };

                // Call the callback when an `invokevirtual` instruction is found in bytecode.
                code_attr.find_instruction<bytecode_tag::INVOKEVIRTUAL>(instruction_cb);
                // Call the callback when an `invokespecial` instruction is found in bytecode.
                code_attr.find_instruction<bytecode_tag::INVOKESPECIAL>(instruction_cb);
            }
        }
    }

    return calls;
}
