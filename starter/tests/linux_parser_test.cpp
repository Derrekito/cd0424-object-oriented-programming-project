#include "linux_parser.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::vector;

// =====================================================================
// Tests for LinuxParser.
// =====================================================================

TEST(LinuxParserMemory, IsValidFraction)
{
  float m = LinuxParser::MemoryUtilization();
  // Utilization is a fraction of total RAM in use: always within [0, 1].
  EXPECT_GE(m, 0.0F) << "Memory utilization went negative: " << m;
  EXPECT_LE(m, 1.0F) << "Memory utilization exceeded 100%: " << m;
}

TEST(LinuxParserPids, IsNotEmpty)
{
  vector<int> pids = LinuxParser::Pids();
  // /proc always contains at least PID 1 (init), so an empty result
  // means the directory scan failed entirely.
  ASSERT_FALSE(pids.empty()) << "Pids() returned empty";
}

TEST(LinuxParserPids, ContainsInit)
{
  vector<int> pids = LinuxParser::Pids();
  // Nothing below makes sense if there are no PIDs to search.
  ASSERT_FALSE(pids.empty()) << "Pids() returned empty";
  // init is always present on a running Linux system.
  EXPECT_NE(std::find(pids.begin(), pids.end(), 1), pids.end())
      << "PID 1 (init) not found among " << pids.size() << " pids";
}

TEST(LinuxParserPids, AllPidsArePositive)
{
  vector<int> pids = LinuxParser::Pids();
  ASSERT_FALSE(pids.empty()) << "Pids() returned no PIDs at all";
  // Every entry must be a real PID parsed from a /proc/<pid> directory.
  for (int pid : pids)
  {
    EXPECT_GT(pid, 0) << "Found a non-positive PID: " << pid;
  }
}
