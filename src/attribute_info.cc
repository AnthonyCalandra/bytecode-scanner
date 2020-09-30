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

#include <fstream>
#include <ios>
#include <memory>
#include <stack>
#include <unordered_map>
#include <variant>
#include <vector>

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "invalid_class_format_exception.hh"
#include "util.hh"

#include "annotation_default_attribute.hh"
#include "bootstrap_methods_attribute.hh"
#include "code_attribute.hh"
#include "constant_value_attribute.hh"
#include "deprecated_attribute.hh"
#include "enclosing_method_attribute.hh"
#include "exceptions_attribute.hh"
#include "inner_classes_attribute.hh"
#include "line_number_table_attribute.hh"
#include "local_variable_table_attribute.hh"
#include "local_variable_type_table_attribute.hh"
#include "runtime_invisible_annotations_attribute.hh"
#include "runtime_invisible_parameter_annotations_attribute.hh"
#include "runtime_visible_annotations_attribute.hh"
#include "runtime_visible_parameter_annotations_attribute.hh"
#include "signature_attribute.hh"
#include "source_file_attribute.hh"
#include "stack_map_table_attribute.hh"
#include "synthetic_attribute.hh"

void skip_element_value_field(std::ifstream& file)
{
    READ_U1_FIELD(tag, "Failed to read tag for annotation.");
    unsigned char tag_char = reinterpret_cast<unsigned char>(tag);
    if (tag_char == 'B' || tag_char == 'C' || tag_char == 'D' || tag_char == 'F' ||
        tag_char == 'I' || tag_char == 'J' || tag_char == 'S' || tag_char == 'Z' ||
        tag_char == 's')
    {
        READ_U2_FIELD(const_value_index, "Failed to read const value index for annotation.");
    }
    else if (tag_char == 'e')
    {
        READ_U2_FIELD(type_name_index, "Failed to read type name index for annotation.");
        READ_U2_FIELD(const_name_index, "Failed to read const name index for annotation.");
    }
    else if (tag_char == 'c')
    {
        READ_U2_FIELD(class_info_index, "Failed to read const info index for annotation.");
    }
    else if (tag_char == '@')
    {
        skip_annotations(file);
    }
    else if (tag_char == '[')
    {
        READ_U2_FIELD(num_values, "Failed to read values length for annotation.");
        skip_element_value_field(file);
    }
    else
    {
        throw invalid_class_format{"Unknown annotation tag type."};
    }
}

void skip_annotations(std::ifstream& file)
{
    READ_U2_FIELD(num_annotations, "Failed to read annotations length for field.");
    for (uint16_t curr_annotation_idx = 0; curr_annotation_idx < num_annotations;
        curr_annotation_idx++)
    {
        READ_U2_FIELD(type_index, "Failed to read type index for annotation.");
        READ_U2_FIELD(num_ev_pairs, "Failed to read ev pairs length for annotation.");
        for (uint16_t curr_ev_pair_idx = 0; curr_ev_pair_idx < num_ev_pairs; curr_ev_pair_idx++)
        {
            READ_U2_FIELD(element_name_index, "Failed to read element name index for annotation.");
            skip_element_value_field(file);
        }
    }
}

using attribute_parser_fn = std::function<std::unique_ptr<attribute_info>(std::ifstream&, const
    constant_pool&)>;
using utf8_entry_value_type = decltype(std::declval<cp_utf8_entry>().value);
using attribute_parser_table = std::unordered_map<utf8_entry_value_type, attribute_parser_fn>;

attribute_parser_table build_attribute_parser_table()
{
    attribute_parser_table table;
    table["AnnotationDefault"] = parse_annotation_default_attribute;
    table["BootstrapMethods"] = parse_bootstrap_methods_attribute;
    table["Code"] = parse_code_attribute;
    table["ConstantValue"] = parse_constant_value_attribute;
    table["Deprecated"] = parse_deprecated_attribute;
    table["EnclosingMethod"] = parse_enclosing_method_attribute;
    table["Exceptions"] = parse_exceptions_attribute;
    table["InnerClasses"] = parse_inner_classes_attribute;
    table["LineNumberTable"] = parse_line_number_table_attribute;
    table["LocalVariableTable"] = parse_local_variable_table_attribute;
    table["LocalVariableTypeTable"] = parse_local_variable_type_table_attribute;
    table["RuntimeInvisibleAnnotations"] = parse_runtime_invisible_annotations_attribute;
    table["RuntimeInvisibleParameterAnnotations"] =
    parse_runtime_invisible_parameter_annotations_attribute;
    table["RuntimeVisibleAnnotations"] = parse_runtime_visible_annotations_attribute;
    table["RuntimeVisibleParameterAnnotations"] =
    parse_runtime_visible_parameter_annotations_attribute;
    table["Signature"] = parse_signature_attribute;
    table["SourceFile"] = parse_source_file_attribute;
    table["StackMapTable"] = parse_stack_map_table_attribute;
    table["Synthetic"] = parse_synthetic_attribute;
    return table;
}

static const attribute_parser_table attribute_parsers = build_attribute_parser_table();

entry_attributes parse_attributes(std::ifstream& file, const constant_pool& cp)
{
    entry_attributes field_attributes;
    READ_U2_FIELD(attributes_count, "Failed to parse attributes count of field.");
    for (uint16_t curr_attribute_idx = 0; curr_attribute_idx < attributes_count;
        curr_attribute_idx++)
    {
        READ_U2_FIELD(attribute_name_index, "Failed to parse attribute name index of field.");
        READ_U4_FIELD(attribute_length, "Failed to parse attribute length of field.");
        auto cp_entry_handle = cp.get_entry_info(attribute_name_index);
        // Unexpected attribute entries must be ignored according to the JVM spec.
        if (!cp_entry_handle)
        {
            // Skip the rest of this attribute.
            file.seekg(attribute_length, std::ios::ios_base::cur);
            continue;
        }

        constant_pool_entry_info cp_entry = cp_entry_handle.value();
        // At this point, a valid cp entry has been found but is not a UTF8 symbol. Since
        // attributes must be identified by the UTF8 entry value, the class is probably malformed.
        if (cp_entry.type != constant_pool_type::Utf8)
        {
            throw invalid_class_format{"Attribute name unidentifiable -- cp entry not utf8."};
        }

        auto utf8_entry = std::get<cp_utf8_entry>(cp_entry.entry);
        auto attribute_parser_it = attribute_parsers.find(utf8_entry.value);
        if (attribute_parser_it == attribute_parsers.cend())
        {
            throw invalid_class_format{"Attribute parser not found."};
        }

        // Skip over debugger information.
        if (utf8_entry.value == "SourceDebugExtension")
        {
            file.seekg(attribute_length, std::ios::ios_base::cur);
            continue;
        }

        const auto& parser_fn = attribute_parser_it->second;
        field_attributes.emplace_back(parser_fn(file, cp));
    }

    return field_attributes;
}
