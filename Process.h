
#ifndef SIMPROCESS_PROCESS_H
#define SIMPROCESS_PROCESS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

enum STATE {EXECUTE, WAIT, BLOCKED, END};

class Process {

private:
    int pid;

public:
    Process(const std::string &file);
    Process(int ppid, const std::string &file);


    int ppid;
    int counter;
    long time;
    std::ifstream stream;
    int priority;
    const unsigned int startTime;
    int reg;
    STATE state;


    int getPid() const;
    void execute();

    static std::vector<Process*> processes;
    static unsigned int TIME;
    static int NUM;
};


#endif //SIMPROCESS_PROCESS_H
