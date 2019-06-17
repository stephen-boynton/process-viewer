#include "Process.h"
#include "ProcessParser.h"

int main(int argc, char const *argv[])
{
  string cmd = ProcessParser::getSysKernelVersion();
  cout << cmd << endl;
  return 0;
}
