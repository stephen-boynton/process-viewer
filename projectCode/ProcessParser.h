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
  static vector<string> filterOutNonNumbers(vector<string> v);
  static string convertIfstreamToString(ifstream i);
  static vector<string> convertStringToVector(string s);
  static string getPathToPidStat(string pid);
  static vector<string> getVectorOfStatsFromPid(string pid);
  static vector<string> getLineFromGlobalStats(int lineNum);

public:
  static string getCmd(string pid);
  static vector<string> getPidList();
  static string getVmSize(string pid);
  static string getCpuPercent(string pid);
  // static long int getSysUpTime();
  // static string getProcUpTime(string pid);
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
vector<string> ProcessParser::filterOutNonNumbers(vector<string> v)
{
  vector<string> onlyNumbers(v.size());
  auto it = copy_if(v.begin(), v.end(), onlyNumbers.begin(), [](string f) { return Util::is_number(f); });
  onlyNumbers.resize(distance(onlyNumbers.begin(), it));
  return onlyNumbers;
}
// implementation based on example shown here:
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
string ProcessParser::convertIfstreamToString(ifstream i)
{
  string str;
  i.seekg(0, ios::end);
  i.seekg(0, ios::beg);

  str.assign((istreambuf_iterator<char>(i)),
             istreambuf_iterator<char>());

  return str;
}

// based on this implementation
// https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
vector<string> ProcessParser::convertStringToVector(string s)
{
  stringstream ss(s);
  istream_iterator<string> begin(ss);
  istream_iterator<string> end;
  vector<string> vectorOfStrings(begin, end);
  return vectorOfStrings;
};

string ProcessParser::getPathToPidStat(string pid)
{
  return Path::basePath() + pid + "/" + Path::statPath();
}

vector<string> ProcessParser::getVectorOfStatsFromPid(string pid)
{
  string pathToStat = ProcessParser::getPathToPidStat(pid);
  string statsString = ProcessParser::convertIfstreamToString(Util::getStream(pathToStat));
  return ProcessParser::convertStringToVector(statsString);
}

vector<string> ProcessParser::getLineFromGlobalStats(int lineNum)
{
  ifstream stats = Util::getStream(Path::basePath() + Path::statPath());
  string line;
  getline(stats, line);
  return ProcessParser::convertStringToVector(line);
};

// ==================================
//  PUBLIC METHODS
// =================================

string ProcessParser::getCmd(string pid)
{
  return ProcessParser::convertIfstreamToString(Util::getStream(Path::basePath() + pid + Path::cmdPath()));
};

vector<string> ProcessParser::getPidList()
{
  const string pathToPids = Path::basePath();
  vector<string> files{};
  Util::getFilesFromDirectory(pathToPids, files);
  return ProcessParser::filterOutNonNumbers(files);
}

string ProcessParser::getVmSize(string pid)
{
  vector<string> statsVector = ProcessParser::getVectorOfStatsFromPid(pid);
  return statsVector.at(22);
};

string ProcessParser::getCpuPercent(string pid)
{
  vector<string> processStatsVector = ProcessParser::getVectorOfStatsFromPid(pid);
  vector<string> globalStatsVector = ProcessParser::getLineFromGlobalStats();
  int total_cpu = 0;
  for (auto s : globalStatsVector)
  {
    if (s == "cpu")
    {
      continue;
    }
    cout << s << endl;
    total_cpu += stoi(s);
  }
  return "0";
};

#endif