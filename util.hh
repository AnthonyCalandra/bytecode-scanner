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

#include <cstdint>
#include <functional>
#include <tuple>

#include "invalid_class_format_exception.hh"

inline uint32_t to_little_endian_int(uint32_t n) {
  return (n & 0xFF) << 24 | ((n >> 8) & 0xFF) << 16 | ((n >> 16) & 0xFF) << 8 | ((n >> 24) & 0xFF);
}

inline uint16_t to_little_endian_short(uint16_t n) {
  return (n & 0xFF) << 8 | ((n >> 8) & 0xFF);
}

inline uint64_t to_little_endian_long(uint64_t n) {
  return (n & 0xFF) << 56 | ((n >> 8) & 0xFF) << 48 | ((n >> 16) & 0xFF) << 40 |
    ((n >> 24) & 0xFF) << 32 | ((n >> 32) & 0xFF) << 24 | ((n >> 40) & 0xFF) << 16 |
    ((n >> 48) & 0xFF) << 8 | ((n >> 56) & 0xFF);
}

#define READ_U1_FIELD(var, err_msg) \
  uint8_t var; \
  do { \
    constexpr size_t var##_LENGTH = 1; \
    if (!file.read(reinterpret_cast<char*>(&var), var##_LENGTH)) { \
      throw invalid_class_format{err_msg}; \
    } \
  } while (0)

#define READ_U2_FIELD(var, err_msg) \
  uint16_t var; \
  do { \
    constexpr size_t var##_LENGTH = 2; \
    if (!file.read(reinterpret_cast<char*>(&var), var##_LENGTH)) { \
      throw invalid_class_format{err_msg}; \
    } \
    var = to_little_endian_short(var); \
  } while (0)

#define READ_U4_FIELD(var, err_msg) \
  uint32_t var; \
  do { \
    constexpr size_t var##_LENGTH = 4; \
    if (!file.read(reinterpret_cast<char*>(&var), var##_LENGTH)) { \
      throw invalid_class_format{err_msg}; \
    } \
    var = to_little_endian_int(var); \
  } while (0)

template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename R>
struct function_args_tuple;

template <typename R, typename ...Args>
struct function_args_tuple<std::function<R(Args...)>> {
  using type = std::tuple<Args...>;
};

template <size_t instr_size>
struct find_instructions_cb_fn_type {
  using type = std::function<void(uint16_t)>;
};
template <>
struct find_instructions_cb_fn_type<1> {
  using type = std::function<void(uint16_t, uint8_t)>;
};
template <>
struct find_instructions_cb_fn_type<2> {
  using type = std::function<void(uint16_t, uint8_t, uint8_t)>;
};
template <>
struct find_instructions_cb_fn_type<3> {
  using type = std::function<void(uint16_t, uint8_t, uint8_t, uint8_t)>;
};
template <>
struct find_instructions_cb_fn_type<4> {
  using type = std::function<void(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t)>;
};
