//MaryEileen Fagan
//Scheduler Lab 2
//Operating Systems

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <iomanip>
#include <typeinfo>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

vector<int> randvals;

int randomOS(int u, string s, int& r) {
    r++;
    if (r > randvals.size()){
        r= 0;
    }
    return (1 + (randvals[r] % u));
}

typedef struct process{
    int arrival_time, total_cpu_time, cpu_max, io_max, blocked_time, cpu_time, bt, original_cpu_total, io_time, wait_time, finishing_time;
    float ratio;
    float turnaround_time;
    //blocked, running, ready, unstarted, terminated
    string state;
} process;

process make_process(int& at, int& cpum, int& tcpu, int& iomax, string& s, int& b, int& t, int &bt){
    process newprocess;
    newprocess.arrival_time = at;
    newprocess.total_cpu_time = tcpu;
    newprocess.cpu_max = cpum;
    newprocess.io_max = iomax;
    newprocess.state = s;
    newprocess.blocked_time=b;
    newprocess.cpu_time = t;
    newprocess.bt = bt;
    newprocess.original_cpu_total = tcpu;
    newprocess.io_time = 0;
    newprocess.wait_time = 0;
    newprocess.finishing_time=0;
    newprocess.turnaround_time = 0.0;
    newprocess.ratio =0;
    return newprocess;
}

void print_verbose(vector<process> processes_array, int cycle){
    if (cycle>9)
        cout << "Before cycle   " << cycle << ": ";
    else
        cout << "Before cycle   " << cycle << ": ";
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state=="unstarted"){
            cout << " unstarted 0";
        }
        if (processes_array[i].state=="ready"){
            cout << " ready 0";
        }
    
        if (processes_array[i].state=="running"){
            cout << " running " << processes_array[i].cpu_time;
        }
        if (processes_array[i].state=="blocked"){
            cout << " blocked " << processes_array[i].blocked_time;
        }
        if (processes_array[i].state=="terminated"){
            cout << " terminated 0";
        }
    }
            cout << endl;
}

vector<process> sort_by_arrival(vector<process> processes_array){
    for (int i=0; i<processes_array.size()-1; i++){
        for (int j=i+1; j<processes_array.size(); j++){
            if (processes_array[i].arrival_time>processes_array[j].arrival_time){
                process temp = processes_array[j];
                processes_array.erase(processes_array.begin() + j);
                processes_array.insert(processes_array.begin()+i, temp);
            }
        }
    }
    return processes_array;
}


vector<process> do_blocked_processes(vector<process> processes_array, bool running, int& total_io_time, int& r){
    int index;
    bool blocked = false;
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state=="blocked" && processes_array[i].blocked_time>0){
            processes_array[i].blocked_time--;
            blocked = true;
        }
        if (processes_array[i].state=="blocked" && processes_array[i].blocked_time==0){
            processes_array[i].state="ready";
        }
        
    }
    if (blocked == true) total_io_time = total_io_time + 1;
    return processes_array;
}



vector<process> do_running_processes(vector<process> processes_array, bool& running, int cycle, int& r){
    for (int i=0; i<processes_array.size(); i++){
        if ((processes_array[i].state=="running") && processes_array[i].cpu_time>0){
            processes_array[i].total_cpu_time--;
            processes_array[i].cpu_time--;
            running = true;
        }
        
        if (processes_array[i].total_cpu_time==0 && processes_array[i].state=="running"){
            running = false;
            processes_array[i].state = "terminated";
            processes_array[i].finishing_time = cycle;
            processes_array[i].bt = 50000;
        }
        
        if ((processes_array[i].state=="running") && processes_array[i].cpu_time==0){
            running =false;
            processes_array[i].state = "blocked";
            processes_array[i].blocked_time = randomOS(processes_array[i].io_max, "IOBURST", r);
            processes_array[i].io_time =processes_array[i].io_time+processes_array[i].blocked_time;
            processes_array[i].bt = processes_array[i].blocked_time + cycle + 1;
        }
        
    }
    return processes_array;
}

