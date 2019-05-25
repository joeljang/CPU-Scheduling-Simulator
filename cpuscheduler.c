#include <stdio.h>
#include <time.h>

#define PROCESS_NUM 50
#define ALGORITHM_NUM 6
#define TIME 1000
#define TQ 10

#define FCFS 0
#define SJF 1
#define PRIORITY 2 
#define RR 3

#define MAX_CPUBURST 30
#define MAX_IOBURST 5
#define MAX_ARRIVAL 100
#define MAX_PRIORITY 30

#define T 1
#define F 0

//Process
typedef struct myProcess* processPointer;

typedef struct myProcess {
    int pid;
    int CPUburst;
    int IOburst;
    int arrivalTime;
    int priority;
    int CPURemainTime;
    int IORemainTime;
    int waitingTime;
    int turnaroundTime;
}myProcess;

int cur_proc_num_JQ=0;
int cur_proc_num_RQ=0;
int cur_proc_num_WQ=0;
int cur_proc_num_T=0;

int timeConsumed = 0;
int Computation_start = 0;
int Computation_end = 0;
int Computation_idle = 0;


//Evaluation
typedef struct evaluation* evaluationPointer;
		
typedef struct evaluation {
    int alg;
    int preemptive;
    int startTime;
    int endTime;
    int avg_waitingTime;
    int avg_turnaroundTime;
    double CPU_util;
    int completed;
}evaluation;

evaluationPointer eval[ALGORITHM_NUM];

int cur_eval_num = 0;


//CONFIGURATION
    //Job Queue
processPointer jobQueue[PROCESS_NUM];
    //Clone Job Queue
processPointer cJobQueue[PROCESS_NUM];

    //Ready Queue
processPointer readyQueue[PROCESS_NUM];

    //Waiting Queue
processPointer waitingQueue[PROCESS_NUM];

    //Terminated Queue
processPointer terminatedQueue[PROCESS_NUM];

    //Currently Running Process
processPointer runningProcess=NULL;

//Initiazation
void init_eval() {
    for (int i=0;i<ALGORITHM_NUM;i++) {
        eval[i] = NULL;
    }
}

void init_JQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        jobQueue[i] = NULL;
    }
}

void init_CJQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        cJobQueue[i] = NULL;
    }
}

void init_RQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        readyQueue[i] = NULL;
    }
}

void init_WQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        waitingQueue[i] = NULL;
    }
}

void init_TQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        terminatedQueue[i] = NULL;
    }
}

//Clearing of Memory
void clear_eval() {
    for (int i=0;i<ALGORITHM_NUM;i++) {
        free(eval[i]);
        eval[i]=NULL;
    }
}

void clear_JQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        free(jobQueue[i]);
        jobQueue[i] = NULL;
    }
}

void clear_CJQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        free(cJobQueue[i]);
        cJobQueue[i] = NULL;
    }
}

void clear_RQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        free(readyQueue[i]);
        readyQueue[i] = NULL;
    }
}

void clear_WQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        free(waitingQueue[i]);
        waitingQueue[i] = NULL;
    }
}

void clear_TQ() {
    for (int i=0;i<PROCESS_NUM;i++) {
        free(terminatedQueue[i]);
        terminatedQueue[i] = NULL;
    }
}
    int CPURemainTime;
    int IORemainTime;
    int waitingTime;
    int turnaroundTime;

// PROCESS CREATION
processPointer createProcess(int pid, int CPUburst, int IOburst, int arrivalTime, int priority, int CPURemainTime, int IORemainTime, int waitingTime, int turnaroundTime) {
    processPointer newProcess = (processPointer)malloc(sizeof(myProcess));
    newProcess->pid = pid;
    newProcess->CPUburst = CPUburst;
    newProcess->IOburst = IOburst;
    newProcess->arrivalTime = arrivalTime;
    newProcess->priority = priority;
    newProcess->CPURemainTime = CPUburst;
    newProcess->IORemainTime = IOburst;
    newProcess->waitingTime = 0;
    newProcess->turnaroundTime = 0;
    return newProcess;
}

