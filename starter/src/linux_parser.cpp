#include "linux_parser.h"

#include <dirent.h>
// #include <pwd.h> // convert uid to username
#include <unistd.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

//  https://stackoverflow.com/a/23376195
//  Idle      = idle + iowait
//  NonIdle   = user + nice + system + irq + softirq + steal
//  Total     = Idle + NonIdle
//  Utilization = (Total - Idle) / Total

// https://man7.org/linux/man-pages/man5/proc.5.html

namespace
{
std::istringstream getStats(int pid)
{
  string line;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, line);
    size_t lastParenthesis = line.find_last_of(')');
    if (lastParenthesis == std::string::npos)
    {
      std::cerr << "ERROR: Could not find ')' in stat file\n";
      return {};
    }
    // start istringstream AFTER last parenthisis
    std::istringstream linestream(line.substr(lastParenthesis + 1));
    return linestream;
  }
  return {}; // Return an empty stream is all else fails
}

} // namespace

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "PRETTY_NAME")
        {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// DONE: BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
  vector<int> pids;
  fs::directory_iterator dir;

  try
  {
    dir = fs::directory_iterator(kProcDirectory);
  }
  catch (const fs::filesystem_error &e)
  {
    std::cerr << "Error reading directory: " << e.what() << '\n';
  }

  for (const auto &entry : dir)
  {
    if (!entry.is_directory())
    {
      continue;
    }

    const std::string filename = entry.path().filename().string();
    if (!filename.empty() &&
        std::all_of(filename.begin(), filename.end(), ::isdigit))
    {
      int pid = std::stoi(filename);
      pids.push_back(pid);
    }
  }
  return pids;
}

// DONE: Read and return the system memory utlization
float LinuxParser::MemoryUtilization()
{
  string line;
  string key;
  string value;
  float memTotal = 0, memFree = 0;

  std::string filepath = kProcDirectory + kMeminfoFilename;
  std::ifstream filestream(filepath);

  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "MemTotal")
        {
          memTotal = stof(value);
        }
        if (key == "MemFree")
        {
          memFree = stof(value);
        }
      }
    }
  }
  if (memTotal == 0)
  {
    return 0;
  }
  return (memTotal - memFree) / memTotal;
}

// DONE: Read and return the system uptime
// https://man7.org/linux/man-pages/man5/proc_uptime.5.html
long LinuxParser::UpTime()
{
  string value;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open())
  {
    filestream >> value;
    return stoi(value);
  }

  return 0;
}

// DONE: Read and return the number of jiffies for the system
// Jiffies is the number of ticks occurred since system boot:
// https://blogs.oracle.com/linux/jiffies-the-heartbeat-of-the-linux-operating-system
// https://man7.org/linux/man-pages/man5/proc_stat.5.html
// http://kernel.org/doc/Documentation/filesystems/proc.txt
// https://github.com/torvalds/linux/blob/master/include/linux/jiffies.h
long LinuxParser::Jiffies()
{
  CpuTime data = CpuUtilization();
  return data.user + data.nice + data.system + data.idle + data.iowait +
         data.irq + data.softirq + data.steal;
}

// DONE: Read and return the number of active jiffies for a PID
// active jiffies = user, nice, system, irq, softirq, steal
long LinuxParser::ActiveJiffies(int pid)
{
  std::istringstream linestream = getStats(pid);
  // note linestream begins at field 3

  string skip; // std::advance seems to introduce off by one bugs. using this
               // to skip
  long utime = 0;
  long stime = 0;

  // auto it = std::istream_iterator<std::string>(linestream);
  // std::advance(it, 12);
  //
  // Looks like the above should have been 10 instead of 12.

  for (int i = 0; i < 11; i++)
  {
    linestream >> skip;
  }

  linestream >> utime >> stime;
  return utime + stime;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  CpuTime data = CpuUtilization();
  return data.idle + data.iowait;
}

// DONE: Read and return CPU utilization
LinuxParser::CpuTime LinuxParser::CpuUtilization()
{
  string line;
  string key;
  // CpuTime struct members:
  // user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0,
  // softirq = 0, steal = 0, guest = 0, guest_nice = 0;
  CpuTime cpuData;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> cpuData.user >> cpuData.nice >> cpuData.system >>
        cpuData.idle >> cpuData.iowait >> cpuData.irq >> cpuData.softirq >>
        cpuData.steal >> cpuData.guest >> cpuData.guest_nice;
  }
  return cpuData;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  string line;
  int value = 0;
  string key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes")
      {
        linestream >> value;
        return value;
      }
    }
  }

  return value;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{

  string line;
  int value = 0;
  string key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running")
      {
        linestream >> value;
        return value;
      }
    }
  }
  return value;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
  string cmd;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, cmd);
    // cat -v /proc/1/cmdline <--- reveiled a bug due to NULLs
    std::replace(cmd.begin(), cmd.end(), '\0', ' ');
  }
  return cmd;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  string line;
  string key;
  string value;

  std::string filepath = kProcDirectory + to_string(pid) + kStatusFilename;
  std::ifstream filestream(filepath);

  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmRSS")
      {
        // std::cerr << "value: " << value << '\n';
        return to_string(stol(value) / 1024);
      }
    }
  }
  return "";
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
  string line;
  string key;
  string value;
  // total up time = uUptime + kUptime

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "Uid")
        {
          return value;
        }
      }
    }
  }
  return value;
}
// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid)
{
  std::string target_uid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  std::string line;
  while (std::getline(stream, line))
  {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream ls(line);
    std::string name, x, uid;
    if (ls >> name >> x >> uid && uid == target_uid)
    {
      return name;
    }
  }
  return ""; // not found
}

// DONE: Read and return the uptime of a process
// https://man7.org/linux/man-pages/man5/proc_pid_stat.5.html (22) starttime
// https://superuser.com/a/1882857
// https: // en.cppreference.com/cpp/iterator/istream_iterator
long LinuxParser::UpTime(int pid)
{
  // linstream begins on field 3
  std::istringstream linestream = getStats(pid);
  const long ticks_per_sec = sysconf(_SC_CLK_TCK);
  long startTime = 0;
  string skip;

  //  auto it = std::istream_iterator<std::string>(linestream);
  //  std::advance(it, 19);
  for (int i = 0; i < 19; i++)
  {
    linestream >> skip;
  }
  linestream >> startTime;

  return UpTime() - (startTime / ticks_per_sec);
}
