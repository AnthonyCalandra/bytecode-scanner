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
#include <memory>
#include <vector>

#include "code_attribute.hh"

std::unique_ptr<attribute_info> parse_code_attribute(std::ifstream& file,
    const constant_pool& cp) {
  READ_U2_FIELD(max_stack, "Failed to parse max stack count of Code attribute.");
  READ_U2_FIELD(max_locals, "Failed to parse max local count of Code attribute.");
  READ_U4_FIELD(code_length, "Failed to parse code length of Code attribute.");
  auto bytecode = std::make_unique<uint8_t[]>(code_length);
  if (!file.read(reinterpret_cast<char*>(bytecode.get()), code_length)) {
    throw invalid_class_format{"Failed to parse bytecode of Code attribute."};
  }

  READ_U2_FIELD(exception_table_length, "Failed to parse exception table length of Code attribute.");
  std::vector<exception_table_entry> exception_table;
  for (uint16_t curr_et_idx = 0; curr_et_idx < exception_table_length; curr_et_idx++) {
    READ_U2_FIELD(start_pc, "Failed to parse start pc of Code attribute.");
    READ_U2_FIELD(end_pc, "Failed to parse end pc of Code attribute.");
    READ_U2_FIELD(handler_pc, "Failed to parse handler pc of Code attribute.");
    READ_U2_FIELD(catch_pc, "Failed to parse catch pc of Code attribute.");
    exception_table.emplace_back(exception_table_entry{start_pc, end_pc, handler_pc, catch_pc});
  }

  return std::make_unique<code_attribute>(max_stack, max_locals, code_length, std::move(bytecode),
    std::move(exception_table), parse_attributes(file, cp));
}