vector<process> do_rr_running_processes(vector<process> processes_array, bool& running, int cycle, int& quantum, int& r){
    for (int i=0; i<processes_array.size(); i++){
        if ((processes_array[i].state=="running") && processes_array[i].cpu_time>0){
            processes_array[i].total_cpu_time--;
            processes_array[i].cpu_time--;
            quantum--;
            running = true;
        }
        
        if (processes_array[i].total_cpu_time==0 && processes_array[i].state=="running"){
            running = false;
            processes_array[i].state = "terminated";
            processes_array[i].finishing_time = cycle;
            processes_array[i].bt = 50000;
        }
        
        if ((processes_array[i].state=="running") && processes_array[i].cpu_time==0){
            running =false;
            processes_array[i].state = "blocked";
            processes_array[i].blocked_time = randomOS(processes_array[i].io_max, "IOBURST", r);
            processes_array[i].io_time =processes_array[i].io_time+processes_array[i].blocked_time;
            processes_array[i].bt = processes_array[i].blocked_time + cycle +1;
            
            
        }
        
        if (processes_array[i].state == "running" && quantum==0 && processes_array[i].cpu_time>0){
            running = false;
            processes_array[i].state = "ready";
            int max = 0;
            for (int i=0; i<processes_array.size()-1; i++){
                if (processes_array[max].state != "ready"){
                    max = max +1;
                    if (max>processes_array.size()-1) max =0;
                }
            }
            for (int j=0; j<processes_array.size(); j++){
                if ((processes_array[j].bt>processes_array[max].bt)&& processes_array[j].state=="ready"){
                    max = j;
                }
            }
            processes_array[i].bt = cycle + 1;
        }
        
    }
    return processes_array;
}

vector<process> do_arriving_processes(vector<process> processes_array, bool running, int cycle, int& r){
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state=="unstarted" && processes_array[i].arrival_time==cycle){
            processes_array[i].state ="ready";
        }
    }
    return processes_array;
}

vector<process> do_ready_processes(vector<process> processes_array, bool& running, int& k, int& p, int& r){
    int min = 0;
    for (int i=0; i<processes_array.size(); i++){
    
        if (processes_array[min].bt>processes_array[i].bt){
            min =i;
        }
    }
    for (int i=0; i<processes_array.size(); i++){
        if ((processes_array[min].bt==processes_array[i].bt) && i<min){
            min =i;
        }
    }
    p = min;
    
    if (running==false){
        for (int i=0; i<processes_array.size(); i++){
            if (running==false && processes_array[p].state=="ready"){
                running = true;
                processes_array[p].state="running";
                processes_array[p].cpu_time = randomOS(processes_array[p].cpu_max, "CPU BURST", r);
                if (processes_array[p].cpu_time>processes_array[p].total_cpu_time)
                    processes_array[p].cpu_time=processes_array[p].total_cpu_time;
                k=p;
                processes_array[p].bt=0;
            }
            else{
                p =p+1;
                if (p>processes_array.size()-1){
                    p=0;
                }
            }
        }
    }
    p=k;
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state == "ready"){
            processes_array[i].wait_time = processes_array[i].wait_time + 1;
        }
    }
    
    return processes_array;
}

