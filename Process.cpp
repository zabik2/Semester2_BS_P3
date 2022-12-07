#include "Process.h"

int Process::NUM = 0;
unsigned int Process::TIME = -1;
std::vector<Process*> Process::processes;

Process::Process(int ppid, const std::string &file) : pid(++NUM), ppid(ppid), stream(file), startTime(TIME+1) {
    priority = 0;
    counter = 0;
    state = WAITING;
    time = 0;
}
Process::Process(const string &file): Process(-1, file){}

int Process::getPid() const {
    return pid;
}

void Process::execute() {
    if(!stream){
        std::cout << "Process "<<pid<<" program file not found!"<<std::endl;
        state = END;
        return;
    }
    std::string line;
    if (std::getline(stream, line)){
        int val;
        switch (toupper(line[0])) {
            case 'L':
                line.erase(0,2);
                reg = std::stoi(line);
                break;
            case 'A':
                line.erase(0,2);
                val = std::stoi(line);
                reg += val;
                break;
            case 'S':
                line.erase(0,2);
                val = std::stoi(line);
                reg -= val;
                break;
            case 'B':
                state = BLOCKED;
                counter = 0;
                break;
            case 'X': {
                line.erase(0, 2);
                int pos = line.find('\r');
                if(pos != string::npos){
                    line.erase(pos);
                }
                Process* process = new Process(pid, line);
                processes.push_back(process);
                state = WAIT;
                break;
            }
            case 'Z':
                state = END;
                stream.close();
                break;
        }
        time++;
        counter++;

    }
}


