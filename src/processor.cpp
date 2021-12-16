#include <thread>

#include "linux_parser.h"
#include "processor.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // user  nice  system  idle  iowait  irq  softirq  steal  guest guest_nice
  // 74608  2520  24433  1117073  6176  4054  0      0      0     0

  // PrevIdle = previdle + previowait;
  // Idle = idle + iowait;
  // PrevNonIdle =
  //     prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
  // NonIdle = user + nice + system + irq + softirq + steal;

  // PrevTotal = PrevIdle + PrevNonIdle;
  // Total = Idle + NonIdle;
  // differentiate: actual value minus the previous one
  // totald = Total - PrevTotal;
  // idled = Idle - PrevIdle;
  // CPU_Percentage = (totald - idled)/totald;

  const long prev_idle = LinuxParser::IdleJiffies();
  // const long prev_nonidle = LinuxParser::ActiveJiffies();
  const long pre_total = LinuxParser::Jiffies();

  // refresh after 100ms
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  long idle = LinuxParser::IdleJiffies();
  // long nonidle = LinuxParser::ActiveJiffies();
  long total = LinuxParser::Jiffies();

  long totald = total - pre_total;
  long idled = idle - prev_idle;

  return (float)(totald - idled) / totald;
}