void createProcesses() {
    srand(time(NULL));
    int cpu;
    int io;
    int arrival;
    int priority;
    int i;
    int randomio;
	for(i=0;i<PROCESS_NUM; i++) {
		//CPU burst : 1~20
		//IO burst : 1~10
		cpu=rand()%MAX_CPUBURST + 1;
        randomio=rand()%10;
        io=0;
		arrival = rand()%MAX_ARRIVAL;
		priority = rand()%MAX_PRIORITY;
        if(randomio==0) {
            io=rand()%MAX_IOBURST + 1;
            cpu=0;
        }
		jobQueue[i] = createProcess(i+1, cpu, io, arrival, priority,0,0,0,0);
	}
	//sorting using insertion sort 
	processPointer temp;
	for (int i=0;i<PROCESS_NUM;i++) {
	    for (int j=0;j<PROCESS_NUM;j++) {
	        if (jobQueue[j]->arrivalTime > jobQueue[i]->arrivalTime) {
	            temp = jobQueue[i];
	            jobQueue[i] = jobQueue[j];
	            jobQueue[j] = temp;
	        }
	    }
	}
	
	cur_proc_num_JQ = PROCESS_NUM;
}
//Operations on Queues
    //CLONE QUEUE FOR SIMULATION
void createClone_JQ() {
    init_CJQ();
    for (int i=0;i<PROCESS_NUM;i++) {
        cJobQueue[i] = createProcess(jobQueue[i]->pid, jobQueue[i]->CPUburst, jobQueue[i]->IOburst, jobQueue[i]->arrivalTime, jobQueue[i]->priority, jobQueue[i]->CPURemainTime, jobQueue[i]->IORemainTime, jobQueue[i]->waitingTime, jobQueue[i]->turnaroundTime);
    }
    cur_proc_num_JQ = PROCESS_NUM;
}
    //Remove from JQ
processPointer removeFrom_CJQ(processPointer proc) {
    //printf("WHAT ARE WE TRYING TO REMOVE FROM CJQ? -> pid: %d\n", proc->pid);
    int temp = -1;
    if(cur_proc_num_JQ>0) {
        for (int i=0;i<cur_proc_num_JQ;i++) {
            if (cJobQueue[i]->pid == proc->pid) {
                temp = i;
                //printf("THERE IS A MATCH! -> %d\n", cJobQueue[temp]->pid);
            }
        }
        if (temp == -1) {
            printf("<error> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } else {
            processPointer removed = cJobQueue[temp];
            //printf("WE ARE GOING TO REMOVE THIS FROM CJQ-> pid: %d\n", removed->pid);
            for (int i=temp;i<cur_proc_num_JQ - 1; i++) {
                cJobQueue[i] = cJobQueue[i+1];
            }
            cJobQueue[cur_proc_num_JQ - 1] = NULL;
            cur_proc_num_JQ--;
            
            return removed;
        }
    } else {
        printf("<error> Clone Job Queue is EMPTY!!");
        return NULL;
    }
}
    //Insert into JQ
void insertInto_RQ(processPointer proc) {
    readyQueue[cur_proc_num_RQ++] = proc;
}

processPointer removeFrom_RQ (processPointer proc) {
    int temp = -1;
    if(cur_proc_num_RQ>0) {
        for (int i=0;i<cur_proc_num_RQ;i++) {
            if (readyQueue[i]->pid == proc->pid) {
                temp = i;
            }
        }
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } else {
            processPointer removed = readyQueue[temp];
            for(int i = temp; i < cur_proc_num_RQ - 1; i++) {
                readyQueue[i] = readyQueue[i+1];  
            }   
            readyQueue[cur_proc_num_RQ - 1] = NULL;
            cur_proc_num_RQ--;
            return removed;
        }
        
    } else {
        printf("<error> Ready Queue is empty");
        return NULL;
    }
}

void insertInto_WQ (processPointer proc) {
    int temp = -1;
    if(cur_proc_num_WQ<PROCESS_NUM) {
        for (int i=0;i<cur_proc_num_WQ;i++) {
            if (waitingQueue[i]->pid == proc->pid) {
                temp = i;
            }
        }
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Waiting Queue\n", proc->pid);
            return;  
        }
        waitingQueue[cur_proc_num_WQ++] = proc;
    } else {
        printf("<error> Waiting Queue is full");
        return;
    }
    //print_WQ();
}

