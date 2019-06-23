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
  static const int IDLE_TIME = 3;
  static const int IO_WAIT_TIME = 4;

  static auto getPathToPidStat(string pid);
  static auto getPathToPidStatus(string pid);
  static auto getStatsFromPid(string pid);
  static auto getPidTime(vector<string> v);
  static auto calculateTimeFromVector(vector<string> times);
  static auto parseUserListForUserName(string uuid);
  static auto getSysVersion();
  static auto getSysActiveCpuTime(vector<string> times);
  static auto getSysIdleCpuTime(vector<string> times);

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
  static auto getNumberOfCores();
  static auto getNumberOfRunningProcesses();
  static auto getOsName();
  static auto PrintCpuStats(vector<string> values1, vector<string> values2);
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
  auto total = float{0};
  for (auto s : time)
  {
    total += stof(s);
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

auto ProcessParser::getSysActiveCpuTime(vector<string> times)
{
  times.erase(times.begin() + IDLE_TIME);
  times.erase(times.begin() + IO_WAIT_TIME);
  return ProcessParser::calculateTimeFromVector(times);
};

auto ProcessParser::getSysIdleCpuTime(vector<string> times)
{
  auto idleTime = stof(times.at(IDLE_TIME));
  auto ioWaitTime = stof(times.at(IO_WAIT_TIME));
  return idleTime + ioWaitTime;
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
  auto stats = Util::getStream(Path::basePath() + Path::statPath());
  auto timeVector = Util::getVectorOfLineWithMatchingWord(stats, "cpu");
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
  auto stats = Util::getStream(ProcessParser::getPathToPidStatus(pid));
  auto uuidVector = Util::getVectorOfLineWithMatchingWord(stats, "Uid:");
  auto uuid = uuidVector.at(1);
  return ProcessParser::parseUserListForUserName(uuid);
}

auto ProcessParser::getTotalThreads()
{
  return thread::hardware_concurrency();
};

auto ProcessParser::getTotalNumberOfProcesses()
{
  auto stats = Util::getStream(Path::basePath() + Path::statPath());
  auto processVector = Util::getVectorOfLineWithMatchingWord(stats, "processes");
  return stoi(processVector.at(1));
};

auto ProcessParser::getNumberOfCores()
{
  auto cpuInfo = Util::getStream(Path::basePath() + Path::cpuInfoPath());
  auto coreInfoVec = Util::getVectorOfLineWithMatchingWord(cpuInfo, "cores", 1);
  return coreInfoVec.at(3);
}

auto ProcessParser::getNumberOfRunningProcesses()
{
  auto stats = Util::getStream(Path::basePath() + Path::statPath());
  auto processVector = Util::getVectorOfLineWithMatchingWord(stats, "procs_running");
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
  auto stats = Util::getStream((Path::basePath() + Path::statPath()));
  auto timeVector = Util::getVectorOfLineWithMatchingWord(stats, "cpu" + coreNumber);
  timeVector.erase(timeVector.begin());
  return timeVector;
}

auto ProcessParser::getSysRamPercent()
{
  auto path = Path::basePath() + Path::memInfoPath();
  auto contextWords = vector<string>{"MemFree:", "MemAvailable:", "Buffers:"};

  auto memFloats = vector<float>{};
  for (auto word : contextWords)
  {
    auto stats = Util::getStream(path);
    auto line = Util::getVectorOfLineWithMatchingWord(stats, word);
    memFloats.push_back(stof(line.at(1)));
  }

  auto num = 1 - (memFloats.at(0) / (memFloats.at(1) - memFloats.at(2)));
  return 100.0f * num;
}

auto ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2)
{
  auto activeTime = float{getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1)};
  auto idleTime = float{getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1)};
  auto totalTime = float{activeTime + idleTime};
  return to_string(float{100.0f * (activeTime / totalTime)});
};

#endif