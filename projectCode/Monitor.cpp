#include "Process.h"
#include "ProcessParser.h"

int main(int argc, char const *argv[])
{
  auto cmd = ProcessParser::getSysRamPercent();
  cout << cmd << endl;
  return 0;
}
