#include "adc.h"

#include <avr/interrupt.h>

#include "utils/movingaverage.h"

namespace {

const uint8_t ADC_ROTATION_MAX = 3;
const uint8_t ADC_ROTATION[ADC_ROTATION_MAX] = {0x08, 0x09, 0x06};

uint8_t adc_current = 0;

canio::utils::MovingAverage<uint16_t, 20> adc_avg[ADC_ROTATION_MAX];

ISR(ADC_vect) {
  adc_avg[adc_current++].push(ADCH);

  if (adc_current == ADC_ROTATION_MAX) adc_current = 0;

  ADMUX = (1 << REFS0) | (1 << ADLAR) | ADC_ROTATION[adc_current];
  ADCSRA |= (1 << ADSC);
}

}  // namespace

namespace canio {
namespace device {

Adc::Adc() {
  // Disabled by default
  disable();
}

void Adc::enable() {
  // Enable ADC interrupt; slowest clock reference (1/128)
  ADCSRA = (1 << ADEN) | (1 << ADIF) | (1 << ADIE) | (1 << ADPS2) |
           (1 << ADPS1) | (1 << ADPS0);

  for (auto& avg : adc_avg) avg.clear();

  // Select first ADC
  adc_current = 0;
  ADMUX = (1 << REFS0) | (1 << ADLAR) | ADC_ROTATION[adc_current];

  // Start first conversion; ISR will re-start conversion
  ADCSRA |= (1 << ADSC);
}

void Adc::disable() {
  ADCSRA = (1 << ADIF);
  for (auto& avg : adc_avg) avg.clear();
}

uint32_t Adc::get() {
  uint32_t ret = 0;
  for (const auto& avg : adc_avg) {
    ret <<= 8;
    ret |= (avg.get() & 0xFF);
  }
  return ret;
}

}  // namespace device
}  // namespace canio