processPointer removeFrom_WQ (processPointer proc) {
    int temp = -1;
    if(cur_proc_num_WQ>0) {
        for (int i=0;i<cur_proc_num_WQ;i++) {
            if (waitingQueue[i]->pid == proc->pid) {
                temp = i;
            }
        }
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } else {
            processPointer removed = waitingQueue[temp];
            for(int i = temp; i < cur_proc_num_WQ - 1; i++) {
                waitingQueue[i] = waitingQueue[i+1];
            }   
            waitingQueue[cur_proc_num_WQ - 1] = NULL;
            cur_proc_num_WQ--;
            return removed;
        }
        
    } else {
        puts("<ERROR> Waiting Queue is empty");
        return NULL;
    }
}

void insertInto_T (processPointer proc) {
    if(cur_proc_num_T<PROCESS_NUM) {
        terminatedQueue[cur_proc_num_T++] = proc;
    }
    else {
        puts("<error> Cannot terminate the process");
        return;
    }
}

// ALGORITHMS

processPointer FCFS_a() {
    if (cur_proc_num_RQ==0) {
        if (runningProcess==NULL) {
            return NULL;
        } else {
            return runningProcess;
        }
    } else {
        processPointer earliestProc = readyQueue[0]; //Looking for the process that arrived the earliest.
        if(runningProcess != NULL) { //Since it is not preemptive, let the runnig Process continue until finished.  
				return runningProcess;
        	} else {
				return removeFrom_RQ(earliestProc);
			}
    }
}

processPointer SJF_a(int preemptive) {
    processPointer shortestJob = readyQueue[0];
	if(shortestJob != NULL) { // 
		int i;
        for(i = 0; i < cur_proc_num_RQ; i++) {
            if (readyQueue[i]->CPURemainTime <= shortestJob->CPURemainTime) { 
                if(readyQueue[i]->CPURemainTime == shortestJob->CPURemainTime) { //If they have the same remaining CPU time, the process that arrived first is executed
                    if (readyQueue[i]->arrivalTime < shortestJob->arrivalTime) {
                        shortestJob = readyQueue[i];
                    }
                } else {
                    shortestJob = readyQueue[i];
                }
            }
        }
		
		if(runningProcess != NULL) { //If there is a process that is running already
				if(preemptive){ 
					if(runningProcess->CPURemainTime >= shortestJob->CPURemainTime) {
						if(runningProcess->CPURemainTime == shortestJob->CPURemainTime) { ///If they have the same remaining CPU time, the process that arrived first is executed
		                    if (runningProcess->arrivalTime <= shortestJob->arrivalTime){
								return runningProcess;
							}
						}
						printf("Preemption in SJF is detected!");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(shortestJob);
					}
					return runningProcess;

				} else {
                    return runningProcess;
                }				
        	} else {
				return removeFrom_RQ(shortestJob);
			}
	}else { // ready Queue is empty!
		return runningProcess;
	}
}

processPointer Priority_a(int preemptive) {
    processPointer importantJob = readyQueue[0];
	
	if(importantJob != NULL) { 
        for (int i=0;i<cur_proc_num_RQ;i++) {
            if (readyQueue[i]-> priority <= importantJob->priority) {
                if(readyQueue[i]->priority == importantJob->priority) { //If they have the same priority, the process that arrived first is executed
                    if (readyQueue[i]->arrivalTime < importantJob->arrivalTime)
						importantJob = readyQueue[i];
                } else {
                    importantJob = readyQueue[i];
                }
            }
        }
		
		if(runningProcess != NULL) { // If there is a process that is running already
				if(preemptive){
					if(runningProcess->priority >= importantJob->priority) {
						if(runningProcess->priority == importantJob->priority) { //If they have the same priority, the process that arrived first is executed
		                    if (runningProcess->arrivalTime <= importantJob->arrivalTime){
								return runningProcess;
							} 
						}
						printf("Preemption in Priority is detected.");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(importantJob);
					}
					return runningProcess;
				} else {
                    return runningProcess;
                }
        	} else {
				return removeFrom_RQ(importantJob);
			}
	}else { // ready Queue is empty!
		return runningProcess;
	}
}

processPointer RR_a() {
    processPointer earliestProc = readyQueue[0];
        if (earliestProc != NULL){
            if(runningProcess != NULL) { 
				if(timeConsumed >= TQ){ //If the running process' time expires		
					insertInto_RQ(runningProcess);
					return removeFrom_RQ(earliestProc);
				} else {
					return runningProcess;
				}
        	} else {
				return removeFrom_RQ(earliestProc);
			}
        } else { //ready queue is empty!
            return runningProcess; 
        }
}

