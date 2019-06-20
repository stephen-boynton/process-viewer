#ifndef PROCESSPARSER_H
#define PROCESSPARSER_H

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"

using namespace std;

class ProcessParser
{
private:
  static const int VIRTUAL_MEM_SIZE_LOCATION = 22;
  static const int UTIME_LOCATION = 13;
  static const int CSTIME_LOCATION = 16;
  static const int UUID_LOCATION = 8;

  static auto getPathToPidStat(string pid);
  static auto getPathToPidStatus(string pid);
  static auto getStatsFromPid(string pid);
  static auto getLineFromGlobalStats(int line);
  static auto getLineFromPidStatus(string pid, int line);
  static auto getPidTime(vector<string> v);
  static auto calculateTimeFromVector(vector<string> times);
  static auto parseUserListForUserName(string uuid);
  static auto getSysVersion();
  static auto getlineVectorFromStatusByInitalWord(string word);

public:
  static auto getCmd(string pid);
  static auto getPidList();
  static auto getVmSize(string pid);
  static auto getCpuPercent(string pid);
  static auto getSysUpTime();
  static auto getProcUpTime(string pid);
  static auto getProcUser(string pid);
  static auto getSysCpuPercent(string coreNumber = "");
  static auto getSysRamPercent();
  static auto getSysKernelVersion();
  static auto getTotalThreads();
  static auto getTotalNumberOfProcesses();
  static auto getNumberOfRunningProcesses();
  static auto getOsName();
  static auto printCpuStats(vector<string> values1, vector<string> values2);
};

// ======================================================================================================
//  PRIVATE METHODS
// =====================================================================================================

auto ProcessParser::getPathToPidStat(string pid)
{
  return Path::basePath() + pid + "/" + Path::statPath();
}

auto ProcessParser::getPathToPidStatus(string pid)
{
  return Path::basePath() + pid + Path::statusPath();
}

auto ProcessParser::getStatsFromPid(string pid)
{
  auto stats = Util::getStream(ProcessParser::getPathToPidStat(pid));
  return Util::convertIfstreamToVector(stats);
}

auto ProcessParser::getLineFromGlobalStats(int line)
{
  //TODO: update to get line by searchvalue aka starting string, like they do in the test
  auto stats = Util::getStream(Path::basePath() + Path::statPath());
  return Util::getLineFromIfstream(stats, line);
};

auto ProcessParser::getLineFromPidStatus(string pid, int line)
{
  //TODO: is this also relative to each cp
  auto stats = Util::getStream(ProcessParser::getPathToPidStatus(pid));
  return Util::getLineFromIfstream(stats, line);
};

auto ProcessParser::getPidTime(vector<string> v)
{
  auto start = v.begin() + UTIME_LOCATION;
  auto end = v.begin() + CSTIME_LOCATION;
  auto timeVector = vector<string>(start, end + 1);
  timeVector.push_back(v.at(21));
  return timeVector;
}

auto ProcessParser::calculateTimeFromVector(vector<string> time)
{
  auto total = 0;
  for (auto s : time)
  {
    total += stoi(s);
  }
  return total;
}

auto ProcessParser::parseUserListForUserName(string uuid)
{
  auto line = string{};
  auto userlistStream = Util::getStream(Path::etcPasswdPath());
  auto found = size_t{};

  auto lines = vector<string>{};
  while (getline(userlistStream, line))
  {
    lines.push_back(line);
  }

  auto userlist = vector<vector<string>>{};
  for (auto l : lines)
  {
    auto delimeter = ":";
    auto user = vector<string>{};
    auto pos = size_t{0};
    while ((pos = l.find(delimeter)) != string::npos)
    {
      auto sub = l.substr(0, pos);
      user.emplace_back(sub);
      l.erase(0, pos + 1);
    }
    userlist.emplace_back(user);
  }

  for (auto usr : userlist)
  {
    if (usr.at(2) == uuid)
    {
      return usr.at(0);
    }
  }
  throw runtime_error("No user found for this process");
};

auto ProcessParser::getSysVersion()
{
  auto path = Path::basePath() + Path::versionPath();
  auto version = Util::getStream(path);
  return Util::convertIfstreamToVector(version);
}

auto ProcessParser::getlineVectorFromStatusByInitalWord(string word)
{
  auto stats = Util::getStream(Path::basePath() + Path::statPath());
  auto line = string{};
  auto lines = vector<vector<string>>{};
  while (getline(stats, line))
    lines.push_back(Util::convertStringToVector(line));

  for (auto l : lines)
    if (l.at(0) == word)
      return l;

  throw runtime_error("Unable to find line using provided word in global stats");
};

// ===================================================================================================
//  PUBLIC METHODS
// ===================================================================================================

auto ProcessParser::getCmd(string pid)
{
  auto cmd = Util::getStream(Path::basePath() + pid + Path::cmdPath());
  return Util::convertIfstreamToString(cmd);
};

auto ProcessParser::getPidList()
{
  auto const pathToPids = Path::basePath();
  auto files = vector<string>{};
  Util::getFilesFromDirectory(pathToPids, files);
  return Util::filterOutNonNumbers(files);
}

auto ProcessParser::getSysUpTime()
{
  auto stats = ProcessParser::getLineFromGlobalStats(0);
  auto timeVector = Util::convertStringToVector(stats);
  timeVector.erase(timeVector.begin());
  return ProcessParser::calculateTimeFromVector(timeVector);
};

