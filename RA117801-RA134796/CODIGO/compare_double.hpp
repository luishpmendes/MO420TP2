#pragma once
bool lessDouble(double a, double b) {
  return a > b+EPS;
}

bool equalDouble(double a, double b) {
  return !lessDouble(a, b) && !lessDouble(b, a);
}
