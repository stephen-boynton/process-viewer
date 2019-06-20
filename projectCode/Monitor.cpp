#include "Process.h"
#include "ProcessParser.h"

int main(int argc, char const *argv[])
{
  auto cmd = ProcessParser::getTotalNumberOfProcesses();
  cout << cmd << endl;
  return 0;
}
