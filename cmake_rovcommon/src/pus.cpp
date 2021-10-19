//
// Created by thm on 13/02/2020.
//

#include "pus.h"
#include "log.h"


uint16_t rov::pus::crcCalc(uint8_t byteToEncode, uint16_t chk) { return (((chk << 8) & 0xFF00) ^ lTable()[(((chk >> 8) ^ byteToEncode) & 0x00FF)]); }

uint16_t rov::pus::crcEncode(const uint8_t *buffer, uint32_t size, uint16_t chk) {

  for (uint32_t index = 0; index < size; index++) {
    chk = crcCalc(buffer[index], chk);
  }
  return chk;
}


/* Decoding procedure */
bool rov::pus::crcIsOk(const uint8_t *buffer, uint32_t size, uint16_t chk) {
  /* The error detection syndrome, S(x) is given by: */
  /* S(x)=(x^16 * C¤(x) + x^n * L(x)) modulo G(x) */
  /* If S(x) = 0 then no error is detected. */
  for (uint32_t index = 0; index < size; index++) {
    chk = crcCalc(buffer[index], chk);
  }
  return (chk == 0);
}

bool rov::pus::crcIsOk(const std::vector<uint8_t> &buffer, uint16_t chk) { return crcIsOk(buffer.data(), buffer.size(), chk); }

std::vector<uint8_t> &rov::pus::crcSetChecksum16bit(std::vector<uint8_t> &buffer, uint16_t chk, size_t offset) {
  uint16_t checkSum = chk;
  for (auto byte : buffer) {
    checkSum += byte;
  }

  *(buffer.end() - (2 + offset)) = (checkSum >> 8) & 0xff;
  *(buffer.end() - (1 + offset)) = (checkSum & 0xff);
  return buffer;
}

std::vector<uint8_t>::iterator rov::pus::crcSet(std::vector<uint8_t>::iterator itS, std::vector<uint8_t>::iterator itE, uint16_t chk) {
  while (itS != itE - 2) {
    chk = crcCalc(*itS, chk);
    itS++;
  }

  *(itE - (2)) = (chk >> 8) & 0xff;
  *(itE - (1)) = (chk & 0xff);

  return itE - (2);
}

std::vector<uint8_t>::iterator rov::pus::crcSetChecksum16bit(std::vector<uint8_t>::iterator itS, std::vector<uint8_t>::iterator itE, uint16_t chk) {
  while (itS != itE - 2) {
    chk += *itS;
    itS++;
  }

  *(itE - (2)) = (chk >> 8) & 0xff;
  *(itE - (1)) = (chk & 0xff);

  return itE - (2);
}

std::vector<uint8_t> &rov::pus::crcAppend(std::vector<uint8_t> &buffer, uint16_t chk) {
  uint16_t crc = crcEncode(buffer.data(), buffer.size(), chk);
  buffer.emplace_back((crc >> 8) & 0xff);
  buffer.emplace_back(crc & 0xff);
  return buffer;
}


rov::pus::tc::tc(uint8_t serviceTypeValue, uint8_t subServiceTypeValue, size_t dataSize)
    :// clang-format off
  /////////// TC packet primary header setup ///////////
  TCPH_pkgVersionNumber(3,0),
  TCPH_pkgType(1,0),
  TCPH_secondHeaderFlag(1,0),
  TCPH_apid(11, 0),
  TCPH_sequenceFlags(2, 0),
  TCPH_sequenceCount(14, 0),
  TCPH_pkgLength(16, 0),

  /////////// TC packet secondary header setup ///////////
  TCSH_pusVersion(4, 0),
  TCSH_ack(4, 0),
  TCSH_serviceType(8,serviceTypeValue),
  TCSH_subServiceType(8,subServiceTypeValue),
  TCSH_sourceId(8,16),
//  TCSH_spare(0,0),

  /////////// TC Package Data setup ///////////
  pkg(dataSize + PUS_COMBINED_HEADER_SIZE + PEC_SIZE)
// clang-format on
{}

rov::pus::tc &rov::pus::tc::operator++() {
  TCPH_sequenceCount.val++;
  if (TCPH_sequenceCount.val == 0x4000) {
    TCPH_sequenceCount.val = 0;
  }
  return *this;
}

rov::pus::tc rov::pus::tc::operator++(int) {
  tc temp = *this;
  ++*this;
  return temp;
}
void rov::pus::tc::packInto(std::vector<uint8_t>::iterator destinationStart, std::vector<uint8_t>::iterator destinationEnd, const std::vector<uint8_t> &payload) {

  field header;
  uint32_t pusSize = payload.size() + TCSH_HEADER_SIZE + PEC_SIZE - 1;

  TCPH_pkgLength.val = pusSize;

  header = TCPH_pkgVersionNumber << TCPH_pkgType << TCPH_secondHeaderFlag << TCPH_apid << TCPH_sequenceFlags << TCPH_sequenceCount << TCPH_pkgLength;

  for (int i = 0; i < TCPH_HEADER_SIZE; ++i) {
    *(destinationStart + (TCPH_HEADER_SIZE - 1) - i) = (header.val >> 8u * i) & 0xffu;
  }

  header = TCSH_pusVersion << TCSH_ack << TCSH_serviceType << TCSH_subServiceType << TCSH_sourceId;

  for (int i = 0; i < TCSH_HEADER_SIZE; ++i) {
    *(destinationStart+ (TCPH_HEADER_SIZE + TCSH_HEADER_SIZE - 1) - i) = (header.val >> 8u * i) & 0xffu;
  }

  std::copy(payload.begin(), payload.end(), destinationStart + PUS_COMBINED_HEADER_SIZE);
  rov::pus::crcSet(destinationStart, destinationEnd);
}

rov::pus::tc *rov::pus::tc::pack(const std::vector<uint8_t> &payload) {
  packInto(pkg.begin(), pkg.end(), payload);
  return this;
}

const std::vector<uint8_t> &rov::pus::tc::getpkg() { return pkg; }
size_t rov::pus::tc::size() { return pkg.capacity(); }

void rov::pus::tc::pp() {
  rov::DBP_HEX(TCPH_apid.val);
  rov::DBP_HEX(TCPH_sequenceFlags.val);
  rov::DBP_HEX(TCPH_sequenceCount.val);
  rov::DBP_HEX(TCPH_pkgLength.val);

  rov::DBP_HEX(TCSH_pusVersion.val);
  rov::DBP_HEX(TCSH_ack.val);
  rov::DBP_HEX(TCSH_serviceType.val);
  rov::DBP_HEX(TCSH_subServiceType.val);
  rov::DBP_HEX(TCSH_sourceId.val);
}


