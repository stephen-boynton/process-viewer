#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "ProcessParser.h"

class Process
{
private:
  string pid;
  string user;
  string cmd;
  string cpu;
  string mem;
  string upTime;

public:
  Process(string pid)
  {
    this->pid = pid;
    this->user = ProcessParser::getProcUser(pid);
    this->mem = ProcessParser::getVmSize(pid);
    this->cmd = ProcessParser::getCmd(pid);
    this->upTime = ProcessParser::getProcUpTime(pid);
    this->cpu = ProcessParser::getCpuPercent(pid);
  }
  void setPid(int pid);
  auto getPid() const;
  auto getUser() const;
  auto getCmd() const;
  auto getCpu() const;
  auto getMem() const;
  auto getUpTime() const;
  auto getProcess();
};

void Process::setPid(int pid)
{
  this->pid = pid;
}
auto Process::getPid() const
{
  return this->pid;
}
auto Process::getProcess()
{
  this->mem = ProcessParser::getVmSize(this->pid);
  this->upTime = ProcessParser::getProcUpTime(this->pid);
  this->cpu = ProcessParser::getCpuPercent(this->pid);

  return (this->pid + "   " + this->user + "   " + this->mem.substr(0, 5) + "     " + this->cpu.substr(0, 5) + "     " + this->upTime.substr(0, 5) + "    " + this->cmd.substr(0, 30) + "...");
}

auto Process::getUser() const
{
  return this->user;
};

auto Process::getCmd() const
{
  return this->cmd;
};

auto Process::getCpu() const
{
  return this->cpu;
};

auto Process::getMem() const
{
  return this->mem;
};

auto Process::getUpTime() const
{
  return this->upTime;
};

#endif