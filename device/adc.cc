#include "adc.h"

#include <avr/interrupt.h>

#include "utils/byteorder.h"
#include "utils/movingaverage.h"

static canio::device::Adc* adc_ = nullptr;

ISR(ADC_vect) {
  // If I'm reading the datasheet right, "ADCL must be read first, then ADCH"...
  uint16_t val = ADCL | (ADCH << 8);
  if (adc_) adc_->irq(val);
}

namespace {

void disablePullUps(uint8_t enable_bit_mask) {
  if (enable_bit_mask & 0x01) PORTC &= ~(1 << PC4);
  if (enable_bit_mask & 0x02) PORTC &= ~(1 << PC6);
  if (enable_bit_mask & 0x04) PORTB &= ~(1 << PB5);
  if (enable_bit_mask & 0x08) PORTB &= ~(1 << PB7);

  if (enable_bit_mask & 0x10) PORTB &= ~(1 << PB2);
  if (enable_bit_mask & 0x20) PORTC &= ~(1 << PC5);
  // IO_7 has no ADC, cannot be enabled...
  if (enable_bit_mask & 0x80) PORTB &= ~(1 << PB6);
}

}  // namespace

namespace canio {
namespace device {

// See README.md for pin assignments
const uint8_t ADC_ROTATION[ADC_ROTATION_MAX] = {
  8, 10, 6, 4, 5, 9, /* Unused */ 0, 7
};

Adc::Adc() {
  // Disabled by default
  disable();
}

void Adc::enable(uint8_t enable_bit_mask) {
  if (enable_bit_mask == 0) return;
  enabled_bit_mask_ = enable_bit_mask & 0xBF; // Mask out IO_7 (no ADC)

  // Generally, seems a good idea to disable internal pull-ups,
  // as they effectively become voltage dividers. But of course,
  // it can also save a resistor and work well in some cases.
  // So maybe make this configurable at some point?
  disablePullUps(enable_bit_mask);

  adc_ = this;

  // Enable ADC interrupt; slowest clock reference (1/128)
  ADCSRA = (1 << ADEN) | (1 << ADIF) | (1 << ADIE) | (1 << ADPS2) |
           (1 << ADPS1) | (1 << ADPS0);

  for (auto& avg : average_) avg.clear();

  // Select first ADC
  current_idx_ = -1;
  startNextAdcConversion();
}
 
void Adc::disable() {
  adc_ = nullptr;
  enabled_bit_mask_ = 0;
  ADCSRA = (1 << ADIF);
  for (auto& avg : average_) avg.clear();
}

void Adc::get(uint16_t& value, uint8_t offset) {
  value = utils::lsb_to_msb(average_[offset].get());
}

void Adc::irq(uint16_t value) {
  average_[current_idx_].push(value);
  startNextAdcConversion();
}

void Adc::startNextAdcConversion() {
  if (enabled_bit_mask_ == 0)
    return;  // Prevent endless looping if no ADC is enabled

  do {
    ++current_idx_;
    if (current_idx_ == ADC_ROTATION_MAX) current_idx_ = 0;
  } while ((enabled_bit_mask_ & (1 << current_idx_)) == 0);

  ADMUX = (1 << REFS0) | ADC_ROTATION[current_idx_];
  ADCSRA |= (1 << ADSC);
}

}  // namespace device
}  // namespace canio