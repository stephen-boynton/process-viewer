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

public:
  static string getCmd(string pid);
  static vector<string> getPidList();
  static string getVmSize(string pid);
  // static string getCpuPercent(string pid);
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
// implementation based on allocation of memory upfront example shown here:
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
string ProcessParser::convertIfstreamToString(ifstream i)
{
  string str;

  i.seekg(0, ios::end);
  str.reserve(i.tellg());
  i.seekg(0, ios::beg);

  str.assign((istreambuf_iterator<char>(i)),
             istreambuf_iterator<char>());
  return str;
}

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
  return ProcessParser::convertIfstreamToString(Util::getStream(Path::basePath() + Path::memInfoPath()));
};

#endif