processPointer schedule(int alg, int preemptive) { //Processes with the scheudling algorithm in the given time
	processPointer selectedProcess = NULL;
    
    switch(alg) {
        case FCFS:
            selectedProcess = FCFS_a();
            break;
        case SJF:
        	selectedProcess = SJF_a(preemptive);
        	break;
        case PRIORITY:
        	selectedProcess = Priority_a(preemptive);
        	break;
        case RR:
        	selectedProcess = RR_a();
        	break;
        default:
            return NULL;
    }

    if (selectedProcess==NULL) {
        printf("returning a null for selected process..\n");
        return NULL;
    } else{
        return selectedProcess;
    }
}

//SIMULATION
void simulate(int amount, int alg, int preemptive) { 
	//First, we put the processes that arrived in the selected time to the RQ
	processPointer temp = NULL;
    
	for(int i = 0; i < cur_proc_num_JQ; i++) {
		if(cJobQueue[i]->arrivalTime == amount) {
            temp = removeFrom_CJQ(cJobQueue[i--]);
			insertInto_RQ(temp);
		}
	}

	processPointer prevProc = runningProcess;
	runningProcess = schedule(alg, preemptive); //Pick the process to execute in this turn.

	printf("%d: ",amount);

	if(prevProc != runningProcess) { //If the selected process changed from previous simulate
		printf("Process changed. Previous process run time: %d\n", timeConsumed);
		timeConsumed = 0; //Initialize time consumed for selected process
	}
	
    for(int i = 0; i < cur_proc_num_RQ; i++) { //make every processes in the ready queue wait longer. 
        if(readyQueue[i]) {
        	readyQueue[i]->waitingTime++;
        	readyQueue[i]->turnaroundTime++;
    	}
    }
	
    processPointer temporary = NULL;
    for(int i = 0; i < cur_proc_num_WQ; i++) { //Execute the processes in the waiting Queue. No separate IO process scheduling implementation. Just run the IO operations at the same time. 
		if(waitingQueue[i]) {
			waitingQueue[i]->waitingTime++;
			waitingQueue[i]->turnaroundTime++;
			waitingQueue[i]->IORemainTime--;
			
			if(waitingQueue[i]->IORemainTime <= 0 ) { //If finished IO time
				printf("(pid: %d) -> IO complete, ", waitingQueue[i]->pid);
				temporary = removeFrom_WQ(waitingQueue[i--]); //Remove from the waiting Queue
                insertInto_T(temporary);
				//print_WQ();
			}
		}
	}
	if(runningProcess->IORemainTime > 0) { //If the process needs IO operation, send it to the waiting queue.
				insertInto_WQ(runningProcess);
				runningProcess = NULL;
				printf("-> IO request\n");
	}

    if(runningProcess != NULL) { //Execute the running process
        runningProcess->CPURemainTime--;
        runningProcess->turnaroundTime++;
        timeConsumed++;
        printf("(pid: %d) -> running ",runningProcess->pid);
        
        if(runningProcess->CPURemainTime <= 0) { // If execution is finished, send it to the terminated queue.
			insertInto_T(runningProcess);
			runningProcess = NULL;
			printf("-> terminated");
		}
        printf("\n");
    } else { //If there is no process running..
    	printf("idle\n");
    	Computation_idle = Computation_idle + 1;
	}
}

void analyze(int alg, int preemptive) {
	
	int wait_sum = 0;
	int turnaround_sum = 0;
	int i;
	processPointer p=NULL;
    printf("cur_proc_num: %d\n", cur_proc_num_T);
	printf("===========================================================\n");
	for(i=0;i<cur_proc_num_T;i++){
		p = terminatedQueue[i];
		printf("(pid: %d)\n",p->pid);
		printf("waiting time = %d, ",p->waitingTime);
		printf("turnaround time = %d, \n",p->turnaroundTime);
		printf("===========================================================\n");
		wait_sum += p->waitingTime;
		turnaround_sum += p->turnaroundTime;
	}
	printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",Computation_start, Computation_end,
	 (double)(Computation_end - Computation_idle)/(Computation_end - Computation_start)*100);
	
	if(cur_proc_num_T != 0) {
		printf("Average waiting time: %d\n",wait_sum/cur_proc_num_T);
		printf("Average turnaround time: %d\n",turnaround_sum/cur_proc_num_T);
	}	
	printf("Completed: %d\n",cur_proc_num_T);

