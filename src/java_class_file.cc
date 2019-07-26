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
#include <vector>

#include "constant_pool.hh"
#include "field_info.hh"
#include "invalid_class_format_exception.hh"
#include "java_class_file.hh"
#include "method_info.hh"
#include "util.hh"

constexpr uint32_t CLASS_MAGIC_NUMBER = 0xCAFEBABE;

struct version_info {
  uint16_t minor_version;
  uint16_t major_version;
};

java_class_file java_class_file::parse_class_file(const std::string& path) {
  std::ifstream file {path, std::ios::binary};
  if (!file.is_open()) {
    throw std::ios_base::failure{"Classfile not found at " + path};
  }

  READ_U4_FIELD(magic_number, "Failed to parse magic number.");
  // Either a malformed Java classfile or not one at all.
  if (magic_number != CLASS_MAGIC_NUMBER) {
    throw invalid_class_format{"Parsed magic number does not match Java classfile."};
  }

  // TODO needs to convert values to LE.
  version_info version_info;
  constexpr size_t VERSION_INFO_LENGTH = 4;
  if (!file.read(reinterpret_cast<char*>(&version_info), VERSION_INFO_LENGTH)) {
    throw invalid_class_format{"Failed to parse version info of class file."};
  }

  auto constant_pool = constant_pool::parse_constant_pool(file);

  READ_U2_FIELD(access_flag_bytes, "Failed to parse access flags of class file.");
  auto access_flags = classfile_access_flag{access_flag_bytes};

  READ_U2_FIELD(this_index, "Failed to parse `this` index of class file.");
  READ_U2_FIELD(super_index, "Failed to parse `super` index of class file.");
  READ_U2_FIELD(interface_count, "Failed to parse interface count of class file.");

  std::vector<constant_pool_entry_id> interfaces_ids;
  for (uint16_t curr_interface_idx = 0; curr_interface_idx < interface_count; curr_interface_idx++) {
    READ_U2_FIELD(entry_id, "Failed to parse interface id of class file.");
    interfaces_ids.push_back(entry_id);
  }

  auto class_instance = java_class_file{
    std::move(constant_pool), access_flags, this_index, super_index, std::move(interfaces_ids)
  };
  // Parsing fields, methods, and attributes are slightly different in this case because we need to
  // store a reference to the constant pool for these classes. Despite the tight coupling, this is
  // intentional to really simplify the user-facing API.
  class_instance.fields = parse_fields(file, class_instance.cp);
  class_instance.methods = parse_methods(file, class_instance.cp);
  class_instance.attributes = parse_attributes(file, class_instance.cp);
  return class_instance;
}

java_class_file::java_class_file(constant_pool cp, classfile_access_flag access_flags,
  constant_pool_entry_id this_index, constant_pool_entry_id super_index,
  std::vector<constant_pool_entry_id> interfaces_ids, std::vector<field_info> fields,
  std::vector<method_info> methods, entry_attributes attributes) :
    cp{std::move(cp)}, access_flags{access_flags}, this_index{this_index}, super_index{super_index},
    interfaces_ids{std::move(interfaces_ids)}, fields{std::move(fields)},
    methods{std::move(methods)}, attributes{std::move(attributes)} {}

java_class_file::java_class_file(constant_pool cp, classfile_access_flag access_flags,
  constant_pool_entry_id this_index, constant_pool_entry_id super_index,
  std::vector<constant_pool_entry_id> interfaces_ids) :
    java_class_file{std::move(cp), access_flags, this_index, super_index, std::move(interfaces_ids), {}, {}, {}} {}
