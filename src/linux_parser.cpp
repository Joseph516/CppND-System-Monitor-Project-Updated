#include <dirent.h>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "format.h"
#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string key, kb;
  long value;
  string line;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  std::unordered_map<string, long> meminfo;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value >> kb;
      meminfo[key] = value;
    }

    stream.close();
    return (float)(meminfo["MemTotal"] - meminfo["MemFree"]) /
           meminfo["MemTotal"];
  }

  return 0.0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime, idle_time;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle_time;
    stream.close();
    return uptime;
  }

  return 0;
}

// DONE: Read and return the number of jiffies for the system
// Hint: jiffies store the number of ticks occurred since system start-up. HZ is
// defined as number of ticks in one second, and jiffies is number of ticks
// occurrd, see the number in /proc/stat
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// /proc/pid/stat/ #14,15,16,16,22, relevant with function UpTime(int pid)
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;

  long utime, stime, cutime, cstime, /*starttime,*/ total_time;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // 使用流迭代器提取#14，15，16，17，22位置的数据(in clock ticks)
    std::istream_iterator<string> in_iter(linestream), eof;
    std::vector<string> single_word(in_iter, eof);
    // start from 0
    utime = stol(single_word[13]);
    stime = stol(single_word[14]);
    cutime = stol(single_word[15]);
    cstime = stol(single_word[16]);
    // starttime = stol(single_word[21]);  // omit permitted

    total_time = utime + stime + cutime + cstime;
    stream.close();
  }

  return total_time;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_jiffies = CpuUtilization();
  return stol(cpu_jiffies[kUser_]) + stol(cpu_jiffies[kNice_]) +
         stol(cpu_jiffies[kSystem_]) + stol(cpu_jiffies[kIRQ_]) +
         stol(cpu_jiffies[kSoftIRQ_]) + stol(cpu_jiffies[kSteal_]) +
         stol(cpu_jiffies[kGuest_]) + stol(cpu_jiffies[kGuestNice_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_jiffies = CpuUtilization();
  return stol(cpu_jiffies[kIdle_]) + stol(cpu_jiffies[kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, cpu, value;
  std::vector<string> cpu_jiffies{};

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
      cpu_jiffies.push_back(value);
    }
    stream.close();
  }

  return cpu_jiffies;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key;
  int value;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return value;
      }
    }
    stream.close();
  }

  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key;
  int value;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return value;
      }
    }
    stream.close();
  }

  return 0;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmdline;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmdline);
    stream.close();
  }
  return cmdline;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string line;
  std::string key, value, kb;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      linestream >> key >> value >> kb;
      if (key == "VmSize") {
        return to_string(stol(value) / 1024);
      }
    }
    stream.close();
  }

  return string();
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid") {
        return value;
      }
    }
    stream.close();
  }

  return string();
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string name, x, uid;
  string uid_dest = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      linestream >> name >> x >> uid;
      if (uid == uid_dest) {
        return name;
      }
    }
    stream.close();
  }

  return string();
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long uptime_system = LinuxParser::UpTime();
  long uptime_process = 0, starttime;
  std::string line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // 使用流迭代器提取#22位置的数据(in clock ticks)
    std::istream_iterator<string> in_iter(linestream), eof;
    std::vector<string> single_word(in_iter, eof);
    // start from 0
    starttime = stol(single_word[21]);

    uptime_process = uptime_system - (starttime / sysconf(_SC_CLK_TCK));
    stream.close();
  }
  return uptime_process;
}