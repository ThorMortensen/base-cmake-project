//
// Created by thm on 5/20/20.
//

#pragma once



#include <array>
#include <stdint.h>

namespace rov {

static const constexpr uint8_t NOP = 0xFF;
static const constexpr size_t HEX_TO_BYTE_LUT_SIZE = 128;
static const constexpr size_t BYTE_TO_HEX_LUT_SIZE = 256;
static const constexpr char NIBBLE_CHART[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Compile time table init
constexpr static ::std::array<uint8_t, HEX_TO_BYTE_LUT_SIZE> initHexToByteLut() {
  using result_t = ::std::array<uint8_t, HEX_TO_BYTE_LUT_SIZE>;
  result_t r = {0};
  for (size_t i = 0; i < HEX_TO_BYTE_LUT_SIZE; ++i) {
    r[i] = NOP;
  }
  r['0'] = 0x0;
  r['1'] = 0x1;
  r['2'] = 0x2;
  r['3'] = 0x3;
  r['4'] = 0x4;
  r['5'] = 0x5;
  r['6'] = 0x6;
  r['7'] = 0x7;
  r['8'] = 0x8;
  r['9'] = 0x9;
  r['A'] = 0xA;
  r['B'] = 0xB;
  r['C'] = 0xC;
  r['D'] = 0xD;
  r['E'] = 0xE;
  r['F'] = 0xF;
  r['a'] = 0xa;
  r['b'] = 0xb;
  r['c'] = 0xc;
  r['d'] = 0xd;
  r['e'] = 0xe;
  r['f'] = 0xf;
  return r;
}

static const constexpr ::std::array<uint8_t, HEX_TO_BYTE_LUT_SIZE> hexToByteLut = initHexToByteLut();

} // namespace rov

