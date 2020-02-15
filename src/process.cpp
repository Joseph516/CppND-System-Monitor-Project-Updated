#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  user_ = LinuxParser::User(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
  cpu_usage_ =
      ((float)LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) /
      UpTime();
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
// cpu_usage = 100 * ((ActiveJiffies(pid) / Hertz) / Uptime(pid))
float Process::CpuUtilization() { return cpu_usage_; }

// DONE: Return the command that generated this process
string Process::Command() { return command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return stoi(this->ram_) < stoi(a.ram_);
}

// DONE: Overload the "more than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator>(Process const& a) const {
  return stoi(this->ram_) > stoi(a.ram_);
}