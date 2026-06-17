#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor
{
public:
  // For this project, utilization is total jiffies/total uptime (not current
  // utilization). current utilization is acceptable, but beyond the scope of
  // the project
  float Utilization(); // TODO: See src/processor.cpp

  // NA: Declare any necessary private members
private:
};

#endif