vector<process> do_rr_ready_processes(vector<process> processes_array, bool& running, int& k, int& p, int& quantum, int& r){
    int min = p +1;
    if (min>processes_array.size()-1) min = 0;
    int j = min+1;
    if (j>processes_array.size()-1) j =0;
    for (int i=0; i<processes_array.size()-1; i++){
        if (processes_array[min].bt>processes_array[j].bt){
            min =j;
        }
        j++;
        if (j>processes_array.size()-1) j =0;
    }
    for (int i=0; i<processes_array.size(); i++){
        if ((processes_array[min].bt==processes_array[i].bt) && i<min){
            min =i;
        }
    }

    p = min;
    
    if (running==false){
        for (int i=0; i<processes_array.size(); i++){
            if (running==false && processes_array[p].state=="ready"){
                running = true;
                quantum = 2;
                processes_array[p].state="running";
                if (processes_array[p].cpu_time==0){
                    processes_array[p].cpu_time = randomOS(processes_array[p].cpu_max, "CPU BURST", r);
                    if (processes_array[p].cpu_time>processes_array[p].total_cpu_time)
                        processes_array[p].cpu_time=processes_array[p].total_cpu_time;
                }
                k=p;
                processes_array[p].bt=0;
            }
            else{
                p =p+1;
                if (p>processes_array.size()-1){
                    p=0;
                }
            }
        }
    }
    p=k;
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state == "ready"){
            processes_array[i].wait_time = processes_array[i].wait_time + 1;
        }
    }
    
    return processes_array;
}


vector<process> do_last_ready_processes(vector<process> processes_array, bool& running, int& k, int& p, int cycle, int& r){
    int max = 0;
    if (running ==false){
        for (int i=0; i<processes_array.size()-1; i++){
            if(processes_array[max].state == "blocked"){
                max = max +1;
                if (max>processes_array.size()-1) max =0;
            }
        }
    for (int i=0; i<processes_array.size(); i++){
        if ((processes_array[max].bt<processes_array[i].bt) && !(processes_array[i].bt>cycle+1)){
            max =i;
        }
    }
    for (int i=0; i<processes_array.size(); i++){
        if ((processes_array[max].bt==processes_array[i].bt) && i<max){
            max =i;
        }
    }
    p = max;
    }
    
    if (running==false){
        for (int i=0; i<processes_array.size(); i++){
            if (running==false && processes_array[p].state=="ready"){
                running = true;
                processes_array[p].state="running";
                processes_array[p].cpu_time = randomOS(processes_array[p].cpu_max, "CPU BURST", r);
                if (processes_array[p].cpu_time>processes_array[p].total_cpu_time)
                    processes_array[p].cpu_time=processes_array[p].total_cpu_time;
                k=p;
                processes_array[p].bt=0;
            }
            else{
                p =p+1;
                if (p>processes_array.size()-1){
                    p=0;
                }
            }
        }
    }
    p=k;
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state == "ready"){
            processes_array[i].wait_time = processes_array[i].wait_time + 1;
        }
    }
    
    return processes_array;
}

vector<process> do_hprn_ready_processes(vector<process> processes_array, bool& running, int& k, int& p, int cycle, int& r){
    for (int i=0; i<processes_array.size(); i++){
        float small_t = processes_array[i].original_cpu_total-processes_array[i].total_cpu_time;
        if (1>small_t) small_t = 1;
        processes_array[i].ratio = (float)(cycle - processes_array[i].arrival_time)/small_t;
    }
    
    //if (running==false){
        int max = 0;
    for (int i=0; i<processes_array.size()-1; i++){
        if (processes_array[max].state!="ready"){
            max++;
            if (max>processes_array.size()-1) max =0;
        }
    }
        //while (processes_array[max].state == "blocked")
            //max = max +1;
        for (int i=0; i<processes_array.size(); i++){
            if ((processes_array[max].ratio<processes_array[i].ratio) && !(processes_array[i].bt>cycle+1) && processes_array[i].state=="ready"){
                max =i;
            }
        }
        for (int i=0; i<processes_array.size(); i++){
            if ((processes_array[max].ratio==processes_array[i].ratio) && i<max){
                max =i;
            }
        }
        p = max;

    if (running==false){
        for (int i=0; i<processes_array.size(); i++){
            if (running==false && processes_array[p].state=="ready"){
                running = true;
                processes_array[p].state="running";
                processes_array[p].cpu_time = randomOS(processes_array[p].cpu_max, "CPU BURST", r);
                if (processes_array[p].cpu_time>processes_array[p].total_cpu_time)
                    processes_array[p].cpu_time=processes_array[p].total_cpu_time;
                k=p;
                processes_array[p].bt=0;
            }
            else{
                p =p+1;
                if (p>processes_array.size()-1){
                    p=0;
                }
            }
        }
    }
    p=k;
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state == "ready"){
            processes_array[i].wait_time = processes_array[i].wait_time + 1;
        }
    }
    
    return processes_array;
}



