#ifndef ADC_H
#define ADC_H

#include <stdint.h>

extern "C" void ADC_vect(void) __attribute__((signal));

namespace canio {
namespace device {

class Adc {
 public:
  Adc();

  void disable();
  void enable();

  uint32_t get();
};

}  // namespace device
}  // namespace canio

#endif  // ADC_H