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

#include <iostream>
#include <optional>
#include <vector>

#include "attribute_info.hh"
#include "code_attribute.hh"
#include "invalid_class_format_exception.hh"
#include "java_class_file.hh"
#include "line_number_table_attribute.hh"

struct api_call_info {
  uint16_t line_number;
  std::string api_str;
};

std::optional<api_call_info> find_api_invoke(const constant_pool& cp, uint16_t pc, uint8_t high,
    uint8_t low, const std::string& api) {
  constant_pool_entry_id cp_method_ref = (high << 8) + low;
  // TODO change this to use `std::get<cp_methodref_info_entry>`.
  const auto& method_ref = *std::get_if<cp_methodref_info_entry>(
    &cp.get_entry(cp_method_ref)->entry);
  // TODO change this to use `std::get<cp_class_info_entry>`.
  const auto& class_ref = *std::get_if<cp_class_info_entry>(
    &cp.get_entry(method_ref.cp_index)->entry);
  // TODO change this to use `std::get<cp_utf8_entry>`.
  const auto& class_name_utf8_ref = *std::get_if<cp_utf8_entry>(
    &cp.get_entry(class_ref.cp_index)->entry);
  if (class_name_utf8_ref.value == api) {
    // TODO change this to use `std::get<cp_name_and_type_index_entry>`.
    const auto& name_and_type_ref = *std::get_if<cp_name_and_type_index_entry>(
      &cp.get_entry(method_ref.cp_index2)->entry);
    // TODO change this to use `std::get<cp_utf8_entry>`.
    const auto& method_name_utf8_ref = *std::get_if<cp_utf8_entry>(
      &cp.get_entry(name_and_type_ref.cp_index)->entry);
    return std::make_optional<api_call_info>({
      // Store the pc instead of line number for now.
      pc, class_name_utf8_ref.value + "." + method_name_utf8_ref.value
    });
  }

  return std::nullopt;
}

uint16_t get_line_number(const code_attribute& code, uint16_t pc) {
  for (const auto& code_attr: code.get_code_attributes()) {
    if (code_attr->get_type() == attribute_info_type::LineNumberTable) {
      const auto& lnt_attr = dynamic_cast<const line_number_table_attribute&>(*code_attr);
      return *lnt_attr.find_line_number_from_pc(pc);
    }
  }

  // Should never reach here.
  throw invalid_class_format{"Missing `LineNumberTable` attribute in `Code`."};
  return 0;
}

void find_api_calls(const java_class_file& clazz, const std::string api) {
  std::vector<api_call_info> calls;
  const auto& cp = clazz.get_class_constant_pool();
  for (const method_info& method: clazz.get_class_methods()) {
    for (const auto& attr: method.get_method_attributes()) {
      // The `Code` attribute contains raw bytecode and line number information.
      if (attr->get_type() == attribute_info_type::Code) {
        const auto& code_attr = dynamic_cast<const code_attribute&>(*attr);
        // Call the callback when an `invokevirtual` instruction is found in bytecode.
        code_attr.find_instruction<bytecode_tag::INVOKEVIRTUAL>(
          [&cp, &api, &calls, &code_attr](uint16_t pc, uint8_t high, uint8_t low) {
            if (auto call = find_api_invoke(cp, pc, high, low, api); call) {
              call->line_number = get_line_number(code_attr, call->line_number);
              calls.emplace_back(*call);
            }
          });
        // Call the callback when an `invokespecial` instruction is found in bytecode.
        code_attr.find_instruction<bytecode_tag::INVOKESPECIAL>(
          [&cp, &api, &calls, &code_attr](uint16_t pc, uint8_t high, uint8_t low) {
            if (auto call = find_api_invoke(cp, pc, high, low, api); call) {
              call->line_number = get_line_number(code_attr, call->line_number);
              calls.emplace_back(*call);
            }
          });
      }
    }
  }

  for (const auto& call : calls) {
    std::cout << '\t' << call.api_str << " on line " << call.line_number << std::endl;
  }
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "USAGE: " << argv[0] << " <classfile-name> <api-name>" << std::endl;
    std::cerr << "EXAMPLE: " << argv[0] << " Foo.class java/io/PrintStream" << std::endl;
    return -1;
  }

  std::string class_name = argv[1];
  std::string api_name = argv[2];
  try {
    const auto clazz = java_class_file::parse_class_file(class_name);
    std::cout << "Found the following " << api_name << " API calls in " << class_name << ":" <<
      std::endl;
    find_api_calls(clazz, api_name);
  } catch (const std::ios::failure& io_failure) {
    std::cerr << io_failure.what() << std::endl;
  } catch (const invalid_class_format& icf) {
    std::cerr << icf.what() << std::endl;
  }

  return 0;
}
