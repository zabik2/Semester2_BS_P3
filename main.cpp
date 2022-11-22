#include <algorithm>
#include "Process.h"

using namespace std;


void processExec(Process &pr){
    pr.state = EXECUTE;
    pr.execute();
}

//Choose Algorithm
enum ALG {FIRST, QUANTUMACTIVE};        //switch between Quantum used, and non-Quantum variant
ALG currentAlg = FIRST;

//Choose Quantum
const int QUANTUM = 2;

//Update PC every tick
void update(int &pc){
    if(!Process::processes.empty()){
        // Block if Quantum reached
        if(currentAlg == QUANTUMACTIVE && pc >= 0){
            if(Process::processes[pc]->counter >= QUANTUM) {
                Process::processes[pc]->counter = 0;
                Process::processes[pc]->state = WAIT;
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
                if(p->state == WAIT && i != pc){
                    //found waiting process
                    pc = i;
                    p->state = EXECUTE;
                    return;
                }
            }
            //no process found continuing waiting one
            if(pc >= 0 && Process::processes[pc]->state == WAIT){
                Process::processes[pc]->state = EXECUTE;
            }else
                pc = -1;
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
                p->state = WAIT;
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
    }else if(currentAlg == QUANTUMACTIVE){
        //continue old process
        Process::processes[pc]->state = EXECUTE;
    }
    return pc;
}

int main() {
    int pc = 0;
    unsigned int timer = 0;
    int EA = 4;
    vector<int> IDvector;
    vector<string>FileNameVector;
    vector <unsigned int> StartTimeVector;
    vector <int> EndTime;
    vector <int> VerweilzeitVector;
    vector <long> CPUTimeUsed;
    vector <int> Akkumulator;

    unsigned int simulateTicks = 0;
    float countProc = 1;
    std::vector<Process*> blocked;
    Process* first = new Process("init");   // launch from init
    Process::processes.push_back(first);
    std::cout << "System starting."<<std::endl;


    while(true){

        if(Process::processes.empty() && blocked.empty()){
            cout << std::endl << "No Processes!"<<std::endl;
            cout << "pid \tFile Name \tStart time \tEnd time \tCPU Time used \t Verweilzeit \tAkkumulator" <<std::endl;
            for (int i = 0; i <= IDvector.size()-1; i++){
                cout << IDvector[i] << "\t" << FileNameVector[i] << " \t\t" << StartTimeVector[i] << "\t\t" << EndTime[i] << "\t\t" << CPUTimeUsed[i] << "\t\t "
                << VerweilzeitVector[i] <<" \t\t" << Akkumulator[i] << endl;
            }
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
            cout << "CURRENT TIME: "<<timer <<std::endl;
            cout << "RUNNING PROCESS:" <<std::endl;
            cout << "timer \tpid \tFileName \tPC \tAkku \tstart time \tCPU time used so far" <<std::endl;

            if(input[0] == 's' || input == "step"){
                simulateTicks = 1;
                continue;
            }else if(input[0] == 'u' || input == "unblock"){    // Vorzeitiges unblock
                if(!blocked.empty()){
                    Process* b = blocked.front();
                    b->state = EXECUTE;
                    if(pc >= 0)
                        Process::processes[pc]->state = WAIT;
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
                cout << "pid \tppid \tPC \tAkku \tstart time \tCPU time used so far" <<std::endl;
                if(!Process::processes.empty() && pc >= 0) {
                    Process *running = Process::processes[pc];
                    if (running && running->state != BLOCKED)
                        std::cout << running->getPid() << " \t" << running->ppid << " \t" << running->CommandCounter
                                  << " \t\t" << running->reg << " \t" << running->startTime << " \t\t" << running->time << std::endl;
                }
                cout << "BLOCKED PROCESS:" <<std::endl;
                cout << "pid \tppid \tPC \tAkku \tstart time \tCPU time used so far" <<std::endl;
                for(Process* p : blocked){
                    if(p->state == BLOCKED)
                        std::cout << p->getPid() << " \t" << p->ppid << " \t" << p->CommandCounter << " \t\t" << p->reg << " \t" << p->startTime << " \t\t" << p->time << std::endl;
                }
                std::cout << "PROCESS READY TO EXECUTE:" <<std::endl;
                std::cout << "pid \tppid \tPC \tAkku \tstart time \tCPU time used so far" <<std::endl;
                for(int i = 0; i < Process::processes.size(); i++){
                    Process* p = Process::processes[i];
                    if(p->state == WAIT && i != pc)
                        std::cout << p->getPid() << " \t" << p->ppid << " \t" << p->CommandCounter << " \t\t" << p->reg << " \t" << p->startTime << " \t\t" << p->time << std::endl;
                }


            }else if(input[0] == 'r' || input == "run"){
                simulateTicks = 100;

                continue;
            }else if(input[0] == 'q' || input == "quit"){
                break;
            }
            continue;
        }


            if (!blocked.empty()) {// EA Simulation
                Process *b = blocked.front();
                if (timer - b->BlockedTime >= EA){
                b->state = EXECUTE;
                if (pc >= 0)
                    Process::processes[pc]->state = WAIT;
                for (int i = 0; i < Process::processes.size(); i++) {
                    Process *p = Process::processes[i];
                    if (p->getPid() == b->getPid()) {
                        pc = i;
                        break;
                    }
                }
                blocked.erase(blocked.begin());
            }
        }


        if(!Process::processes.empty() && pc >= 0) {
            Process *running = Process::processes[pc];
            if (running && running->state != BLOCKED)
                std::cout << timer << "\t" << running->getPid() << "\t" << running->FileName << " \t\t"  << running->CommandCounter
                          << " \t" << running->reg << " \t" << running->startTime << " \t\t" << running->time << std::endl;
        }
        else
            std::cout << timer << "\t Blocked durch EA und keine andere Prozesse verfügbar" << std::endl;
        //Sim CPU
        if(!Process::processes.empty()) {
            try {
                if(pc >= 0) {
                    processExec(*Process::processes[pc]);

                    if (Process::processes[pc]->state == END) {
                        simulateTicks = 1;
                        IDvector.push_back(Process::processes[pc]->getPid());
                        FileNameVector.push_back(Process::processes[pc]->FileName);
                        StartTimeVector.push_back(Process::processes[pc]->startTime);
                        EndTime.push_back(timer);
                        VerweilzeitVector.push_back(timer-Process::processes[pc]->startTime+1);
                        CPUTimeUsed.push_back(Process::processes[pc]->time);
                        Akkumulator.push_back(Process::processes[pc]->reg);
                        delete Process::processes[pc];
                        Process::processes.erase(Process::processes.begin() + pc);
                    } else if (Process::processes[pc]->state == BLOCKED) {
                        blocked.push_back(Process::processes[pc]);
                        //Process::processes.erase(Process::processes.begin() + pc);
                    } else if (Process::processes[pc]->state == WAIT ) {
                        countProc++;

                        pc = calc(pc);
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
    std::cout << "Durchschnittliche Takte per Prozess: " << EndTime[EndTime.size()-1]/countProc << std::endl;
    float VerweilzeitDurchschnitt = 0;
    for (int i = 0; i <= VerweilzeitVector.size()-1; i++)
        VerweilzeitDurchschnitt += VerweilzeitVector[i];

    std:cout << "Durchschnittliche Verweilzeit: " << VerweilzeitDurchschnitt/(float)VerweilzeitVector.size() << std::endl;
    return 0;
}