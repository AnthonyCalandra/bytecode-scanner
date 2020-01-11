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
#include <string>
#include <type_traits>
#include <vector>

#include "cxxopts.hh"

#include "find_api_calls.hh"
#include "invalid_class_format_exception.hh"
#include "java_class_file.hh"

std::vector<std::string> denormalize_api_names(std::vector<std::string> apis) {
  std::for_each(apis.begin(), apis.end(), [](std::string& api) {
    std::replace(api.begin(), api.end(), '.', '/');
    api.erase(std::remove(api.begin(), api.end(), ' '), api.end());
  });
  return apis;
}

void do_command(cxxopts::ParseResult args) {
  const auto class_name = args["input"].as<std::string>();
  try {
    const auto clazz = java_class_file::parse_class_file(class_name);

    if (args.count("dump-cp")) {
      const auto& constant_pool = clazz.get_class_constant_pool();
      for (const auto& [entry_id, entry_data] : constant_pool) {
        const constant_pool_type entry_type = entry_data.type;
        std::cout << '#' << entry_id << " = ";
        std::visit([&constant_pool, &entry_type](const auto& arg) {
          using cp_entry_type = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<cp_entry_type, cp_utf8_entry>)
            std::cout << "Utf8\t\t" << arg.value;
          else if constexpr (std::is_same_v<cp_entry_type, cp_integer_entry>)
            std::cout << "Integer\t\t" << arg.value;
          else if constexpr (std::is_same_v<cp_entry_type, cp_float_entry>)
            std::cout << "Float\t\t" << arg.value;
          else if constexpr (std::is_same_v<cp_entry_type, cp_long_entry>)
            std::cout << "Long\t\t" << arg.value;
          else if constexpr (std::is_same_v<cp_entry_type, cp_double_entry>)
            std::cout << "Double\t\t" << arg.value;
          else if constexpr (std::is_same_v<cp_entry_type, cp_methodhandle_info_entry>)
            std::cout << "MethodHandle";
          else if constexpr (std::is_same_v<cp_entry_type, cp_index_entry>) {
            if (entry_type == constant_pool_type::Class)
              std::cout << "Class";
            else if (entry_type == constant_pool_type::String)
              std::cout << "String";
            else if (entry_type == constant_pool_type::MethodType)
              std::cout << "MethodType";

            std::cout << "\t\t#" << arg.cp_index;
            // Class, String, and MethodType all point to a Utf8 index.
            const constant_pool_entry_info cp_entry_info =
              constant_pool.get_entry(arg.cp_index).value();
            const auto& utf8_entry = std::get<cp_utf8_entry>(cp_entry_info.entry);
            std::cout << "\t\t-> " << utf8_entry.value;
          } else if constexpr (std::is_same_v<cp_entry_type, cp_double_index_entry>) {
            if (entry_type == constant_pool_type::FieldRef)
              std::cout << "FieldRef";
            else if (entry_type == constant_pool_type::MethodRef)
              std::cout << "MethodRef";
            else if (entry_type == constant_pool_type::InterfaceMethodRef)
              std::cout << "InterfaceMethodRef";
            else if (entry_type == constant_pool_type::NameAndType)
              std::cout << "NameAndType";
            else if (entry_type == constant_pool_type::InvokeDynamic)
              std::cout << "InvokeDynamic";

            std::cout << "\t\t#" << arg.cp_index << ":#" << arg.cp_index2 << "\t\t -> ";
            // These are used to allow the NameAndType entry logic to be reused for entries
            // that have it as their second constant pool operand.
            constant_pool_type current_entry_type = entry_type;
            cp_double_index_entry current_entry = arg;
            // FieldRef, MethodRef, and InterfaceMethodRef point to a Class entry and
            // NameAndType entry.
            if (current_entry_type == constant_pool_type::FieldRef ||
                current_entry_type == constant_pool_type::MethodRef ||
                current_entry_type == constant_pool_type::InterfaceMethodRef) {
              // Get Class entry.
              const constant_pool_entry_info class_entry_info =
                constant_pool.get_entry(current_entry.cp_index).value();
              const auto& class_entry = std::get<cp_class_info_entry>(class_entry_info.entry);
              // From the Class entry, get the Utf8 entry.
              const constant_pool_entry_info utf8_entry_info =
                constant_pool.get_entry(class_entry.cp_index).value();
              const auto& utf8_entry = std::get<cp_utf8_entry>(utf8_entry_info.entry);
              std::cout << utf8_entry.value << ".";

              // To avoid repeating code, set the current entry to the NameAndType entry.
              const constant_pool_entry_info name_and_type_entry_info =
                constant_pool.get_entry(current_entry.cp_index2).value();
              current_entry_type = constant_pool_type::NameAndType;
              current_entry =
                std::get<cp_double_index_entry>(name_and_type_entry_info.entry);
            }

            // NameAndType points to two Utf8 entries.
            if (current_entry_type == constant_pool_type::NameAndType) {
              // Pull out the the Utf8 entries from NameAndType (respectively).
              const constant_pool_entry_info name_entry_info =
                constant_pool.get_entry(current_entry.cp_index).value();
              const constant_pool_entry_info type_entry_info =
                constant_pool.get_entry(current_entry.cp_index2).value();
              const auto& name_utf8_entry = std::get<cp_utf8_entry>(name_entry_info.entry);
              const auto& type_utf8_entry = std::get<cp_utf8_entry>(type_entry_info.entry);
              // `<init>` has quotes around it according to Java's tool; follow their format.
              std::string normalize_init = name_utf8_entry.value == "<init>"
                ? "\"<init>\"" : name_utf8_entry.value;
              std::cout << normalize_init << ":" << type_utf8_entry.value;
            }
          }

          std::cout << std::endl;
        }, entry_data.entry);
      }
    } else if (args.count("scan")) {
      // The constant pool stores APIs as, for example, "java/io/PrintStream" instead of the
      // common convention of "java.io.PrintStream".
      const auto api_names =
        denormalize_api_names(args["scan"].as<std::vector<std::string>>());
      std::cout << "Found the following API calls in " << class_name << ":" << std::endl;
      const auto calls = find_api_calls(clazz, api_names);
      for (const auto& call : calls) {
        std::cout << '\t' << call.api_str << " in method " << call.method << " on line " <<
          call.line_number << std::endl;
      }
    }
  } catch (const std::ios::failure& io_failure) {
    std::cerr << io_failure.what() << std::endl;
  } catch (const invalid_class_format& icf) {
    std::cerr << icf.what() << std::endl;
  }
}

int main(int argc, char** argv) {
  cxxopts::Options options("bytecode-scanner", "Java bytecode utility");
  options
    .allow_unrecognised_options()
    .add_options()
      ("input", "Input class file", cxxopts::value<std::string>())
      ("c,dump-cp", "Dump constant pool")
      ("s,scan", "Scan for a CSV list of APIs", cxxopts::value<std::vector<std::string>>());
  options.parse_positional({ "input" });

  bool error = false;
  try {
    cxxopts::ParseResult args = options.parse(argc, argv);
    if (!args.count("input")) {
      error = true;
    }

    if (!error) {
      do_command(std::move(args));
    }
  } catch (const cxxopts::OptionException& e) {
    error = true;
  }

  if (error) {
    std::cerr << options.help() << std::endl;
    return 1;
  }

  return 0;
}
