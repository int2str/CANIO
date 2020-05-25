#ifndef BYTEORDER_H
#define BYTEORDER_H

namespace canio {
namespace utils {

constexpr uint16_t lsb_to_msb(uint16_t lsb) {
  return (lsb & 0xff) << 8 | lsb >> 8;
}

}  // namespace canio
}  // namespace utils

#endif  // BYTEORDER_H