bool all_terminated(vector<process> processes_array){
    for (int i=0; i<processes_array.size(); i++){
        if (processes_array[i].state!="terminated"){
            return false;
        }
    }
    return true;
}


void fcfs_scheduler(vector<process> processes_array, int& numProc, bool& verbose){
    bool running=false;
    int cycle =0, k=0, p=0, r=-1;
    int total_io_time =0;
    processes_array = sort_by_arrival(processes_array);
    cout << "The (sorted) input is:  " << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl << endl;
    
    if (verbose==true)
        cout << "This detailed printout gives the state and remaining burst for each process" << endl << endl;
    
    while (!all_terminated(processes_array)){
        
        if (verbose==true)
            print_verbose(processes_array, cycle);
        
            
        processes_array = do_blocked_processes(processes_array, running, total_io_time, r);
        
        processes_array = do_running_processes(processes_array, running, cycle, r);
        
        processes_array = do_arriving_processes(processes_array, running, cycle, r);
     
        processes_array = do_ready_processes(processes_array, running, k, p, r);
        
        cycle++;
       
    }
    float cpu_util=0.00, io_util=0.00, avg_waittime=0.00, avg_turnaround=0.00;
    for (int i=0; i<processes_array.size(); i++){
        cpu_util = cpu_util + processes_array[i].original_cpu_total;
        avg_waittime = avg_waittime + processes_array[i].wait_time;
        io_util = io_util + processes_array[i].io_time;
        processes_array[i].turnaround_time = (float)processes_array[i].finishing_time - (float)processes_array[i].arrival_time;
        avg_turnaround = avg_turnaround + (float)processes_array[i].turnaround_time;
        
    }
 
    cpu_util = cpu_util/(float)(cycle-1);
    io_util = total_io_time/(float)(cycle-1);
    avg_waittime = avg_waittime/(float)(processes_array.size());
    float throughput = (100.00/(cycle-1))*processes_array.size();
    avg_turnaround = (float)avg_turnaround/(float)(processes_array.size());
    cout << "The scheduling algorithm used was First Come First Served" << endl << endl;
    for (int i=0; i<processes_array.size(); i++){
        cout << "Process " << i << ":" << endl;
        cout << "       (A,B,C,IO) = (" << processes_array[i].arrival_time << "," << processes_array[i].cpu_max << "," << processes_array[i].original_cpu_total << "," << processes_array[i].io_max << ")" << endl;
        cout << "       Finishing time: " << processes_array[i].finishing_time << endl;
        cout << "       Turnaround time: " << processes_array[i].finishing_time - processes_array[i].arrival_time << endl;
        cout << "       I/O time: " << processes_array[i].io_time << endl;
        cout << "       Waiting time: " << processes_array[i].wait_time << endl << endl;
        
    }

    cout << "Summary Data:" << endl;
    cout << "       Finishing time: " << cycle -1 << endl;
    cout << "       CPU Utilization: " << fixed << setprecision(6) << cpu_util << endl;
    cout << "       I/O Utilization: " << fixed << setprecision(6) << io_util << endl;
    cout << "       Throughput: " << fixed << setprecision(6) << throughput << " processes per hundred cycles" << endl;
    cout << "       Average turnaround time: " << fixed << setprecision(6) << avg_turnaround << endl;
    cout << "       Average waiting time: " << fixed << setprecision(6) << avg_waittime << endl;
}

