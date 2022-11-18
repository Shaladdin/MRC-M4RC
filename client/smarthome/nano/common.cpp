#include "header.h"

float mapF(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template <typename T>
T diffrence(T num1, T num2)
{
  T diff = num1 - num2;
  return diff * (diff < 0 ? -1 : 1);
}

template <typename T>
bool changed(T oldVar, T newVar, T index)
{
  return diffrence(oldVar, newVar) > index;
}
