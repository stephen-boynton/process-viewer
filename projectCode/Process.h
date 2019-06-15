#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>

using std::string;

class Process
{
private:
  int PID;
  string user;
  string CMD;
  float CPU;
  int upTime;

public:
  Process(int _pid, string _user, string _cmd, float _cpu, int _uptime) : PID(_pid), user(_user), CMD(_cmd), CPU(_cpu), upTime(_uptime){};
  int GetPID() const { return PID; };
  string GetUser() const { return user; };
  string GetCMD() const { return CMD; };
  float GetCPU() const { return CPU; };
  int GetUpTime() const { return upTime; };

  void SetPID(int _pid) { PID = _pid; };
  void SetUser(string _user) { user = _user; };
  void SetCMD(string _cmd) { CMD = _cmd; };
  void SetCPU(float _cpu) { CPU = _cpu; };
  void SetUpTime(int _uptime) { upTime = _uptime; };
};

#endif