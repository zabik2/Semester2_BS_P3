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
    int ProgrammCounter = 0;
    unsigned int timer = 0;
    int EA = 4;
    vector<int> IDvector;
    vector<string>FileNameVector;
    vector <unsigned int> StartTimeVector;
    vector <int> EndTime;
    vector <int> VerweilzeitVector;
    vector <long> CPUTimeUsed;
    vector <int> Akkumulator;


    int Blockdurchlaufen = 0;


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
        if(ProgrammCounter != -1 && Process::processes.size() <= ProgrammCounter){
            ProgrammCounter = 0;
        }
        if(simulateTicks <= 0){
            cout.clear();
            cin.clear();
            cout<<std::endl<<">> ";
            cout.clear();
            cin.clear();

            cout << "timer \tpid \tFileName \tPC \tAkku \tstart time \tBefehl" <<std::endl;

            simulateTicks = 1000;

        }
        if(!Process::processes.empty() && ProgrammCounter >= 0) {
            Process *running = Process::processes[ProgrammCounter];
            if (running && running->state != BLOCKED)
                std::cout << "\t" << timer << "\t" << running->getPid() << "\t" << running->FileName << " \t\t"  << running->CommandCounter
                          << " \t" << running->reg << " \t" << running->startTime << " \t\t"  << running->Befehl << std::endl;
        }
        else
            std::cout << "\t" << timer << "\t Blocked durch EA und keine andere Prozesse verfügbar" << std::endl;


            if (!blocked.empty()) {// EA Simulation
                Process *b = blocked.front();

                Blockdurchlaufen++;
                //timer - b->BlockedTime >= EA
                if (Blockdurchlaufen >= EA){
                    b->state = WAIT;
                    Blockdurchlaufen = 0;
                    if (ProgrammCounter >= 0)
                       Process::processes[ProgrammCounter]->state = WAIT;
                blocked.erase(blocked.begin());
            }
        }

        //Sim CPU
        if(!Process::processes.empty()) {
            try {
                if(ProgrammCounter >= 0) {
                    processExec(*Process::processes[ProgrammCounter]);

                    if (Process::processes[ProgrammCounter]->state == END) {
                        simulateTicks = 1;
                        IDvector.push_back(Process::processes[ProgrammCounter]->getPid());
                        FileNameVector.push_back(Process::processes[ProgrammCounter]->FileName);
                        StartTimeVector.push_back(Process::processes[ProgrammCounter]->startTime);
                        EndTime.push_back(timer);
                        VerweilzeitVector.push_back(timer - Process::processes[ProgrammCounter]->startTime + 1);
                        CPUTimeUsed.push_back(Process::processes[ProgrammCounter]->time);
                        Akkumulator.push_back(Process::processes[ProgrammCounter]->reg);
                        delete Process::processes[ProgrammCounter];
                        Process::processes.erase(Process::processes.begin() + ProgrammCounter);
                    } else if (Process::processes[ProgrammCounter]->state == BLOCKED) {
                        blocked.push_back(Process::processes[ProgrammCounter]);
                        //Process::processes.erase(Process::processes.begin() + ProgrammCounter);
                    } else if (Process::processes[ProgrammCounter]->state == WAIT ) {
                        countProc++;

                        ProgrammCounter = calc(ProgrammCounter);
                    }
                }
            } catch (...) {
                std::cout << std::endl << "Abort Process " << Process::processes[ProgrammCounter]->getPid() << std::endl;
                break;
            }
        }

        simulateTicks--;
        timer++;
        Process::TIME = timer;
        update(ProgrammCounter);
    }
    std::cout<<std::endl << "Shutting down..."<<std::endl;
    std::cout << "Durchschnittliche Takte per Prozess: " << EndTime[EndTime.size()-1]/countProc << std::endl;
    float VerweilzeitDurchschnitt = 0;
    for (int i = 0; i <= VerweilzeitVector.size()-1; i++)
        VerweilzeitDurchschnitt += VerweilzeitVector[i];

    std:cout << "Durchschnittliche Verweilzeit: " << VerweilzeitDurchschnitt/(float)VerweilzeitVector.size() << std::endl;
    return 0;
}