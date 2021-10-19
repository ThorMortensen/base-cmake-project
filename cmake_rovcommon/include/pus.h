//
// Created by thm on 13/02/2020.
//

#ifndef UART_PI_PUSPACKER_H
#define UART_PI_PUSPACKER_H

#include <array>
#include <bitset>
#include <cstdint>
#include <vector>

namespace rov {

  class pus {
public:
    struct field {
      field(const size_t size, uint64_t value) : bitSize(size), val(value) {}
      field() {}
      size_t bitSize = 0;
      uint64_t val = 0;
      field operator<<(const field &rhs) const {
        return field(bitSize + rhs.bitSize, ((val << rhs.bitSize) | rhs.val));
      }
    };

    static constexpr auto DEFAULT_CRC_SYNDROME = 0xFFFF;

    [[nodiscard]] static uint16_t crcEncode(const uint8_t *buffer, uint32_t size, uint16_t chk = DEFAULT_CRC_SYNDROME /* Reset CRC syndrome to all ones */);
    [[nodiscard]] static bool crcIsOk(const uint8_t *buffer, uint32_t size, uint16_t chk = DEFAULT_CRC_SYNDROME /* Reset CRC syndrome to all ones */);
    [[nodiscard]] static bool crcIsOk(const std::vector<uint8_t> &buffer, uint16_t chk = DEFAULT_CRC_SYNDROME);
    static std::vector<uint8_t> &crcAppend(std::vector<uint8_t> &buffer, uint16_t chk);
    static std::vector<uint8_t>::iterator crcSet(std::vector<uint8_t>::iterator itS, std::vector<uint8_t>::iterator itE, uint16_t chk = DEFAULT_CRC_SYNDROME);
    static std::vector<uint8_t> &crcSetChecksum16bit(std::vector<uint8_t> &buffer, uint16_t chk = 0, size_t offset = 0);
    static std::vector<uint8_t>::iterator crcSetChecksum16bit(std::vector<uint8_t>::iterator itS, std::vector<uint8_t>::iterator itE, uint16_t chk = 0);


    class tc {

  public:
      static constexpr uint8_t ACCEPTANCE_ACK = 0b0000;
      static constexpr uint8_t COMPLETION_ACK = 0b1000;

      static constexpr uint8_t PUS_SIZE_OFFSET = 4;
      static constexpr uint8_t TCPH_HEADER_SIZE = 6;
      static constexpr uint8_t TCSH_HEADER_SIZE = 5;
      static constexpr uint8_t PEC_SIZE = 2;
      static constexpr uint8_t PUS_COMBINED_HEADER_SIZE = TCSH_HEADER_SIZE + TCPH_HEADER_SIZE;
      static constexpr uint8_t PUS_SERVICE_OFFSET = 7;
      static constexpr uint8_t PUS_SUB_SERVICE_OFFSET = 8;

      tc(uint8_t serviceType, uint8_t subServiceType, size_t dataSize);
      //      tc(const tc &obj);
      tc() = default;
      tc &operator++();
      tc operator++(int);
      tc *pack(const std::vector<uint8_t> &payload);
      void packInto(std::vector<uint8_t>::iterator destinationStart, std::vector<uint8_t>::iterator destinationEnd, const std::vector<uint8_t> &payload);
      const std::vector<uint8_t> &getpkg();
      size_t size();
      void pp();

      /***************************
       * TC packet primary header
       ***************************/
      field TCPH_pkgVersionNumber;
      field TCPH_pkgType;
      field TCPH_secondHeaderFlag;
      field TCPH_apid;
      field TCPH_sequenceFlags;
      field TCPH_sequenceCount;
      field TCPH_pkgLength;

      /***************************
       * TC packet secondary header
       ***************************/
      field TCSH_pusVersion;
      field TCSH_ack;
      field TCSH_serviceType;
      field TCSH_subServiceType;
      field TCSH_sourceId;


  private:
      std::vector<uint8_t> pkg;
    };

private:
    // Compile time table init
    constexpr static ::std::array<uint16_t, 256> lTable() {
      using result_t = ::std::array<uint16_t, 256>;
      result_t r = {0};
      uint16_t tmp = 0; /* Temporary value */
      uint16_t itable = 0;
      for (itable = 0; itable < 256; itable++) {
        tmp = 0;
        if ((itable & 1) != 0)
          tmp = tmp ^ 0x1021;
        if ((itable & 2) != 0)
          tmp = tmp ^ 0x2042;
        if ((itable & 4) != 0)
          tmp = tmp ^ 0x4084;
        if ((itable & 8) != 0)
          tmp = tmp ^ 0x8108;
        if ((itable & 16) != 0)
          tmp = tmp ^ 0x1231;
        if ((itable & 32) != 0)
          tmp = tmp ^ 0x2462;
        if ((itable & 64) != 0)
          tmp = tmp ^ 0x48C4;
        if ((itable & 128) != 0)
          tmp = tmp ^ 0x9188;
        r[itable] = tmp;
      }
      return r;
    }

    //    static constexpr ::std::array<uint16_t, 256> lTable = rov::pus::initLTable();


    [[nodiscard]] static uint16_t crcCalc(uint8_t byteToEncode, uint16_t chk /* Syndrome */);
  };


}// namespace rov
#endif// UART_PI_PUSPACKER_H
