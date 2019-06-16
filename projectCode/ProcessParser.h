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

  static string getPathToPidStat(string pid);
  static ifstream &getStatsFromPid(string pid);
  static ifstream &getGlobalStats();
  static vector<ifstream &> getGlobalAndPidStats(string pid);
  static vector<int> getPidTime(vector<string> v);
  static float calculateTimeFromVector(vector<string> times);

public:
  static string getCmd(string pid);
  static vector<string> getPidList();
  static string getVmSize(string pid);
  static string getCpuPercent(string pid);
  static long int getSysUpTime();
  static string getProcUpTime(string pid);
  // static string getProcUser(string pid);
  // static vector<string> getSysCpuPercent(string coreNumber = "");
  // static float getSysRamPercent();
  // static string getSysKernelVersion();
  // static int getTotalThreads();
  // static int getTotalNumberOfProcesses();
  // static int getNumberOfRunningProcesses();
  // static string getOsName();
  // static string printCpuStats(vector<string> values1, vector<string> values2);
};
// ==================================
//  PRIVATE METHODS
// =================================

string ProcessParser::getPathToPidStat(string pid)
{
  return Path::basePath() + pid + "/" + Path::statPath();
}

ifstream &ProcessParser::getStatsFromPid(string pid)
{
  return Util::getStream(ProcessParser::getPathToPidStat(pid));
}

ifstream &ProcessParser::getGlobalStats()
{
  return Util::getStream(Path::basePath() + Path::statPath());
};

vector<ifstream &> ProcessParser::getGlobalAndPidStats(string pid)
{
  vector<ifstream &> ifs{};
  ifs.push_back(ProcessParser::getStatsFromPid(pid));
  ifs.push_back(ProcessParser::getGlobalStats());
  return ifs;
}

vector<int> ProcessParser::getPidTime(vector<string> v)
{
  vector<int> timeVector{};
  auto start = v.begin() + UTIME_LOCATION;
  auto end = v.begin() + CSTIME_LOCATION;
  timeVector.push_back(stoi(v.at(UTIME_LOCATION)));
  timeVector.push_back(stoi(v.at(STIME_LOCATION)));

  vector<T>::const_iterator first = myVec.begin() + 100000;
  vector<T>::const_iterator last = myVec.begin() + 101000;
  vector<T> newVec(first, last);

  return timeVector;
}

float ProcessParser::calculateTimeFromVector(vector<string> time)
{
  float total = 0;
  for (auto s : time)
  {
    total += stof(s);
  }
  return total;
}

// ==================================
//  PUBLIC METHODS
// =================================

string ProcessParser::getCmd(string pid)
{
  return Util::convertIfstreamToString(Util::getStream(Path::basePath() + pid + Path::cmdPath()));
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
  vector<string> stats = Util::convertIfstreamToVector(ProcessParser::getStatsFromPid(pid));
  return stats.at(VIRTUAL_MEM_SIZE_LOCATION);
};

string ProcessParser::getCpuPercent(string pid)
{
  vector<ifstream &> pidAndGlobalStatsStream = ProcessParser::getGlobalAndPidStats(pid);
  const chrono::milliseconds TIME_DELAY_IN_MILLI(500);
  this_thread::sleep_for(TIME_DELAY_IN_MILLI);
  vector<ifstream &> pidAndGlobalStatsStream2 = ProcessParser::getGlobalAndPidStats(pid);

  vector<string> processStatsVector = Util::convertIfstreamToVector(pidAndGlobalStatsStream.at(0));
  vector<string> globalStatsVector = Util::convertStringToVector(Util::getLineFromIfstream(pidAndGlobalStatsStream.at(1), 0));

  vector<string> processStatsVector2 = Util::convertIfstreamToVector(pidAndGlobalStatsStream2.at(0));
  vector<string> globalStatsVector2 = Util::convertStringToVector(Util::getLineFromIfstream(pidAndGlobalStatsStream2.at(1), 0));

  vector<int> baseTime = ProcessParser::getPidTime(processStatsVector);
  vector<int> updatedTime = ProcessParser::getPidTime(processStatsVector2);

  globalStatsVector.erase(globalStatsVector.begin());
  globalStatsVector2.erase(globalStatsVector2.begin());
  int cpuTime = ProcessParser::calculateTimeFromVector(globalStatsVector);
  int cpuTime2 = ProcessParser::calculateTimeFromVector(globalStatsVector2);

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

static long int getSysUpTime(){

};

#endif