void lcfs_scheduler(vector<process> processes_array, int& numProc, bool& verbose){
    int r =-1;
    bool running=false;
    int cycle =0, k=0, p=0;
    int total_io_time =0;
    processes_array = sort_by_arrival(processes_array);
    cout << "The (sorted) input is:  " << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl << endl;
    
    if (verbose==true)
        cout << "This detailed printout gives the state and remaining burst for each process" << endl << endl;
    
    while (!all_terminated(processes_array)){
        
        if (verbose==true)
            print_verbose(processes_array, cycle);
        
        
        processes_array = do_blocked_processes(processes_array, running, total_io_time, r);
        
        processes_array = do_running_processes(processes_array, running, cycle, r);
        
        processes_array = do_arriving_processes(processes_array, running, cycle, r);
        
        processes_array = do_last_ready_processes(processes_array, running, k, p, cycle, r);
        
        cycle++;
        
    }
    float cpu_util=0.00, io_util=0.00, avg_waittime=0.00, avg_turnaround=0.00;
    for (int i=0; i<processes_array.size(); i++){
        cpu_util = cpu_util + processes_array[i].original_cpu_total;
        avg_waittime = avg_waittime + processes_array[i].wait_time;
        io_util = io_util + processes_array[i].io_time;
        processes_array[i].turnaround_time = (float)processes_array[i].finishing_time - (float)processes_array[i].arrival_time;
        avg_turnaround = avg_turnaround + (float)processes_array[i].turnaround_time;
        
    }
    
    cpu_util = cpu_util/(float)(cycle-1);
    io_util = total_io_time/(float)(cycle-1);
    avg_waittime = avg_waittime/(float)(processes_array.size());
    float throughput = (100.00/(cycle-1))*processes_array.size();
    avg_turnaround = (float)avg_turnaround/(float)(processes_array.size());
    cout << "The scheduling algorithm used was Last Come First Served" << endl << endl;
    for (int i=0; i<processes_array.size(); i++){
        cout << "Process " << i << ":" << endl;
        cout << "       (A,B,C,IO) = (" << processes_array[i].arrival_time << "," << processes_array[i].cpu_max << "," << processes_array[i].original_cpu_total << "," << processes_array[i].io_max << ")" << endl;
        cout << "       Finishing time: " << processes_array[i].finishing_time << endl;
        cout << "       Turnaround time: " << processes_array[i].finishing_time - processes_array[i].arrival_time << endl;
        cout << "       I/O time: " << processes_array[i].io_time << endl;
        cout << "       Waiting time: " << processes_array[i].wait_time << endl << endl;
        
    }
    
    cout << "Summary Data:" << endl;
    cout << "       Finishing time: " << cycle -1 << endl;
    cout << "       CPU Utilization: " << fixed << setprecision(6) << cpu_util << endl;
    cout << "       I/O Utilization: " << fixed << setprecision(6) << io_util << endl;
    cout << "       Throughput: " << fixed << setprecision(6) << throughput << " processes per hundred cycles" << endl;
    cout << "       Average turnaround time: " << fixed << setprecision(6) << avg_turnaround << endl;
    cout << "       Average waiting time: " << fixed << setprecision(6) << avg_waittime << endl;
}