auto ProcessParser::getVmSize(string pid)
{
  auto stats = ProcessParser::getStatsFromPid(pid);
  auto vmSizeInBytes = stof(stats.at(VIRTUAL_MEM_SIZE_LOCATION));
  return to_string((vmSizeInBytes / 1e9));
};

auto ProcessParser::getCpuPercent(string pid)
{
  auto stats = ProcessParser::getStatsFromPid(pid);
  auto procTimes = ProcessParser::getPidTime(stats);

  auto startTime = stof(procTimes.back());
  procTimes.pop_back();

  auto totalTime = ProcessParser::calculateTimeFromVector(procTimes);
  auto uptime = ProcessParser::getSysUpTime();
  // attempting to only have explicit conversions, not implicit
  auto freq = static_cast<float>(sysconf(_SC_CLK_TCK));
  auto seconds = static_cast<float>(uptime - (startTime / freq));
  auto result = static_cast<float>(100.0 * ((totalTime / freq) / seconds));
  return to_string(result);
};

auto ProcessParser::getProcUpTime(string pid)
{
  auto stats = ProcessParser::getStatsFromPid(pid);
  auto timeVector = ProcessParser::getPidTime(stats);
  return timeVector.at(0);
}

auto ProcessParser::getProcUser(string pid)
{
  auto uuidLine = ProcessParser::getLineFromPidStatus(pid, UUID_LOCATION);
  auto uuidVector = Util::convertStringToVector(uuidLine);
  auto uuid = uuidVector.at(1);
  return ProcessParser::parseUserListForUserName(uuid);
}

auto ProcessParser::getTotalThreads()
{
  return thread::hardware_concurrency();
};

auto ProcessParser::getTotalNumberOfProcesses()
{
  auto processVector = ProcessParser::getlineVectorFromStatusByInitalWord("processes");
  return stoi(processVector.at(1));
};

auto ProcessParser::getNumberOfRunningProcesses()
{
  auto running = ProcessParser::getLineFromGlobalStats(9);
  auto processVector = Util::convertStringToVector(running);
  return stoi(processVector.at(1));
};

auto ProcessParser::getOsName()
{
  auto versionInfo = ProcessParser::getSysVersion();
  return versionInfo.at(0);
}

auto ProcessParser::getSysKernelVersion()
{
  auto versionInfo = ProcessParser::getSysVersion();
  return versionInfo.at(2);
}

auto ProcessParser::getSysCpuPercent(string coreNumber)
{
  // TODO:
  //   // It is possible to use this method for selection of data for overall cpu or every core.
  //   // when nothing is passed "cpu" line is read
  //   // when, for example "0" is passed  -> "cpu0" -> data for first core is read
  //   string line;
  //   string name = "cpu" + coreNumber;
  //   ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  //   while (std::getline(stream, line))
  //   {
  //     if (line.compare(0, name.size(), name) == 0)
  //     {
  //       istringstream buf(line);
  //       istream_iterator<string> beg(buf), end;
  //       vector<string> values(beg, end);
  //       // set of cpu data active and idle times;
  //       return values;
  //     }
  //   }
  //   return (vector<string>());
  // }

  // float get_sys_active_cpu_time(vector<string> values)
  // {
  //   return (stof(values[S_USER]) +
  //           stof(values[S_NICE]) +
  //           stof(values[S_SYSTEM]) +
  //           stof(values[S_IRQ]) +
  //           stof(values[S_SOFTIRQ]) +
  //           stof(values[S_STEAL]) +
  //           stof(values[S_GUEST]) +
  //           stof(values[S_GUEST_NICE]));
  // }

  // float get_sys_idle_cpu_time(vector<string> values)
  // {
  //   return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
  // }
}

auto ProcessParser::getSysRamPercent()
{
  // TODO:
  // float ProcessParser::getSysRamPercent()
  // {
  //   string line;
  //   string name1 = "MemAvailable:";
  //   string name2 = "MemFree:";
  //   string name3 = "Buffers:";

  //   string value;
  //   int result;
  //   ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
  //   float total_mem = 0;
  //   float free_mem = 0;
  //   float buffers = 0;
  //   while (std::getline(stream, line))
  //   {
  //     if (total_mem != 0 && free_mem != 0)
  //       break;
  //     if (line.compare(0, name1.size(), name1) == 0)
  //     {
  //       istringstream buf(line);
  //       istream_iterator<string> beg(buf), end;
  //       vector<string> values(beg, end);
  //       total_mem = stof(values[1]);
  //     }
  //     if (line.compare(0, name2.size(), name2) == 0)
  //     {
  //       istringstream buf(line);
  //       istream_iterator<string> beg(buf), end;
  //       vector<string> values(beg, end);
  //       free_mem = stof(values[1]);
  //     }
  //     if (line.compare(0, name3.size(), name3) == 0)
  //     {
  //       istringstream buf(line);
  //       istream_iterator<string> beg(buf), end;
  //       vector<string> values(beg, end);
  //       buffers = stof(values[1]);
  //     }
  //   }
  //   //calculating usage:
  //   return float(100.0 * (1 - (free_mem / (total_mem - buffers))));
  // }
}

auto ProcessParser::printCpuStats(vector<string> values1, vector<string> values2){
    //     string ProcessParser::printCpuStats(vector<string> values1, vector<string> values2){
    //         /*
    // Because CPU stats can be calculated only if you take measures in two different time,
    // this function has two parameters: two vectors of relevant values.
    // We use a formula to calculate overall activity of processor.
    // */
    //         float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    // float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    // float totalTime = activeTime + idleTime;
    // float result = 100.0 * (activeTime / totalTime);
    // return to_string(result);
    // }
};

#endif