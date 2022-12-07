#include <algorithm>
#include "Process.h"

using namespace std;

void processExec(Process &pr){
    pr.state = EXECUTE;
    pr.execute();
}

//Choose Algorithm
enum ALG {PARALLEL, FIRST, ROBIN};
ALG currentAlg = FIRST;

//Choose Quantum
const int QUANTUM = 5;

//Update PC every tick
void update(int &pc){
    if(currentAlg != PARALLEL && !Process::processes.empty()){

        if(currentAlg == ROBIN && pc >= 0){
            if(Process::processes[pc]->counter >= QUANTUM) {
                Process::processes[pc]->counter = 0;
                Process::processes[pc]->state = WAITING;
            }
        }

        //if current process waiting
        if(pc < 0 || Process::processes[pc]->state != EXECUTE){
            int count = 0;
            for(int i = pc; count <  Process::processes.size(); i++, count++) {
                if(i >= Process::processes.size()){
                    i = 0;
                }
                Process *p = Process::processes[i];
                if(p->state == WAITING && i != pc){
                    //found waiting process
                    pc = i;
                    p->state = EXECUTE;
                    return;
                }
            }
            //no process found continuing waiting one
            if(pc >= 0 && Process::processes[pc]->state == WAITING){
                Process::processes[pc]->state = EXECUTE;
            }else
                pc = -1;
        }
    }else if(currentAlg == PARALLEL){
        int count = 0;
        int i = pc + 1;
        if(i >= Process::processes.size()){
            i = 0;
        }
        //change to next
        for(; i <= Process::processes.size(); i++,count++) {
            if (i >= Process::processes.size()) {
                i = 0;
            }
            if (Process::processes[i]->state != BLOCKED) {
                pc = i;
                Process::processes[i]->state = EXECUTE;
                break;
            }
            if (count >= Process::processes.size()) {
                pc = -1;
                break;
            }
        }
    }
}

int calc(int pc){
    int oldPC = pc;
    if(currentAlg == FIRST){
        //find new process
        pc = -1;
        bool wset = false;
        bool curset = false;
        for(int i = Process::processes.size()-1; i >= 0; i--){
            Process* p = Process::processes[i];
            if(i == oldPC && !wset){
                p->state = WAITING;
                wset = true;
            }
            if(i != oldPC && !curset && p->state != BLOCKED){
                pc = i;
                p->state = EXECUTE;
                curset = true;
            }
            if(wset && curset){
                break;
            }
        }
    }else if(currentAlg == ROBIN){
        //continue old process
        Process::processes[pc]->state = EXECUTE;
    }
    return pc;
}

int main() {
    int pc = 0;
    unsigned int timer = 0;
    unsigned int simulateTicks = 0;
    int countTicks = 0;
    int countProc = 1;
    std::vector<Process*> blocked;
    Process* first = new Process("init");
    Process::processes.push_back(first);
    std::cout << "System starting."<<std::endl;

    while(true){
        if(Process::processes.empty() && blocked.empty()){
            cout << std::endl << "No Processes!"<<std::endl;
            break;
        }
        if(pc != -1 && Process::processes.size() <= pc){
            pc = 0;
        }
        if(simulateTicks <= 0){
            string input;
            cout.clear();
            cin.clear();
            cout<<std::endl<<">> ";
            getline(cin, input);
            cout.clear();
            cin.clear();

            std::transform(input.begin(), input.end(), input.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            if(input[0] == 's' || input == "step"){
                int pos = input.find(' ');
                simulateTicks = 1;
                if(pos != string::npos){
                    input.erase(0,pos+1);
                    if(input.length() > 0){
                        int arg = std::stoi(input);
                        simulateTicks = arg;
                        continue;
                    }
                }
            }else if(input[0] == 'u' || input == "unblock"){
                if(!blocked.empty()){
                    Process* b = blocked.front();
                    b->state = EXECUTE;
                    if(pc >= 0)
                        Process::processes[pc]->state = WAITING;
                    for(int i = 0; i < Process::processes.size(); i++) {
                        Process *p = Process::processes[i];
                        if(p->getPid() == b->getPid()){
                            pc = i;
                            break;
                        }
                    }
                    blocked.erase(blocked.begin());
                }
                continue;
                
            }else if(input[0] == 'd'|| input == "dump"){
                cout << "****************************************************************" <<std::endl;
                cout << "The current system state is as follows:" <<std::endl;
                cout << "****************************************************************" <<std::endl;
                cout << "CURRENT TIME: "<<timer <<std::endl;
                cout << "RUNNING PROCESS:" <<std::endl;
                cout << "pid \tppid \tpriority \tvalue \tstart time \tCPU time used so far" <<std::endl;
                if(!Process::processes.empty() && pc >= 0) {
                    Process *running = Process::processes[pc];
                    if (running && running->state != BLOCKED)
                        std::cout << running->getPid() << " \t" << running->ppid << " \t" << running->priority
                                  << " \t\t" << running->reg << " \t" << running->startTime << " \t\t" << running->time << std::endl;
                }
                cout << "BLOCKED PROCESS:" <<std::endl;
                cout << "pid \tppid \tpriority \tvalue \tstart time \tCPU time used so far" <<std::endl;
                for(Process* p : blocked){
                    if(p->state == BLOCKED)
                        std::cout << p->getPid()<<" \t"<< p->ppid<<" \t"<<p->priority<<" \t\t"<<p->reg<<" \t"<<p->startTime<<" \t\t"<<p->time <<std::endl;
                }
                std::cout << "PROCESS READY TO EXECUTE:" <<std::endl;
                std::cout << "pid \tppid \tpriority \tvalue \tstart time \tCPU time used so far" <<std::endl;
                for(int i = 0; i < Process::processes.size(); i++){
                    Process* p = Process::processes[i];
                    if(p->state == WAITING && i != pc)
                        std::cout << p->getPid()<<" \t"<< p->ppid<<" \t"<<p->priority<<" \t\t"<<p->reg<<" \t"<<p->startTime<<" \t\t"<<p->time <<std::endl;
                }

            }else if(input[0] == 'q' || input == "quit"){
                break;
            }
            continue;
        }

        //Sim CPU
        if(!Process::processes.empty()) {
            try {
                if(pc >= 0) {
                    countTicks++;
                    processExec(*Process::processes[pc]);
                    if (Process::processes[pc]->state == END) {
                        delete Process::processes[pc];
                        Process::processes.erase(Process::processes.begin() + pc);
                    } else if (Process::processes[pc]->state == BLOCKED) {
                        blocked.push_back(Process::processes[pc]);
                        //Process::processes.erase(Process::processes.begin() + pc);
                    } else if (Process::processes[pc]->state == WAITING && currentAlg != PARALLEL) {
                        countProc++;

                        pc = calc(pc);
                    }else if (currentAlg == PARALLEL) {
                        Process::processes[pc]->state = WAITING;
                    }
                }
            } catch (...) {
                std::cout << std::endl << "Abort Process " << Process::processes[pc]->getPid() << std::endl;
                break;
            }
        }

        simulateTicks--;
        timer++;
        Process::TIME = timer;
        update(pc);
    }
    std::cout<<std::endl << "Shutting down..."<<std::endl;
    std::cout << "Durchschnittliche Takte per Prozess: " << (countTicks / countProc) << std::endl;
    return 0;
}
