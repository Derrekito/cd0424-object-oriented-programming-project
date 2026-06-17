#include "process.h"
#include "linux_parser.h"
#include <cctype>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    : pid_(pid), user_(LinuxParser::User(pid)), cmd_(LinuxParser::Command(pid)),
      cpuUtilization_(CpuUtilization()), ram_(LinuxParser::Ram(pid)),
      upTime_(LinuxParser::UpTime(pid))
{
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization()
{
  if (LinuxParser::UpTime(pid_) == 0)
  {
    return 0;
  }
  return (static_cast<float>(LinuxParser::ActiveJiffies(pid_)) /
          sysconf(_SC_CLK_TCK)) /
         LinuxParser::UpTime(pid_);
  // return cpuUtilization_;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const &a) const
{
  return cpuUtilization_ > a.cpuUtilization_;
}
