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
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "cxxopts.hh"

#include "find_api_calls.hh"
#include "invalid_class_format_exception.hh"
#include "java_class.hh"

void denormalize_api_names(std::vector<std::string>& apis)
{
    std::for_each(apis.begin(), apis.end(), [](std::string& api)
    {
        std::replace(api.begin(), api.end(), '.', '/');
        api.erase(std::remove(api.begin(), api.end(), ' '), api.end());
    });
}

void do_dump_cp(const java_class& clazz)
{
    std::vector<std::string> id_col, entry_col, pointed_col;
    const auto& constant_pool = clazz.get_class_constant_pool();
    for (const auto& [entry_id, entry_data] : constant_pool)
    {
        std::stringstream id_str, entry_str, pointed_str;
        id_str << "#" << entry_id << " = ";

        const constant_pool_type entry_type = entry_data.type;
        // Convert each constant pool entry to a printable string.
        std::visit([&](const auto& arg)
        {
            using cp_entry_type = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<cp_entry_type, cp_utf8_entry>)
            {
                id_str << "Utf8";
                entry_str << arg.value;
            }
            else if constexpr (std::is_same_v<cp_entry_type, cp_integer_entry>)
            {
                id_str << "Integer";
                entry_str << arg.value;
            }
            else if constexpr (std::is_same_v<cp_entry_type, cp_float_entry>)
            {
                id_str << "Float";
                entry_str << arg.value;
            }
            else if constexpr (std::is_same_v<cp_entry_type, cp_long_entry>)
            {
                id_str << "Long";
                entry_str << arg.value;
            }
            else if constexpr (std::is_same_v<cp_entry_type, cp_double_entry>)
            {
                id_str << "Double";
                entry_str << arg.value;
            }
            else if constexpr (std::is_same_v<cp_entry_type, cp_methodhandle_info_entry>)
                id_str << "MethodHandle";
            else if constexpr (std::is_same_v<cp_entry_type, cp_index_entry>)
            {
                if (entry_type == constant_pool_type::Class)
                    id_str << "Class";
                else if (entry_type == constant_pool_type::String)
                    id_str << "String";
                else if (entry_type == constant_pool_type::MethodType)
                    id_str << "MethodType";

                entry_str << "#" << arg.cp_index;

                // Class, String, and MethodType all point to a Utf8 index.
                auto utf8_entry = constant_pool.get_entry_as<cp_utf8_entry>(arg.cp_index).value();
                pointed_str << "-> " << utf8_entry.value;
            }
            else if constexpr (std::is_same_v<cp_entry_type, cp_double_index_entry>)
            {
                if (entry_type == constant_pool_type::FieldRef)
                    id_str << "FieldRef";
                else if (entry_type == constant_pool_type::MethodRef)
                    id_str << "MethodRef";
                else if (entry_type == constant_pool_type::InterfaceMethodRef)
                    id_str << "InterfaceMethodRef";
                else if (entry_type == constant_pool_type::NameAndType)
                    id_str << "NameAndType";
                else if (entry_type == constant_pool_type::InvokeDynamic)
                    id_str << "InvokeDynamic";

                entry_str << "#" << arg.cp_index << ":#" << arg.cp_index2;
                pointed_str << "-> ";

                // These are used to allow the NameAndType entry logic to be reused for
                // entries that have it as their second constant pool operand.
                constant_pool_type current_entry_type = entry_type;
                cp_double_index_entry current_entry = arg;

                // FieldRef, MethodRef, and InterfaceMethodRef point to a Class entry and
                // NameAndType entry.
                if (current_entry_type == constant_pool_type::FieldRef ||
                    current_entry_type == constant_pool_type::MethodRef ||
                    current_entry_type == constant_pool_type::InterfaceMethodRef)
                {
                    // Get Class entry.
                    auto class_entry =
                        constant_pool.get_entry_as<cp_class_info_entry>(current_entry.cp_index).value();
                    // From the Class entry, get the Utf8 entry.
                    auto utf8_entry =
                        constant_pool.get_entry_as<cp_utf8_entry>(class_entry.cp_index).value();
                    pointed_str << utf8_entry.value << ".";

                    // To avoid repeating code, set the current entry to the NameAndType
                    // entry.
                    current_entry_type = constant_pool_type::NameAndType;
                    current_entry =
                        constant_pool.get_entry_as<cp_double_index_entry>(current_entry.cp_index2).value();
                }

                // NameAndType points to two Utf8 entries.
                if (current_entry_type == constant_pool_type::NameAndType)
                {
                    // Pull out the the Utf8 entries from NameAndType (respectively).
                    auto name_utf8_entry =
                        constant_pool.get_entry_as<cp_utf8_entry>(current_entry.cp_index).value();
                    auto type_utf8_entry =
                        constant_pool.get_entry_as<cp_utf8_entry>(current_entry.cp_index2).value();

                    // `<init>` has quotes around it according to Java's tool; follow
                    // their format.
                    std::string normalize_init = name_utf8_entry.value == "<init>"
                        ? "\"<init>\""
                        : name_utf8_entry.value;
                    pointed_str << normalize_init << ":" << type_utf8_entry.value;
                }
            }
        }, entry_data.entry);

        id_col.push_back(id_str.str());
        entry_col.push_back(entry_str.str());
        pointed_col.push_back(pointed_str.str());
    }

    // If any of the column entries are different sizes, it's likely the class parsing failed silently.
    if (id_col.size() != entry_col.size() || entry_col.size() != pointed_col.size())
    {
        throw invalid_class_format{"Failed to dump constant pool."};
    }

    // If there is nothing to output, just return.
    if (!id_col.size() || !entry_col.size() || !pointed_col.size())
    {
        return;
    }

    const auto str_compare = [](const std::string& lhs, const std::string& rhs) {
        return lhs.size() < rhs.size();
    };
    auto id_col_longest = std::max_element(id_col.cbegin(), id_col.cend(), str_compare)->size();
    auto entry_col_longest = std::max_element(entry_col.cbegin(), entry_col.cend(), str_compare)->size();
    auto pointed_col_longest = std::max_element(pointed_col.cbegin(), pointed_col.cend(), str_compare)->size();

    for (size_t i = 0; i < id_col.size(); i++)
    {
        std::cout
            << std::left
            << std::setw(id_col_longest + 1) << id_col[i]
            << std::setw(entry_col_longest + 1) << entry_col[i]
            << std::setw(pointed_col_longest) << pointed_col[i]
            << std::endl;
    }
}

