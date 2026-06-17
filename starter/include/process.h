#ifndef PROCESS_H
#define PROCESS_H

#include "linux_parser.h"
#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process
{
public:
  int Pid();              // DONE: See src/process.cpp
  std::string User();     // DONE: See src/process.cpp
  std::string Command();  // DONE: See src/process.cpp
  float CpuUtilization(); // DONE: See src/process.cpp
  std::string Ram();      // DONE: See src/process.cpp
  long int UpTime();      // DONE: See src/process.cpp
  long int ProcUpTime();
  bool operator<(Process const &a) const; // TODO: See src/process.cpp

  Process(int pid);

  // TODO: Declare any necessary private members
private:
  int pid_;
  std::string user_;
  std::string cmd_;
  float cpuUtilization_;
  std::string ram_;
  long int upTime_;
};

#endif
