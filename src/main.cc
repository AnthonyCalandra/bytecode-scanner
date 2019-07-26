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

#include <iostream>
#include <optional>
#include <vector>

#include "find_api_calls.hh"
#include "invalid_class_format_exception.hh"
#include "java_class_file.hh"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "USAGE: " << argv[0] << " <classfile-name> <api-name>" << std::endl;
    std::cerr << "EXAMPLE: " << argv[0] << " Foo.class java/io/PrintStream" << std::endl;
    return -1;
  }

  std::string class_name = argv[1];
  std::string api_name = argv[2];
  try {
    const auto clazz = java_class_file::parse_class_file(class_name);
    std::cout << "Found the following " << api_name << " API calls in " << class_name << ":" <<
      std::endl;
    find_api_calls(clazz, api_name);
  } catch (const std::ios::failure& io_failure) {
    std::cerr << io_failure.what() << std::endl;
  } catch (const invalid_class_format& icf) {
    std::cerr << icf.what() << std::endl;
  }

  return 0;
}
