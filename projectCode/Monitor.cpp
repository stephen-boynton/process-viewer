#include "Process.h"

int main(int argc, char const *argv[])
{
  /* code */
  Process p(1, "user", "command", 6.99, 17);
  std::cout << p.GetCMD() << std::endl;
  return 0;
}