void hprn_scheduler(vector<process> processes_array, int& numProc, bool& verbose){
    int r =-1;
    bool running=false;
    int cycle =0, k=0, p=0;
    int total_io_time =0;
    processes_array = sort_by_arrival(processes_array);
    cout << "The (sorted) input is:  " << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl << endl;
    
    if (verbose==true)
        cout << "This detailed printout gives the state and remaining burst for each process" << endl << endl;
    
    while (!all_terminated(processes_array)){
        
        if (verbose==true)
            print_verbose(processes_array, cycle);
        
        
        processes_array = do_blocked_processes(processes_array, running, total_io_time, r);
        
        processes_array = do_running_processes(processes_array, running, cycle, r);
        
        processes_array = do_arriving_processes(processes_array, running, cycle, r);
        
        processes_array = do_hprn_ready_processes(processes_array, running, k, p, cycle, r);
        
        cycle++;
        
    }
    float cpu_util=0.00, io_util=0.00, avg_waittime=0.00, avg_turnaround=0.00;
    for (int i=0; i<processes_array.size(); i++){
        cpu_util = cpu_util + processes_array[i].original_cpu_total;
        avg_waittime = avg_waittime + processes_array[i].wait_time;
        io_util = io_util + processes_array[i].io_time;
        processes_array[i].turnaround_time = (float)processes_array[i].finishing_time - (float)processes_array[i].arrival_time;
        avg_turnaround = avg_turnaround + (float)processes_array[i].turnaround_time;
        
    }
    
    cpu_util = cpu_util/(float)(cycle-1);
    io_util = total_io_time/(float)(cycle-1);
    avg_waittime = avg_waittime/(float)(processes_array.size());
    float throughput = (100.00/(cycle-1))*processes_array.size();
    avg_turnaround = (float)avg_turnaround/(float)(processes_array.size());
    cout << "The scheduling algorithm used was HPRN" << endl << endl;
    for (int i=0; i<processes_array.size(); i++){
        cout << "Process " << i << ":" << endl;
        cout << "       (A,B,C,IO) = (" << processes_array[i].arrival_time << "," << processes_array[i].cpu_max << "," << processes_array[i].original_cpu_total << "," << processes_array[i].io_max << ")" << endl;
        cout << "       Finishing time: " << processes_array[i].finishing_time << endl;
        cout << "       Turnaround time: " << processes_array[i].finishing_time - processes_array[i].arrival_time << endl;
        cout << "       I/O time: " << processes_array[i].io_time << endl;
        cout << "       Waiting time: " << processes_array[i].wait_time << endl << endl;
        
    }
    
    cout << "Summary Data:" << endl;
    cout << "       Finishing time: " << cycle -1 << endl;
    cout << "       CPU Utilization: " << fixed << setprecision(6) << cpu_util << endl;
    cout << "       I/O Utilization: " << fixed << setprecision(6) << io_util << endl;
    cout << "       Throughput: " << fixed << setprecision(6) << throughput << " processes per hundred cycles" << endl;
    cout << "       Average turnaround time: " << fixed << setprecision(6) << avg_turnaround << endl;
    cout << "       Average waiting time: " << fixed << setprecision(6) << avg_waittime << endl;
}

