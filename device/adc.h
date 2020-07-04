#ifndef ADC_H
#define ADC_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "utils/movingaverage.h"

extern "C" void ADC_vect() __attribute__((signal));

namespace canio {
namespace device {

const uint8_t ADC_ROTATION_MAX = 8;
const uint8_t ADC_MOVING_AVERAGE_SAMPLES = 20;

class Adc {
 public:
  Adc();

  void disable();
  void enable(uint8_t enable_bit_mask);

  void get(uint16_t& value, uint8_t offset);

 private:
  uint8_t enabled_bit_mask_;
  uint8_t current_idx_;
  canio::utils::MovingAverage<uint32_t, ADC_MOVING_AVERAGE_SAMPLES> average_[ADC_ROTATION_MAX];

  void startNextAdcConversion();

  void irq(uint16_t value);
  friend void ::ADC_vect();
};

}  // namespace device
}  // namespace canio

#endif  // ADC_H