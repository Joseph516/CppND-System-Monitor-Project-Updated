#include <string>

#include "format.h"

using std::string;
using std::to_string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hh = seconds / 3600;
  int mm = (seconds % 3600) / 60;
  int ss = (seconds % 3600) - mm * 60;

  string hh_str = hh < 10 ? '0' + to_string(hh) : to_string(hh);
  string mm_str = mm < 10 ? '0' + to_string(mm) : to_string(mm);
  string ss_str = ss < 10 ? '0' + to_string(ss) : to_string(ss);

  return hh_str + ":" + mm_str + ":" + ss_str;
}