typedef struct evaluation {
    int alg;
    int preemptive;
    int startTime;
    int endTime;
    int avg_waitingTime;
    int avg_turnaroundTime;
    int CPU_util;
    int completed;
}evaluation;

	if(cur_proc_num_T != 0) {
		evaluationPointer newEval = (evaluationPointer)malloc(sizeof(struct evaluation));
		newEval->alg = alg;
		newEval->preemptive = preemptive;
		newEval->startTime = Computation_start;
		newEval->endTime = Computation_end;
		newEval->avg_waitingTime = wait_sum/cur_proc_num_T;
		newEval->avg_turnaroundTime = turnaround_sum/cur_proc_num_T;
		newEval->CPU_util = (double)(Computation_end - Computation_idle)/(Computation_end - Computation_start)*100;
		newEval->completed = cur_proc_num_T;
		eval[cur_eval_num++] = newEval;
	}
    printf("=============================================================\n");
}


void evaluate() {
	
	printf ("\n                       <Evaluation>                    \n");
	int i;
	for(i=0;i<cur_eval_num;i++) {
		
		printf("===========================================================\n");
		
		int alg = eval[i]->alg;
		int preemptive = eval[i]->preemptive;
		
		switch (eval[i]->alg) {
		
		case FCFS:
            puts("<FCFS Algorithm>");
            break;
        case SJF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("SJF Algorithm>");
        	break;
        case RR:
        	puts("<Round Robin Algorithm>");
        	break;
        case PRIORITY:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("Priority Algorithm>");
        	break;
        default:
        return;
		}
		printf("-----------------------------------------------------------\n");
		printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",eval[i]->startTime, eval[i]->endTime, eval[i]->CPU_util);
		printf("Average waiting time: %d\n",eval[i]->avg_waitingTime);
		printf("Average turnaround time: %d\n",eval[i]->avg_turnaroundTime);
		printf("Completed: %d\n",eval[i]->completed);
	}	
	printf("===========================================================");
}


void startSimulation(int alg, int preemptive, int count) {
    printf("cur_proc_num_JQ: %d\n", cur_proc_num_JQ);
    createClone_JQ();
    
    printf("WTF IS THIS...EXAMINING THE CLONE READY QUEUE STATUS\n");
    for (int i=0;i<cur_proc_num_JQ;i++) {
        printf("ARRIVAL TIME: %d\n", cJobQueue[i]->arrivalTime);
        printf("PID: %d\n", cJobQueue[i]->pid);
        printf("CPUtime: %d\n", cJobQueue[i]->CPUburst);
        printf("IOTIME: %d\n", cJobQueue[i]->IOburst);
    }
    
    int first_proc_num = cur_proc_num_JQ;
	
	int i=0;
	if(cur_proc_num_JQ <= 0) {
		puts("<error> Simulation failed. Process doesn't exist in the job queue");
		return;
	}
    printf("LET'S GET ITTTT!\n");
    Computation_start = cJobQueue[0]->arrivalTime;
	Computation_idle = 0;
	for(i=0;i<count;i++) {
        if(i < Computation_start) {
            Computation_idle++;
        } else {
            simulate(i,alg, preemptive);
            if(cur_proc_num_T == first_proc_num) {
                i++;
                break;
            }
        }
	}
	Computation_end = i-1;

	analyze(alg, preemptive);

	clear_CJQ();
    clear_RQ();
    clear_TQ();
    clear_WQ();
    free(runningProcess);
    runningProcess = NULL;
    timeConsumed = 0;
    Computation_start = 0;
	Computation_end = 0;
	Computation_idle = 0;

    cur_proc_num_JQ=0;
    cur_proc_num_RQ=0;
    cur_proc_num_WQ=0;
    cur_proc_num_T=0;
}




int main() {
    //Initiazation
    init_JQ();
    init_RQ();
    init_WQ();
    init_eval();
    
    createProcesses();
    startSimulation(FCFS,F,TIME);
    startSimulation(SJF,F,TIME);
    startSimulation(SJF,T,TIME);
    startSimulation(PRIORITY,F,TIME);
    startSimulation(PRIORITY,T,TIME);
    startSimulation(RR,F,TIME);
    evaluate();

    clear_JQ();
    clear_RQ();
    clear_WQ();
    clear_CJQ();
    clear_eval();
    
    return 0;
}


