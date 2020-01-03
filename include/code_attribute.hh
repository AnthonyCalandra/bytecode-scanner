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

#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>

#include "attribute_info.hh"
#include "bytecode.hh"
#include "constant_pool.hh"
#include "util.hh"

struct exception_table_entry {
  uint16_t start_pc;
  uint16_t end_pc;
  uint16_t handler_pc;
  uint16_t catch_pc;

  explicit exception_table_entry(uint16_t start_pc, uint16_t end_pc, uint16_t handler_pc,
    uint16_t catch_pc) :
      start_pc{start_pc}, end_pc{end_pc}, handler_pc{handler_pc}, catch_pc{catch_pc}
  {}
};

class code_attribute: public attribute_info {
  [[maybe_unused]] uint16_t max_stack;
  [[maybe_unused]] uint16_t max_locals;
  // Despite the program counter being a 2-byte short, the code length is a 4-byte int because of
  // the case where an instruction at address 0xFFFF is bigger than a byte (e.g. `lookupswitch` is a
  // variable-sized instruction thus exceeding the max size of the pc).
  uint32_t code_length;
  std::unique_ptr<uint8_t[]> bytecode;
  std::vector<exception_table_entry> exception_table;
  entry_attributes code_attributes;

public:
  explicit code_attribute(uint16_t max_stack, uint16_t max_locals,
    uint32_t code_length, std::unique_ptr<uint8_t[]> bytecode,
    std::vector<exception_table_entry> exception_table, entry_attributes code_attributes) :
      max_stack{max_stack}, max_locals{max_locals},
      code_length{code_length}, bytecode{std::move(bytecode)},
      exception_table{std::move(exception_table)}, code_attributes{std::move(code_attributes)} {}

  template <bytecode_tag instr>
  using find_instructions_cb =
    // Since each instruction tag is one byte, the instruction size minus one is the number of
    // instruction operands.
    typename find_instructions_cb_fn_type<get_instruction_size(instr) - 1>::type;

  template <bytecode_tag instr>
  void find_instruction(find_instructions_cb<instr> cb) const {
    constexpr size_t target_instr_operands_size = get_instruction_size(instr) - 1;
    for (uint32_t pc = 0; pc < code_length;) {
      bytecode_tag curr_instr = static_cast<bytecode_tag>(bytecode[pc]);
      const size_t curr_instr_size = get_instruction_size(curr_instr);
      // These are variable-sized instructions and require special parsing.
      if (curr_instr == bytecode_tag::LOOKUPSWITCH || curr_instr == bytecode_tag::TABLESWITCH) {
        // Skip past padding bytes until we get to the first address that is a multiple of 4.
        if (pc % 4 != 0) {
          for (; pc % 4 != 0; pc++);
        }

        // Skip `default` bytes.
        pc += 4;
        if (curr_instr == bytecode_tag::LOOKUPSWITCH) {
          // `npairs` is a signed 4-byte, big-endian integer. This should probably never be negative.
          uint32_t* npairs_bytes = reinterpret_cast<uint32_t*>(&bytecode[pc]);
          uint32_t npairs = to_little_endian_int(*npairs_bytes);
          // Each pair consists of two 4-byte ints.
          pc += 8 * npairs;
        } else if (curr_instr == bytecode_tag::TABLESWITCH) {
          // Read both `low` and `high` signed ints. These should probably never be negative.
          uint32_t* low_bytes = reinterpret_cast<uint32_t*>(&bytecode[pc]);
          uint32_t low = to_little_endian_int(*low_bytes);
          pc += 4;
          uint32_t* high_bytes = reinterpret_cast<uint32_t*>(&bytecode[pc]);
          uint32_t high = to_little_endian_int(*high_bytes);
          // There are `high - low + 1` signed integer offsets that must be skipped.
          pc += 4 * (high - low + 1 + 1);
        }

        continue;
      }

      if (curr_instr == instr) {
        typename function_args_tuple<decltype(cb)>::type operands;
        std::get<0>(operands) = pc & 0xFFFF;
        // A `constexpr-for` construct would be perfect here...
        if constexpr (target_instr_operands_size >= 1) std::get<1>(operands) = bytecode[pc + 1];
        if constexpr (target_instr_operands_size >= 2) std::get<2>(operands) = bytecode[pc + 2];
        if constexpr (target_instr_operands_size >= 3) std::get<3>(operands) = bytecode[pc + 3];
        if constexpr (target_instr_operands_size >= 4) std::get<4>(operands) = bytecode[pc + 4];
        std::apply(cb, operands);
      }

      pc += curr_instr_size;
    }
  }

  const entry_attributes& get_code_attributes() const {
    return code_attributes;
  }

  virtual attribute_info_type get_type() const {
    return attribute_info_type::Code;
  }
};

std::unique_ptr<attribute_info> parse_code_attribute(std::ifstream& file,
    const constant_pool& cp);
