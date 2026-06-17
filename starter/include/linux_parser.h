#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

// This looks like helper functions in a namespace to extract specific data

namespace LinuxParser
{
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates
{
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

// going crazy keeping these columns straight. Using a human readable struct.
struct CpuTime
{
  long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0,
       softirq = 0, steal = 0, guest = 0, guest_nice = 0;
};

CpuTime CpuUtilization();
long Jiffies();
long ActiveJiffies();
long IdleJiffies();

// Processes
long ActiveJiffies(int pid);
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
}; // namespace LinuxParser

#endif
