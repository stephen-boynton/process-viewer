#include <string>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>

using namespace std;

// Classic helper functions
class Util
{

public:
  static string convertToTime(long int input_seconds);
  static string getProgressBar(string percent);
  static ifstream getStream(string path);
  static void getFilesFromDirectory(string dir, vector<string> &files);
  static bool is_number(const std::string &s);
  static string getLineFromIfstream(ifstream &ifs, int lineNum);
  static vector<string> filterOutNonNumbers(vector<string> v);
  static string convertIfstreamToString(ifstream &i);
  static vector<string> convertStringToVector(string s);
  static vector<string> convertIfstreamToVector(ifstream &i);
};

string Util::convertToTime(long int input_seconds)
{
  long minutes = input_seconds / 60;
  long hours = minutes / 60;
  long seconds = int(input_seconds % 60);
  minutes = int(minutes % 60);
  string result = to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
  return result;
}

// constructing string for given percentage
// 50 bars is uniformly streched 0 - 100 %
// meaning: every 2% is one bar(|)
string Util::getProgressBar(string percent)
{
  string result = "0% ";
  int _size = 50;
  int boundaries = (stof(percent) / 100) * _size;

  for (int i = 0; i < _size; i++)
  {
    if (i <= boundaries)
    {
      result += "|";
    }
    else
    {
      result += " ";
    }
  }

  result += " " + percent.substr(0, 5) + " /100%";
  return result;
}

// wrapper for creating streams
ifstream Util::getStream(string path)
{
  ifstream stream(path);
  if (!stream)
  {
    throw runtime_error("Non - existing PID");
  }
  return stream;
}
// Based on this implementation:
// https://www.linuxquestions.org/questions/programming-9/c-list-files-in-directory-379323/
void Util::getFilesFromDirectory(string dir, vector<string> &files)
{
  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(dir.c_str())) == NULL)
  {
    cout << "Error(" << errno << ") opening " << dir << endl;
    throw runtime_error("Problem reading directory");
  }

  while ((dirp = readdir(dp)) != NULL)
  {
    files.push_back(string(dirp->d_name));
  }
  closedir(dp);
}
// from https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool Util::is_number(const std::string &s)
{
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it))
    ++it;
  return !s.empty() && it == s.end();
}

string Util::getLineFromIfstream(ifstream &ifs, int lineNum)
{
  string line;
  for (int i = 0; i <= lineNum; i++)
  {
    getline(ifs, line);
  }

  return line;
}

vector<string> Util::filterOutNonNumbers(vector<string> v)
{
  vector<string> onlyNumbers(v.size());
  auto it = copy_if(v.begin(), v.end(), onlyNumbers.begin(), [](string f) { return Util::is_number(f); });
  onlyNumbers.resize(distance(onlyNumbers.begin(), it));
  return onlyNumbers;
}
// implementation based on example shown here:
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
string Util::convertIfstreamToString(ifstream &i)
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
vector<string> Util::convertStringToVector(string s)
{
  stringstream ss(s);
  istream_iterator<string> begin(ss);
  istream_iterator<string> end;
  vector<string> vectorOfStrings(begin, end);
  return vectorOfStrings;
};

vector<string> Util::convertIfstreamToVector(ifstream &i)
{
  string str = Util::convertIfstreamToString(i);
  return Util::convertStringToVector(str);
}
