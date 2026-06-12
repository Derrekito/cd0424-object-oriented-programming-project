#include "format.h"

#include <gtest/gtest.h>

#include <string>

using std::string;

// ============================
// Tests for Format::ElapsedTime
// ============================

TEST(FormatElapsedTime, HandlesZeroSeconds) {
  EXPECT_EQ("00:00:00", Format::ElapsedTime(0));
}

TEST(FormatElapsedTime, HandlesOneSecond) {
  EXPECT_EQ("00:00:01", Format::ElapsedTime(1));
}

TEST(FormatElapsedTime, HandlesJustUnderOneMinute) {
  EXPECT_EQ("00:00:59", Format::ElapsedTime(59));
}

TEST(FormatElapsedTime, HandlesOneMinute) {
  EXPECT_EQ("00:01:00", Format::ElapsedTime(60));
}

TEST(FormatElapsedTime, HandlesOneHour) {
  EXPECT_EQ("01:00:00", Format::ElapsedTime(3600));
}

TEST(FormatElapsedTime, HandlesTypicalTime) {
  EXPECT_EQ("01:01:01", Format::ElapsedTime(3661));
}

TEST(FormatElapsedTime, HandlesMultipleHours) {
  EXPECT_EQ("10:25:30", Format::ElapsedTime(37530));
}

TEST(FormatElapsedTime, HandlesVeryLargeTime) {
  EXPECT_EQ("99:59:59", Format::ElapsedTime(359999));
}

TEST(FormatElapsedTime, NegativeInputBecomesZero) {
  EXPECT_EQ("00:00:00", Format::ElapsedTime(-1));
  EXPECT_EQ("00:00:00", Format::ElapsedTime(-999));
}
