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
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "cxxopts.hh"

#include "find_api_calls.hh"
#include "invalid_class_format_exception.hh"
#include "java_class_file.hh"

std::vector<std::string> denormalize_api_names(std::vector<std::string> apis) {
  std::for_each(apis.begin(), apis.end(), [](std::string& api) {
    std::replace(api.begin(), api.end(), '.', '/');
    api.erase(std::remove(api.begin(), api.end(), ' '), api.end());
  });
  return apis;
}

void do_command(cxxopts::ParseResult args) {
  const auto class_name = args["input"].as<std::string>();
  // The constant pool stores APIs as, for example, "java/io/PrintStream" instead of the
  // common convention of "java.io.PrintStream".
  auto api_names = denormalize_api_names(args["scan"].as<std::vector<std::string>>());
  try {
    const auto clazz = java_class_file::parse_class_file(class_name);
    std::cout << "Found the following API calls in " << class_name << ":" << std::endl;
    const auto calls = find_api_calls(clazz, api_names);
    for (const auto& call : calls) {
      std::cout << '\t' << call.api_str << " in method " << call.method << " on line " <<
        call.line_number << std::endl;
    }
  } catch (const std::ios::failure& io_failure) {
    std::cerr << io_failure.what() << std::endl;
  } catch (const invalid_class_format& icf) {
    std::cerr << icf.what() << std::endl;
  }
}

int main(int argc, char** argv) {
  cxxopts::Options options("bytecode-scanner", "Java bytecode utility");
  options
    .allow_unrecognised_options()
    .add_options()
      ("input", "Input class file", cxxopts::value<std::string>())
      ("s,scan", "Scan for a CSV list of APIs", cxxopts::value<std::vector<std::string>>())
      ("a,dump-all", "Dump full class file information")
      ("dump-cp", "Dump constant pool");
  options.parse_positional({ "input" });

  bool error = false;
  try {
    cxxopts::ParseResult args = options.parse(argc, argv);
    if (!args.count("input")) {
      error = true;
    }

    if (!error) {
      do_command(std::move(args));
    }
  } catch (const cxxopts::OptionException& e) {
    error = true;
  }

  if (error) {
    std::cerr << options.help() << std::endl;
    return 1;
  }

  return 0;
}
