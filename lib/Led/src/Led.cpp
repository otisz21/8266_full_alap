
#include "Led.h"

Led::Led(byte pin) {
  this->pin = pin;
  init();
}

void Led::init() {
  pinMode(pin, OUTPUT);
  off();
}

void Led::on() {
  digitalWrite(pin, HIGH);
}

void Led::off() {
  digitalWrite(pin, LOW);
}

int addTwoInts ( int a, int b ) {
  return a + b;
}