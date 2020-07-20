#ifndef BYTEORDER_H
#define BYTEORDER_H

namespace canio {
namespace utils {

template <typename T>
constexpr T lsb_to_msb(T);

template <>
constexpr uint16_t lsb_to_msb(uint16_t lsb) {
  return (lsb & 0xff) << 8 | lsb >> 8;
}

template <>
constexpr uint32_t lsb_to_msb(uint32_t lsb) {
  return (lsb & 0xff) << 24 | (lsb & 0xff00) << 8 | (lsb & 0xff0000) >> 8 |
         lsb >> 24;
}

}  // namespace utils
}  // namespace canio

#endif  // BYTEORDER_H