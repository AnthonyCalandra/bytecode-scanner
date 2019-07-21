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
#include "bootstrap_methods_attribute.hh"
#include "constant_pool.hh"
#include "util.hh"

std::unique_ptr<attribute_info> parse_bootstrap_methods_attribute(std::ifstream& file,
    const constant_pool& cp) {
  READ_U2_FIELD(num_bootstrap_methods, "Failed to parse number of bootstrap methods of BootstrapMethods attribute.");
  std::vector<bootstrap_method_entry> bootstrap_methods;
  for (uint16_t curr_bm_idx = 0; curr_bm_idx < num_bootstrap_methods; curr_bm_idx++) {
    READ_U2_FIELD(bootstrap_method_ref, "Failed to parse bootstrap method ref of BootstrapMethods attribute.");
    READ_U2_FIELD(num_bootstrap_arguments, "Failed to parse number of bootstrap arguments of BootstrapMethods attribute.");
    std::vector<uint16_t> bootstrap_arguments;
    for (uint16_t curr_ba_idx = 0; curr_ba_idx < num_bootstrap_arguments; curr_ba_idx++) {
      READ_U2_FIELD(bootstrap_argument, "Failed to parse bootstrap argument of BootstrapMethods attribute.");
      bootstrap_arguments.emplace_back(bootstrap_argument);
    }

    bootstrap_methods.emplace_back(bootstrap_method_entry{
      bootstrap_method_ref, std::move(bootstrap_arguments)
    });
  }

  return std::make_unique<bootstrap_methods_attribute>(std::move(bootstrap_methods));
}
