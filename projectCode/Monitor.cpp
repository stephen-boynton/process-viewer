#include "Process.h"
#include "ProcessParser.h"

int main(int argc, char const *argv[])
{
  string cmd = ProcessParser::getCpuPercent("1");
  cout << cmd << endl;
  return 0;
}
