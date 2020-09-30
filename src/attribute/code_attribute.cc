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
#include <iomanip>
#include <memory>
#include <vector>

#include "code_attribute.hh"

void code_attribute::print_code(std::ostream& out) const
{
    for (uint32_t pc = 0; pc < code_length;)
    {
        const bytecode_tag curr_instr = static_cast<bytecode_tag>(bytecode[pc]);
        const size_t curr_instr_size = get_instruction_size(curr_instr);

        out << std::right << pc << ": " << std::left << get_instruction_name(curr_instr);
        // These are variable-sized instructions and require special parsing.
        if (curr_instr == bytecode_tag::LOOKUPSWITCH ||
            curr_instr == bytecode_tag::TABLESWITCH)
        {
            // Skip past padding bytes until we get to the first address that is a multiple of 4.
            if (pc % 4 != 0)
            {
                for (; pc % 4 != 0; pc++);
            }

            // Skip `default` bytes.
            pc += 4;
            if (curr_instr == bytecode_tag::LOOKUPSWITCH)
            {
                // `npairs` is a signed 4-byte, big-endian integer. This should probably never
                // be negative.
                uint32_t* npairs_bytes = reinterpret_cast<uint32_t*>(&bytecode[pc]);
                uint32_t npairs = to_little_endian_int(*npairs_bytes);
                // Each pair consists of two 4-byte ints.
                pc += 8 * npairs;
            }
            else if (curr_instr == bytecode_tag::TABLESWITCH)
            {
                // Read both `low` and `high` signed ints. These should probably never be
                // negative.
                uint32_t* low_bytes = reinterpret_cast<uint32_t*>(&bytecode[pc]);
                uint32_t low = to_little_endian_int(*low_bytes);
                pc += 4;

                uint32_t* high_bytes = reinterpret_cast<uint32_t*>(&bytecode[pc]);
                uint32_t high = to_little_endian_int(*high_bytes);
                // There are `high - low + 1` signed integer offsets that must be skipped.
                pc += 4 * (high - low + 1 + 1);
            }

            out << std::endl;
            continue;
        }

        // Instructions with an operand that references the constant pool.
        if (curr_instr == bytecode_tag::GETSTATIC ||
            curr_instr == bytecode_tag::PUTSTATIC ||
            curr_instr == bytecode_tag::GETFIELD ||
            curr_instr == bytecode_tag::PUTFIELD ||
            curr_instr == bytecode_tag::LDC_W ||
            curr_instr == bytecode_tag::LDC2_W ||
            curr_instr == bytecode_tag::INVOKEVIRTUAL ||
            curr_instr == bytecode_tag::INVOKESPECIAL ||
            curr_instr == bytecode_tag::INVOKESTATIC ||
            curr_instr == bytecode_tag::INVOKEINTERFACE ||
            curr_instr == bytecode_tag::NEW ||
            curr_instr == bytecode_tag::CHECKCAST ||
            curr_instr == bytecode_tag::INSTANCEOF ||
            curr_instr == bytecode_tag::ANEWARRAY ||
            curr_instr == bytecode_tag::MULTIANEWARRAY)
        {
            uint8_t* index1 = reinterpret_cast<uint8_t*>(&bytecode[pc + 1]);
            uint8_t* index2 = reinterpret_cast<uint8_t*>(&bytecode[pc + 2]);
            constant_pool_entry_id cp_index = (*index1 << 8) | *index2;
            out << '\t' << "#" << cp_index << "// ";
        }

        pc += curr_instr_size;
        out << std::endl;
    }
}

std::unique_ptr<attribute_info> parse_code_attribute(std::ifstream& file,
    const constant_pool& cp)
{
    READ_U2_FIELD(max_stack, "Failed to parse max stack count of Code attribute.");
    READ_U2_FIELD(max_locals, "Failed to parse max local count of Code attribute.");
    READ_U4_FIELD(code_length, "Failed to parse code length of Code attribute.");
    auto bytecode = std::make_unique<uint8_t[]>(code_length);
    if (!file.read(reinterpret_cast<char*>(bytecode.get()), code_length))
    {
        throw invalid_class_format{"Failed to parse bytecode of Code attribute."};
    }

    READ_U2_FIELD(exception_table_length, "Failed to parse exception table length of Code "
        "attribute.");
    std::vector<exception_table_entry> exception_table;
    for (uint16_t curr_et_idx = 0; curr_et_idx < exception_table_length; curr_et_idx++)
    {
        READ_U2_FIELD(start_pc, "Failed to parse start pc of Code attribute.");
        READ_U2_FIELD(end_pc, "Failed to parse end pc of Code attribute.");
        READ_U2_FIELD(handler_pc, "Failed to parse handler pc of Code attribute.");
        READ_U2_FIELD(catch_pc, "Failed to parse catch pc of Code attribute.");
        exception_table.emplace_back(start_pc, end_pc, handler_pc, catch_pc);
    }

    return std::make_unique<code_attribute>(cp, max_stack, max_locals, code_length,
        std::move(bytecode), std::move(exception_table), parse_attributes(file, cp));
}