void rr_scheduler(vector<process> processes_array, int& numProc, bool& verbose){
    int r =-1;
    int quantum = 2;
    bool running=false;
    int cycle =0, k=0, p=-1;
    int total_io_time =0;
    processes_array = sort_by_arrival(processes_array);
    cout << "The (sorted) input is:  " << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl << endl;
    
    if (verbose==true)
        cout << "This detailed printout gives the state and remaining burst for each process" << endl << endl;
    
    while (!all_terminated(processes_array)){
        
        if (verbose==true)
            print_verbose(processes_array, cycle);
        
        
        processes_array = do_blocked_processes(processes_array, running, total_io_time, r);
        
        processes_array = do_rr_running_processes(processes_array, running, cycle, quantum, r);
        
        processes_array = do_arriving_processes(processes_array, running, cycle, r);
        
        processes_array = do_rr_ready_processes(processes_array, running, k, p, quantum, r);
        
        cycle++;
        
    }
    float cpu_util=0.00, io_util=0.00, avg_waittime=0.00, avg_turnaround=0.00;
    for (int i=0; i<processes_array.size(); i++){
        cpu_util = cpu_util + processes_array[i].original_cpu_total;
        avg_waittime = avg_waittime + processes_array[i].wait_time;
        io_util = io_util + processes_array[i].io_time;
        processes_array[i].turnaround_time = (float)processes_array[i].finishing_time - (float)processes_array[i].arrival_time;
        avg_turnaround = avg_turnaround + (float)processes_array[i].turnaround_time;
        
    }
    
    cpu_util = cpu_util/(float)(cycle-1);
    io_util = total_io_time/(float)(cycle-1);
    avg_waittime = avg_waittime/(float)(processes_array.size());
    float throughput = (100.00/(cycle-1))*processes_array.size();
    avg_turnaround = (float)avg_turnaround/(float)(processes_array.size());
    cout << "The scheduling algorithm used was First Come First Served" << endl << endl;
    for (int i=0; i<processes_array.size(); i++){
        cout << "Process " << i << ":" << endl;
        cout << "       (A,B,C,IO) = (" << processes_array[i].arrival_time << "," << processes_array[i].cpu_max << "," << processes_array[i].original_cpu_total << "," << processes_array[i].io_max << ")" << endl;
        cout << "       Finishing time: " << processes_array[i].finishing_time << endl;
        cout << "       Turnaround time: " << processes_array[i].finishing_time - processes_array[i].arrival_time << endl;
        cout << "       I/O time: " << processes_array[i].io_time << endl;
        cout << "       Waiting time: " << processes_array[i].wait_time << endl << endl;
        
    }
    
    cout << "Summary Data:" << endl;
    cout << "       Finishing time: " << cycle -1 << endl;
    cout << "       CPU Utilization: " << fixed << setprecision(6) << cpu_util << endl;
    cout << "       I/O Utilization: " << fixed << setprecision(6) << io_util << endl;
    cout << "       Throughput: " << fixed << setprecision(6) << throughput << " processes per hundred cycles" << endl;
    cout << "       Average turnaround time: " << fixed << setprecision(6) << avg_turnaround << endl;
    cout << "       Average waiting time: " << fixed << setprecision(6) << avg_waittime << endl;
}




int main(int argc, char *argv[]){
    bool verbose = false;
    int cycle = 0, numProc=0, at, tcpu, cpum, iomax, b, t, bt, i=1;
    string s;
    ifstream ifile, rfile;
    string line;
    if (string(argv[i])=="--verbose"){
        verbose = true;
        i++;
    }
    ifile.open(argv[i]);
    ifile>>numProc;
    vector <process> processes_array;
    for (int i=numProc; i>0; i--){
        ifile >> at;
        ifile >> cpum;
        ifile >> tcpu;
        ifile >> iomax;
        s="unstarted";
        b=0;
        t=0;
        bt =at + 1;
        processes_array.push_back(make_process(at, cpum, tcpu, iomax, s, b, t, bt));
    }
    rfile.open("random-numbers");
    while(std::getline(rfile, line))
    {
        const char * c = line.c_str();
        int num = atoi(c);
        randvals.push_back(num);
    }
    rfile.close();
    
   
    cout << "================= FCFS ==============================" << endl;
    cout << "The original input was: ";
    cout << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
    }
    cout << endl;
    fcfs_scheduler(processes_array, numProc, verbose);
   
    cout << endl <<endl << "================= RR ==============================" << endl;
    cout << "The original input was: ";
    cout << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl;
    rr_scheduler(processes_array, numProc, verbose);
    

    
    cout << endl <<endl << "================= LCFS ==============================" << endl;
    cout << "The original input was: ";
    cout << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl;
    lcfs_scheduler(processes_array, numProc, verbose);
  
    cout << endl <<endl << "================= HPRN ==============================" << endl;
    cout << "The original input was: ";
    cout << numProc << " ";
    for (int i=0; i<processes_array.size(); i++){
        cout << processes_array[i].arrival_time << " " << processes_array[i].cpu_max << " " << processes_array[i].total_cpu_time << " " << processes_array[i].io_max << "   ";
        
    }
    cout << endl;
    hprn_scheduler(processes_array, numProc, verbose);

    return 0;
}
