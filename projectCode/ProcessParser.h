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

  static string getPathToPidStat(string pid);
  static string getPathToPidStatus(string pid);
  static vector<string> getStatsFromPid(string pid);
  static string getLineFromGlobalStats(int line);
  static string getLineFromPidStatus(string pid, int line);
  static vector<string> getPidTime(vector<string> v);
  static long int calculateTimeFromVector(vector<string> times);
  static string parseUserListForUserName(string uuid);
  static string getSysVersion();

public:
  static string getCmd(string pid);
  static vector<string> getPidList();
  static string getVmSize(string pid);
  static string getCpuPercent(string pid);
  static long int getSysUpTime();
  static long int getProcUpTime(string pid);
  static string getProcUser(string pid);
  // static vector<string> getSysCpuPercent(string coreNumber = "");
  // static float getSysRamPercent();
  // static string getSysKernelVersion();
  static int getTotalThreads();
  static int getTotalNumberOfProcesses();
  static int getNumberOfRunningProcesses();
  static string getOsName();
  // static string printCpuStats(vector<string> values1, vector<string> values2);
};
// ==================================
//  PRIVATE METHODS
// =================================

string ProcessParser::getPathToPidStat(string pid)
{
  return Path::basePath() + pid + "/" + Path::statPath();
}

string ProcessParser::getPathToPidStatus(string pid)
{
  return Path::basePath() + pid + Path::statusPath();
}

vector<string> ProcessParser::getStatsFromPid(string pid)
{
  ifstream stats = Util::getStream(ProcessParser::getPathToPidStat(pid));
  return Util::convertIfstreamToVector(stats);
}

string ProcessParser::getLineFromGlobalStats(int line)
{
  ifstream stats = Util::getStream(Path::basePath() + Path::statPath());
  return Util::getLineFromIfstream(stats, line);
};

string ProcessParser::getLineFromPidStatus(string pid, int line)
{
  ifstream stats = Util::getStream(ProcessParser::getPathToPidStatus(pid));
  return Util::getLineFromIfstream(stats, line);
};

vector<string> ProcessParser::getPidTime(vector<string> v)
{
  auto start = v.begin() + UTIME_LOCATION;
  auto end = v.begin() + CSTIME_LOCATION;
  vector<string> timeVector(start, end + 1);
  return timeVector;
}

long int ProcessParser::calculateTimeFromVector(vector<string> time)
{
  long int total = 0;
  for (auto s : time)
  {
    total += stoi(s);
  }
  return total;
}

string ProcessParser::parseUserListForUserName(string uuid)
{
  string line;
  ifstream userlistStream = Util::getStream(Path::etcPasswdPath());
  size_t found;

  vector<string> lines{};
  while (getline(userlistStream, line))
  {
    lines.push_back(line);
  }

  vector<vector<string>> userlist{};
  for (string l : lines)
  {
    string delimeter = ":";
    vector<string> user{};
    size_t pos = 0;
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
};

string ProcessParser::getSysVersion()
{
  ifstream version = Util::getStream(Path::basePath() + Path::versionPath());
  return Util::convertIfstreamToString(version);
}

// ==================================
//  PUBLIC METHODS
// =================================

string ProcessParser::getCmd(string pid)
{
  ifstream cmd = Util::getStream(Path::basePath() + pid + Path::cmdPath());
  return Util::convertIfstreamToString(cmd);
};

vector<string> ProcessParser::getPidList()
{
  const string pathToPids = Path::basePath();
  vector<string> files{};
  Util::getFilesFromDirectory(pathToPids, files);
  return Util::filterOutNonNumbers(files);
}

string ProcessParser::getVmSize(string pid)
{
  vector<string> stats = ProcessParser::getStatsFromPid(pid);
  return stats.at(VIRTUAL_MEM_SIZE_LOCATION);
};

string ProcessParser::getCpuPercent(string pid)
{
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
//   vector<string> values(beg, end); // done!
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

long int ProcessParser::getSysUpTime()
{
  string stats = ProcessParser::getLineFromGlobalStats(0);
  vector<string> timeVector = Util::convertStringToVector(stats);
  timeVector.erase(timeVector.begin());
  return ProcessParser::calculateTimeFromVector(timeVector);
};

long int ProcessParser::getProcUpTime(string pid)
{
  vector<string> stats = ProcessParser::getStatsFromPid(pid);
  vector<string> timeVector = ProcessParser::getPidTime(stats);
  return ProcessParser::calculateTimeFromVector(timeVector);
}

string ProcessParser::getProcUser(string pid)
{
  string uuidLine = ProcessParser::getLineFromPidStatus(pid, UUID_LOCATION);
  cout << uuidLine << endl;
  vector<string> uuidVector = Util::convertStringToVector(uuidLine);
  string uuid = uuidVector.at(1);
  return ProcessParser::parseUserListForUserName(uuid);
}

int ProcessParser::getTotalThreads()
{
  return thread::hardware_concurrency();
};

int ProcessParser::getTotalNumberOfProcesses()
{
  string processes = ProcessParser::getLineFromGlobalStats(8);
  vector<string> processVector = Util::convertStringToVector(processes);
  return stoi(processVector.at(1));
};

int ProcessParser::getNumberOfRunningProcesses()
{
  string running = ProcessParser::getLineFromGlobalStats(9);
  vector<string> processVector = Util::convertStringToVector(running);
  return stoi(processVector.at(1));
};

string ProcessParser::getOsName()
{
  return ProcessParser::getSysVersion();
}

#endif