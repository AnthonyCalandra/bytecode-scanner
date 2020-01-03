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
#include "inner_classes_attribute.hh"
#include "util.hh"

std::unique_ptr<attribute_info> parse_inner_classes_attribute(std::ifstream& file,
    const constant_pool& cp) {
  READ_U2_FIELD(number_of_classes, "Failed to parse number of classes of InnerClasses attribute.");
  std::vector<inner_class_entry> inner_classes;
  for (uint16_t curr_class_idx = 0; curr_class_idx < number_of_classes; curr_class_idx++) {
    READ_U2_FIELD(inner_class_info_index, "Failed to parse inner class info index of InnerClasses attribute.");
    READ_U2_FIELD(outer_class_info_index, "Failed to parse outer class info index of InnerClasses attribute.");
    READ_U2_FIELD(inner_name_index, "Failed to parse inner name index of InnerClasses attribute.");
    READ_U2_FIELD(inner_class_access_flags, "Failed to parse inner class access flags of InnerClasses attribute.");
    inner_classes.emplace_back(inner_class_info_index, outer_class_info_index,
      inner_name_index, inner_class_access_flags);
  }

  return std::make_unique<inner_classes_attribute>(std::move(inner_classes));
}