void do_dump_class(const java_class& clazz)
{
    const std::vector<method_info>& methods = clazz.get_class_methods();
    for (const auto& method : methods)
    {
        std::string method_name = method.get_name();
        std::cout << method_name << ":" << std::endl;

        const entry_attributes& attributes = method.get_method_attributes();
        for (const auto& attribute : attributes)
        {
            std::cout << static_cast<int>(attribute->get_type()) << std::endl;
        }

        std::cout << std::endl;
    }
}

void do_scan(const java_class& clazz, const std::string& class_name, const std::vector<std::string>& api_names)
{
    std::cout << "Found the following API calls in " << class_name << ":" << std::endl;
    const auto calls = find_api_calls(clazz, api_names);
    for (const auto& call : calls)
    {
        std::cout << '\t' << call.api_str << " in method " << call.method << " on line " <<
            call.line_number << std::endl;
    }
}

void do_command(cxxopts::ParseResult args, bool& error) {
    const auto class_name = args["input"].as<std::string>();
    try
    {
        const auto clazz = java_class::parse_class_file(class_name);
        if (args.count("dump-cp"))
        {
            do_dump_cp(clazz);
        }
        else if (args.count("dump-class"))
        {
            do_dump_class(clazz);
        }
        else if (args.count("scan"))
        {
            // The constant pool stores APIs as, for example, "java/io/PrintStream" instead of the
            // common convention of "java.io.PrintStream".
            auto api_names = args["scan"].as<std::vector<std::string>>();
            denormalize_api_names(api_names);
            do_scan(clazz, class_name, api_names);
        }
        else
        {
            error = true;
        }
    }
    catch (const std::ios::failure& io_failure)
    {
        std::cerr << io_failure.what() << std::endl;
    }
    catch (const invalid_class_format& icf)
    {
        std::cerr << icf.what() << std::endl;
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("bytecode-scanner", "Java bytecode utility");
    options
        .allow_unrecognised_options()
        .add_options()
            ("input", "Input class file", cxxopts::value<std::string>())
            ("c,dump-cp", "Dump constant pool")
            ("d,dump-class", "Dump given class")
            ("s,scan", "Scan for a CSV list of APIs", cxxopts::value<std::vector<std::string>>());
    options.parse_positional({ "input" });

    bool error = false;
    try
    {
        cxxopts::ParseResult args = options.parse(argc, argv);
        if (!args.count("input"))
        {
            error = true;
        }

        if (!error)
        {
            do_command(std::move(args), error);
        }
    }
    catch (const cxxopts::OptionException& e)
    {
        error = true;
    }

    if (error)
    {
        std::cerr << options.help() << std::endl;
        return 1;
    }

    return 0;
}
