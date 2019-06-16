#include "Process.h"
#include "ProcessParser.h"

int main(int argc, char const *argv[])
{
  string cmd = ProcessParser::getProcUser("1");
  cout << cmd << endl;
  return 0;
}
