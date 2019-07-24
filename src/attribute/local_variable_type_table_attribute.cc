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
#include <vector>

#include "attribute_info.hh"
#include "constant_pool.hh"
#include "local_variable_type_table_attribute.hh"
#include "util.hh"

std::unique_ptr<attribute_info> parse_local_variable_type_table_attribute(std::ifstream& file,
    const constant_pool& cp) {
  READ_U2_FIELD(local_variable_type_table_length, "Failed to parse local variable type table length of LocalVariableTypeTable attribute.");
  std::vector<local_variable_type_table_entry> local_variable_type_table;
  for (uint16_t curr_lvtt_idx = 0; curr_lvtt_idx < local_variable_type_table_length;
      curr_lvtt_idx++) {
    READ_U2_FIELD(start_pc, "Failed to parse start pc of LocalVariableTypeTable attribute.");
    READ_U2_FIELD(length, "Failed to parse length of LocalVariableTypeTable attribute.");
    READ_U2_FIELD(name_index, "Failed to parse name index of LocalVariableTypeTable attribute.");
    READ_U2_FIELD(signature_index, "Failed to parse signature index of LocalVariableTypeTable attribute.");
    READ_U2_FIELD(index, "Failed to parse index of LocalVariableTypeTable attribute.");
    local_variable_type_table.emplace_back(local_variable_type_table_entry{
      start_pc, length, name_index, signature_index, index
    });
  }

  return std::make_unique<local_variable_type_table_attribute>(std::move(local_variable_type_table));
}
