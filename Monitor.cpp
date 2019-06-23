#include "Process.h"
#include "ProcessParser.h"

int main(int argc, char const *argv[])
{
  auto cmd = ProcessParser::getNumberOfCores();
  cout << cmd << endl;
  return 0;
}
