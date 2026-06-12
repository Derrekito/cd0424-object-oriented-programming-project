#include "format.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using std::string;
using namespace std::chrono;
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
std::string Format::ElapsedTime(long seconds) {
  // prevent negative time
  seconds = std::max<long>(seconds, 0);

  // Convert long into a chrono object
  std::chrono::seconds duration{seconds};

  // Format directly to HH:MM:SS with leading zeros
  // I'm sticking to C++17 to better prepare for my target industry
  // std::chrono::hh_mm_ss hms{duration};

  // https://devdocs.io/cpp/header/chrono
  //  auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
  //  duration -= hours;
  //  auto mins = std::chrono::duration_cast<std::chrono::minutes>(duration);
  //  duration -= mins;
  //  // auto secs = std::chrono::duration_cast<std::chrono::seconds>(duration);
  //  auto secs = duration;  // left over seconds is all that's left

  // Break down into hours, minutes, seconds
  auto hours = duration_cast<std::chrono::hours>(duration);
  auto remaining = duration - hours;
  auto mins = duration_cast<std::chrono::minutes>(remaining);
  remaining -= mins;
  auto secs = duration_cast<std::chrono::seconds>(remaining);

  // Build the string with leading zeros
  // https://devdocs.io/cpp/header/ostream
  std::ostringstream oss;
  // fill character set to 0 with std::setfill('0')
  // fill width set with std::setw(2). First use is to cover hours, next is to
  // cover minutes, then again for seconds for a has fill of 00:00:00
  oss << std::setfill('0') << std::setw(2) << hours.count() << ":"
      << std::setw(2) << mins.count() << ":" << std::setw(2) << secs.count();

  return oss.str();
}
