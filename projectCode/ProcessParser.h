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

public:
  static auto getCmd(string pid);
  static auto getPidList();
  static auto getVmSize(string pid);
  static auto getCpuPercent(string pid);
  static auto getSysUpTime();
  static auto getProcUpTime(string pid);
  static auto getProcUser(string pid);
  // static auto getSysCpuPercent(string coreNumber = "");
  // static float getSysRamPercent();
  static auto getSysKernelVersion();
  static auto getTotalThreads();
  static auto getTotalNumberOfProcesses();
  static auto getNumberOfRunningProcesses();
  static auto getOsName();
  // static string printCpuStats(auto values1, auto values2);
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
  auto stats = Util::getStream(Path::basePath() + Path::statPath());
  return Util::getLineFromIfstream(stats, line);
};

auto ProcessParser::getLineFromPidStatus(string pid, int line)
{
  auto stats = Util::getStream(ProcessParser::getPathToPidStatus(pid));
  return Util::getLineFromIfstream(stats, line);
};

auto ProcessParser::getPidTime(vector<string> v)
{
  auto start = v.begin() + UTIME_LOCATION;
  auto end = v.begin() + CSTIME_LOCATION;
  auto timeVector = vector<string>(start, end + 1);
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

  vector<vector<string>> userlist{};
  for (auto l : lines)
  {
    auto delimeter = string{":"};
    auto user = vector<string>{};
    auto pos = size_t{0};
    while ((pos = l.find(delimeter)) != string::npos)
    {
      string sub = l.substr(0, pos);
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
  auto pathToPids = string{Path::basePath()};
  auto files = vector<string>{};
  Util::getFilesFromDirectory(pathToPids, files);
  return Util::filterOutNonNumbers(files);
}

auto ProcessParser::getVmSize(string pid)
{
  auto stats = ProcessParser::getStatsFromPid(pid);
  return stats.at(VIRTUAL_MEM_SIZE_LOCATION);
};

auto ProcessParser::getCpuPercent(string pid)
{
  auto stats = vector<string>{ProcessParser::getStatsFromPid(pid)};
  auto procTimes = vector<string>{ProcessParser::getPidTime(stats)};
  return "0";
};

// string ProcessParser::getCpuPercent(string pid)
// {
//   string line;
//   string value;
//   float result;
//   ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
//   getline(stream, line);
//   string str = line;
//   istringstream buf(str);
//   istream_iterator<string> beg(buf), end;
//   auto values(beg, end); // done!
//   // acquiring relevant times for calculation of active occupation of CPU for selected process
//   float utime = stof(ProcessParser::getProcUpTime(pid));
//   float stime = stof(values[14]);
//   float cutime = stof(values[15]);
//   float cstime = stof(values[16]);
//   float starttime = stof(values[21]);
//   float uptime = ProcessParser::getSysUpTime();
//   float freq = sysconf(_SC_CLK_TCK);
//   float total_time = utime + stime + cutime + cstime;
//   float seconds = uptime - (starttime / freq);
//   result = 100.0 * ((total_time / freq) / seconds);
//   return to_string(result);
// }

auto ProcessParser::getSysUpTime()
{
  auto stats = ProcessParser::getLineFromGlobalStats(0);
  auto timeVector = Util::convertStringToVector(stats);
  timeVector.erase(timeVector.begin());
  return ProcessParser::calculateTimeFromVector(timeVector);
};

auto ProcessParser::getProcUpTime(string pid)
{
  auto stats = ProcessParser::getStatsFromPid(pid);
  auto timeVector = ProcessParser::getPidTime(stats);
  return ProcessParser::calculateTimeFromVector(timeVector);
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
  auto processes = ProcessParser::getLineFromGlobalStats(8);
  auto processVector = Util::convertStringToVector(processes);
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
  auto versionInfo = vector<string>{ProcessParser::getSysVersion()};
  return versionInfo.at(0);
}

auto ProcessParser::getSysKernelVersion()
{
  auto versionInfo = vector<string>{ProcessParser::getSysVersion()};
  return versionInfo.at(2);
}

#endif