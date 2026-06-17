#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization()
{
  return static_cast<float>(LinuxParser::ActiveJiffies()) /
         static_cast<float>(LinuxParser::Jiffies());
}
