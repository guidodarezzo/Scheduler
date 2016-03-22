
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <streambuf>
#include <vector>
#include <cmath>
#include <algorithm> 
#include <deque>


using namespace std;

// declare my methods...
int strToInt (string s);
void sortProcesses(); // sorts processes.

void fileReader(); // reads in file, creates process objects from each process, stores information in quadruples into objects
				   // Objects are then added to a vector for sorting later.
int randomOS(int burst);
void createRandomNumVector();

void fcfs();
void lcfs();
void roundRobin();
void hprn();


class process;

bool isVerbose = false;

char entireFile[601]; 
int process_num = 0;
int process_idx = 0;
int file_idx = 0; // Keep track of how many elements total in entireFile.

std::vector<int> randomNumbers;
int randomIndex = -1; // this increments every time a random number is used. Is -1 because it increments before returning the random num

std::vector<process> presort;
std::vector<process> processes;

int CPUutilization = 0; // needs to be reset at the end of each algorithm to prep for the next algo to run
int IOutilization = 0; // same as CPUutilization but for total IO Utilization instead.

int quantum = 2; // can be reset globally.


class process {
	private: 
		int arrivalTime_A;
		int cpuBurst_B;
		int cpuTime_C;
		int IOtime_D;

		int arrival_tiebreak; // process's priority number. Equal to arrivalTime_A + position in sorted array.

		string current_state;
		
		int timeRemaining; // set to 0 in constructor
		int CPUcycles; // set to 0 in constructor
		int totalIO; // set to 0 in constructor
		int waitingTime; // set to 0 in constructor.

		int startTime;
		int finishingTime;
		int turnaroundTime;

		int PID; // this is the process's position in the processes vector. 

		bool greaterThan;

		int remainingQuant;

	public:
		process(int arrival, int CPUburst, int CPUtime, int IOtime, int TR, int CC, int TIO, int WT);

		int getArrival();
		void setArrival(int arrival);

		int getCPUburst();
		void setCPUburst(int CPUburst);

		int getCPUTime();
		void setCPUTime(int CPUtime);

		int getIOtime();
		void setIOtime(int IOtime);

		int getArrivalTiebreak();
		void setArrivalTiebreak(int tiebreak_arrival);

		string getCurrent_state();
		void setCurrent_state(string state);

		int getTimeRemaining();
		void setTimeRemaining(int remaining);

		int getCPUCycles();
		void clearCPUCycles();
		void setCPUCycles();

		int getTotalIO();
		void clearTotalIO();
		void setTotalIO(int burst);

		int getWaitingTime();
		void setWaitingtime(int waiting);
		void incrementWaiting();
		void clearWaiting();

		int getFinishingTime();
		void setFinishingTime(int finish);

		int getTurnaroundTime();
		void setTurnaroundTime();

		int getStartTime();
		void setStartTime(int start);

		int getPID();
		void setPID(int numPID);

		bool getGreaterThanQuant();
		void setGreaterThanQuant(int burst);

		int getRemainingQuant();
		void setRemainingQuant(int burst);
		void decrementRemainingQuant();

};


process::process (int arrival, int CPUburst, int CPUtime, int IOtime, int TR, int CC, int TIO, int WT){
	arrivalTime_A = arrival;
	cpuBurst_B = CPUburst;
	cpuTime_C = CPUtime;
	IOtime_D = IOtime;
	timeRemaining = TR;
	CPUcycles = CC;
	totalIO = TIO;
	waitingTime = WT;
}



// these are my getters and setters

int process::getRemainingQuant() {
	return remainingQuant;
}

void process::decrementRemainingQuant() {
	remainingQuant -= quantum;
	if (remainingQuant < 0) {
		remainingQuant = 0;
	}
}

void process::setRemainingQuant(int burst) {
	remainingQuant = burst;
}

bool process::getGreaterThanQuant() {
	return greaterThan;
}

void process::setGreaterThanQuant(int burst) {
	if (burst > 2) {
		greaterThan = true;
	}

	else {
		greaterThan = false;
	}

	
}

int process::getPID(){
	return PID;
}

void process::setPID(int numPID){
	PID = numPID;
}

int process::getStartTime() {
	return startTime;
}
void process::setStartTime(int start){
	startTime = start;
}

int process::getTurnaroundTime(){
	return turnaroundTime;
}
void process::setTurnaroundTime(){
	turnaroundTime = finishingTime - arrivalTime_A;
}

int process::getFinishingTime(){
	return finishingTime;
}
void process::setFinishingTime(int finish){
	finishingTime = finish;
}

void process::clearWaiting() {
	waitingTime = 0;
}

int process::getWaitingTime(){
	return waitingTime;
}
void process::setWaitingtime(int waiting) {
	waitingTime = waiting;
}

void process::incrementWaiting() {
	waitingTime++;
}

int process::getTotalIO() {
	return totalIO;
}

void process::clearTotalIO() {
	totalIO = 0;
}
void process::setTotalIO(int burst){
	totalIO += burst;
}

int process::getCPUCycles(){
	return CPUcycles;
}

void process::clearCPUCycles() {
	CPUcycles = 0;
}

void process::setCPUCycles(){
	CPUcycles++;
}

string process::getCurrent_state() {
	return current_state;
}

void process::setCurrent_state(string state){
	current_state = state;
}

int process::getTimeRemaining() {
	return timeRemaining;
}
void process::setTimeRemaining(int remaining){
	timeRemaining = remaining;
}

int process::getArrival() {
	return arrivalTime_A;
}

void process::setArrival(int arrival){
	arrivalTime_A = arrival;
}

int process::getCPUburst(){
	return cpuBurst_B;
}

void process::setCPUburst(int CPUburst){
	cpuBurst_B = CPUburst;
}

int process::getCPUTime(){
	return cpuTime_C;
}

void process::setCPUTime(int CPUtime){
	cpuTime_C = CPUtime;
}

int process::getIOtime(){
	return IOtime_D;
}

void process::setIOtime(int IOtime){
	IOtime_D = IOtime;
}

int process::getArrivalTiebreak(){
	return arrival_tiebreak;
}

void process::setArrivalTiebreak(int tiebreak_arrival){
	arrival_tiebreak = tiebreak_arrival; // needs to be position in array plus its original arrival time
}



// string to int function
int strToInt (string s) {
	int newInt = atoi(s.c_str());
	return newInt;
}



void fileReader() {

	char fileContent;
	while ((fileContent = getchar()) != EOF){
		entireFile[file_idx] = fileContent;
		file_idx++;
		if (file_idx > 600){
			cout << "Characters in file exceed 600 char limit. Exiting now..." << endl;
			exit(0);
		}
	}

	if (file_idx < 1){
		cout << "File is empty. Exiting program now..." << endl;
		exit(1);

	}

	entireFile[file_idx] = '*'; // mark the first empty element in char array as '*'.

	// file has been copied to entireFile[].
	int idx = 0;
	while (entireFile[idx] != '*'){

		char c = entireFile[idx];

		while((c == ' ') || (c == '\t') || (c == '\n') || (c == '(') || (c == ')')){
			idx++;
			c = entireFile[idx];
		}

		string num_processes = "";

		while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '(') && (c != ')')){
			num_processes += c;
			idx++;
			c = entireFile[idx];
		}

		process_num = strToInt(num_processes);

		//cout << "process_num: " << process_num << endl;

		for (int i = 0; i < process_num; i++){


			while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '(') || (c == ')')){
				idx++;
				c = entireFile[idx];
			}

			string varA = "";

			while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '(') && (c != ')')){
				varA += c;
				idx++;
				c = entireFile[idx];
			}

			while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '(') || (c == ')')){
				idx++;
				c = entireFile[idx];
			}

			string varB = "";

			while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '(') && (c != ')')){
				varB += c;
				idx++;
				c = entireFile[idx];
			}

			while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '(') || (c == ')')){
				idx++;
				c = entireFile[idx];
			}

			string varC = "";

			while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '(') && (c != ')')){
				varC += c;
				idx++;
				c = entireFile[idx];
			}

			while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '(') || (c == ')')){
				idx++;
				c = entireFile[idx];
			}

			string varD = "";

			while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '(') && (c != ')')){
				varD += c;
				idx++;
				c = entireFile[idx];
			}

			process temp(strToInt(varA), strToInt(varB), strToInt(varC), strToInt(varD), 0, 0, 0, 0);

			processes.push_back(temp);



		} // end for loop

		break;

	} // end outer while loop


} // end fileReader

void sortProcesses () {

	//process sort_array[process_num];

	std::vector<process> sortVector;


	for (int i = 0; i < process_num; i++){
		presort.push_back(processes.front());
		processes.erase(processes.begin()); // pops the front of the processes
	}

	for (int i = 0 ; i < presort.size() ; i++) {
		sortVector.insert(sortVector.begin()+i,presort.at(i));
	}




	for (int i = (process_num - 1); i > 0; i--) {
		
		for (int j = (i - 1); j >= 0; j--){
			if (sortVector[i].getArrival() < sortVector[j].getArrival()){
				process tmp = sortVector[i];
				sortVector[i] = sortVector[j];
				sortVector[j] = tmp;
			}
		}

	}

	


	for (int i = 0; i < process_num; i++){
		
		sortVector[i].setArrivalTiebreak(i);
		
	}


	for (int j = 0; j < process_num - 1; j++) {

		for (int i = j + 1; i < (process_num); i++) {

				if (sortVector[j].getArrival() < sortVector[i].getArrival()) {
					continue;
				}
				else {

					if ((sortVector[i].getCPUburst() < sortVector[j].getCPUburst()) || (sortVector[i].getCPUburst() == sortVector[j].getCPUburst()) && (sortVector[i].getCPUTime() < sortVector[j].getCPUTime())
						|| (sortVector[i].getCPUburst() == sortVector [j].getCPUburst()) && (sortVector[i].getCPUTime() == sortVector[j].getCPUTime()) && (sortVector[i].getIOtime() < sortVector[j].getIOtime())) {
						process tmp = sortVector[i];
						sortVector[i] = sortVector[j];
						sortVector[j] = tmp;
					}

					// else if ((sortVector[i].getCPUburst() == sortVector[j].getCPUburst()) && (sortVector[i].getCPUTime() < sortVector[j].getCPUTime())) {
					// 	process tmp(sortVector[i].getArrival(), sortVector[i].getCPUburst(), sortVector[i].getCPUTime(), sortVector[i].getIOtime(),0, 0, 0, 0);
					// 	sortVector[i] = sortVector[j];
					// 	sortVector[j] = tmp;
					// }

					// else if ((sortVector[i].getCPUburst() == sortVector [j].getCPUburst()) && (sortVector[i].getCPUTime() == sortVector[j].getCPUTime()) && (sortVector[i].getIOtime() < sortVector[j].getIOtime())) {
					// 	process tmp(sortVector[i].getArrival(), sortVector[i].getCPUburst(), sortVector[i].getCPUTime(), sortVector[i].getIOtime(),0, 0, 0, 0);
					// 	sortVector[i] = sortVector[j];
					// 	sortVector[j] = tmp;
					// }
				}

			
		}

	}



	for (int i = 0; i < process_num; i++){
		sortVector[i].setPID(i);
		//sortVector[i].setArrivalTiebreak(i);
		processes.push_back(sortVector[i]);


	}





} // end sortProcesses


void createRandomNumVector(){

	ifstream infile;
	infile.open("random-numbers.txt");

	int d;
	while (infile >> d){
		randomNumbers.push_back(d);
	}

	
} // end createRandomNumVector

int randomOS(int burst){
	randomIndex++;
	int randomNumber = ((randomNumbers[randomIndex])%burst) + 1;
	return randomNumber;
} // end randomOS

void fcfs() {

	int terminated = 0;
	randomIndex = -1;

	int running_process = -1; // indicates that there are no current processes running
	std::deque<int> ready_process;
	std::deque<int> blocked_process;

	int cycleNum = 0;

	for (int i = 0; i < process_num; i++) {
		processes[i].setCurrent_state("unstarted");
		processes[i].clearTotalIO();
		processes[i].clearCPUCycles();
		processes[i].clearWaiting();
	}

	cout << "The original input was: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << presort[i].getArrival() << "  " << left << presort[i].getCPUburst() << "  " << left << presort[i].getCPUTime() << "  " << left << presort[i].getIOtime() << '\t' << left;
	}

	cout << '\n';


	cout << "The (sorted) input is: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << processes[i].getArrival() << "  " << left << processes[i].getCPUburst() << "  " << left << processes[i].getCPUTime() << "  " << left << processes[i].getIOtime() << '\t' << left;
	}

	cout << '\n';
	cout << '\n';
	
	if (isVerbose) {
		cout << "This detailed printout gives the state and remaining burst for each process " << endl;
	}

	cout << '\n';

	while (terminated < process_num) {
	
		if (isVerbose) {

			// cout << "Before cycle" << setw(5) << cycleNum << ":";
			// for (int i = 0; i < process_num; i++){
			// 	cout << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
			// }
			// cout <<"."<< endl;

			stringstream buffer;

			buffer << "Before cycle" << setw(5) << cycleNum << ":";
			for (int i = 0; i < process_num; i++) {
				buffer << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
			}
			buffer << ".";

			cout << buffer.str() << endl;

		}


		for (int i = 0; i < blocked_process.size(); i++) {

				int getTR = processes[blocked_process[i]].getTimeRemaining();
				processes[blocked_process[i]].setTimeRemaining(getTR - 1);
				if (processes[blocked_process[i]].getTimeRemaining() == 0) {
					processes[blocked_process[i]].setCurrent_state("ready");
					ready_process.push_back(blocked_process[i]);
					blocked_process.erase(blocked_process.begin() + i);
					i--;
				}
			} // end blocked 

			// unstarted and arriving in this cycle, put them into ready for now
		// will check for running later in the loop

		for (int i = 0; i < process_num; i++){

			if ((processes[i].getCurrent_state() == "unstarted") && (processes[i].getArrival() == cycleNum)) {

				//cout << "in unstarted loop" << endl;
				processes[i].setCurrent_state("ready");
				ready_process.push_back(i);
				

				} 
			} // end for loop checking arrivals

		if (running_process > -1) {
			processes[running_process].setTimeRemaining(processes[running_process].getTimeRemaining() - 1);
			processes[running_process].setCPUCycles();

			if (processes[running_process].getCPUCycles() == processes[running_process].getCPUTime()) {
				processes[running_process].setFinishingTime(cycleNum);
				processes[running_process].setTurnaroundTime();
				processes[running_process].setTimeRemaining(0);
				processes[running_process].setCurrent_state("terminated");

				terminated++;
				running_process = -1;
			}

			else if (processes[running_process].getTimeRemaining() == 0) {
				int IOBurst = randomOS(processes[running_process].getIOtime());
				processes[running_process].setTotalIO(IOBurst);
				processes[running_process].setTimeRemaining(IOBurst);
				processes[running_process].setCurrent_state("blocked");
				bool inBlocked = false;
				//blocked_process.push_back(running_process);
				for (int i = 0; i < blocked_process.size(); i++) {
					if (processes[running_process].getArrivalTiebreak() < processes[blocked_process[i]].getArrivalTiebreak()) {
						blocked_process.insert(blocked_process.begin() + i, running_process);
						inBlocked = true;
						break;
					}

					if (processes[running_process].getArrivalTiebreak() == processes[blocked_process[i]].getArrivalTiebreak()) {
						cout << "Arrival tie breaker match" << endl;
					}
				}

				if (!inBlocked) {
					blocked_process.push_back(running_process);
				}
				running_process = -1;
			}
		}

		if (running_process == -1 && !ready_process.empty()) {
			//cout << "inside of runing process check for -1" << endl;
			running_process = ready_process.front();
			processes[running_process].setTimeRemaining(randomOS(processes[running_process].getCPUburst()));
			processes[running_process].setCurrent_state("running");
			ready_process.pop_front();

		} 


		for (int i = 0; i < ready_process.size(); i++) {
			processes[ready_process[i]].incrementWaiting();
		}

		if (running_process > -1) {
			CPUutilization++;
		}

		if (!blocked_process.empty()) {
			IOutilization++;
		}

		cycleNum++;

	} // end outer while loop


	cout << "The scheduling algorithm used was First Come First Served" << endl;
	cout << '\n';
	// now print stats for each cycle.

	int totalTurnaround = 0;
	int totalWait = 0;

	for (int i = 0; i < process_num; i++) {
		cout << "Process " << i << ":" << endl;
		cout << '\t' << "(A,B,C,IO) = (" << processes[i].getArrival() << "," << processes[i].getCPUburst() << "," << processes[i].getCPUTime() << "," << processes[i].getIOtime() << ")" << endl;
		cout << '\t' << "Finishing time: " << processes[i].getFinishingTime() << endl;;
		cout << '\t' << "Turnaround time: " << processes[i].getTurnaroundTime() << endl;
		cout << '\t' << "I/O time: " << processes[i].getTotalIO() << endl;
		cout << '\t' << "Waiting time: " << processes[i].getWaitingTime() << endl;
		cout << '\n';

		totalTurnaround += processes[i].getTurnaroundTime();
		totalWait += processes[i].getWaitingTime();
	}

	// now print Summary Data 

	cout << "Summary Data: " << endl;
	cout << '\t' << "Finishing Time: " << cycleNum - 1 << endl;
	cout << '\t' << "CPU Utilization: " << ((double) CPUutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "I/O Utilization: " << ((double) IOutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "Throughput: " << (process_num * ((double) 100 / (cycleNum - 1))) << endl;
	cout << '\t' << "Average turnaround time: " << ((double) totalTurnaround/process_num) << endl;
	cout << '\t' << "Average waiting time: " << ((double) totalWait / process_num) << endl;



	// reset everything: randomIndex, CPU utilization, IO utilization, etc. 

	randomIndex = -1;
	CPUutilization = 0;
	IOutilization = 0;

} // end fcfs

void lcfs() {

	randomIndex = -1;

	int terminated = 0;

	int running_process = -1; // indicates that there are no current processes running
	std::deque<int> ready_process;
	std::deque<int> blocked_process;

	int cycleNum = 0;

	for (int i = 0; i < process_num; i++) {
		processes[i].setCurrent_state("unstarted");
		processes[i].clearTotalIO();
		processes[i].clearCPUCycles();
		processes[i].clearWaiting();
	}


	cout << "The original input was: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << presort[i].getArrival() << "  " << left << presort[i].getCPUburst() << "  " << left << presort[i].getCPUTime() << "  " << left << presort[i].getIOtime() << '\t' << left;
	}

	cout << '\n';


	cout << "The (sorted) input is: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << processes[i].getArrival() << "  " << left << processes[i].getCPUburst() << "  " << left << processes[i].getCPUTime() << "  " << left << processes[i].getIOtime() << '\t' << left;
	}


	cout << '\n';
	cout << '\n';
	
	if (isVerbose) {
		cout << "This detailed printout gives the state and remaining burst for each process " << endl;
	}

	cout << '\n';


	while (terminated < process_num) {


		if (isVerbose) {

			cout << "Before cycle" << setw(5) << cycleNum << ":";
			for (int i = 0; i < process_num; i++){
				cout << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
			}
			cout <<"."<< endl;

		}

		for (int i = 0; i < blocked_process.size(); i++) {

				int getTR = processes[blocked_process[i]].getTimeRemaining();
				processes[blocked_process[i]].setTimeRemaining(getTR - 1);
				if (processes[blocked_process[i]].getTimeRemaining() == 0) {
					processes[blocked_process[i]].setCurrent_state("ready");
					ready_process.push_back(blocked_process[i]); // changed here
					blocked_process.erase(blocked_process.begin() + i);
					i--;
				}
			} // end blocked 
			// unstarted and arriving in this cycle, put them into ready for now
		// will check for running later in the loop

		for (int i = 0; i < process_num; i++){

			if ((processes[i].getCurrent_state() == "unstarted") && (processes[i].getArrival() == cycleNum)) {

				//cout << "in unstarted loop" << endl;
				processes[i].setCurrent_state("ready");
				ready_process.push_front(i);
				

				} 
			} // end for loop checking arrivals

		if (running_process > -1) {
			processes[running_process].setTimeRemaining(processes[running_process].getTimeRemaining() - 1);
			processes[running_process].setCPUCycles();

			if (processes[running_process].getCPUCycles() == processes[running_process].getCPUTime()) {
				processes[running_process].setFinishingTime(cycleNum);
				processes[running_process].setTurnaroundTime();
				processes[running_process].setTimeRemaining(0);
				processes[running_process].setCurrent_state("terminated");

				terminated++;
				running_process = -1;
			}

			else if (processes[running_process].getTimeRemaining() == 0) {
				int IOBurst = randomOS(processes[running_process].getIOtime());
				processes[running_process].setTotalIO(IOBurst);
				processes[running_process].setTimeRemaining(IOBurst);
				processes[running_process].setCurrent_state("blocked");
				bool inBlocked = false;
				//blocked_process.push_back(running_process);
				for (int i = 0; i < blocked_process.size(); i++) {
			
					if (processes[running_process].getArrivalTiebreak() < processes[blocked_process[i]].getArrivalTiebreak()) {
						blocked_process.insert(blocked_process.begin() + i, running_process);
						inBlocked = true;
						break;
					}

					if (processes[running_process].getArrivalTiebreak() == processes[blocked_process[i]].getArrivalTiebreak()) {
						cout << "Arrival tie breaker match" << endl;
					}
				}

				if (!inBlocked) {
					blocked_process.push_back(running_process);
				}
				running_process = -1;
			}
		}

		if (running_process == -1 && !ready_process.empty()) {
			//cout << "inside of runing process check for -1" << endl;
			running_process = ready_process.back();
			processes[running_process].setTimeRemaining(randomOS(processes[running_process].getCPUburst()));
			processes[running_process].setCurrent_state("running");
			ready_process.pop_back();

		} 


		for (int i = 0; i < ready_process.size(); i++) {
			processes[ready_process[i]].incrementWaiting();
		}

		if (running_process > -1) {
			CPUutilization++;
		}

		if (!blocked_process.empty()) {
			IOutilization++;
		}

		cycleNum++;

	} // end outer while loop

	cout << "The scheduling algorithm used was Last Come First Served" << endl;
	cout << '\n';
	// now print stats for each cycle.

	int totalTurnaround = 0;
	int totalWait = 0;

	for (int i = 0; i < process_num; i++) {
		cout << "Process " << i << ":" << endl;
		cout << '\t' << "(A,B,C,IO) = (" << processes[i].getArrival() << "," << processes[i].getCPUburst() << "," << processes[i].getCPUTime() << "," << processes[i].getIOtime() << ")" << endl;
		cout << '\t' << "Finishing time: " << processes[i].getFinishingTime() << endl;;
		cout << '\t' << "Turnaround time: " << processes[i].getTurnaroundTime() << endl;
		cout << '\t' << "I/O time: " << processes[i].getTotalIO() << endl;
		cout << '\t' << "Waiting time: " << processes[i].getWaitingTime() << endl;
		cout << '\n';

		totalTurnaround += processes[i].getTurnaroundTime();
		totalWait += processes[i].getWaitingTime();
	}

	// now print Summary Data 

	cout << "Summary Data: " << endl;
	cout << '\t' << "Finishing Time: " << cycleNum - 1 << endl;
	cout << '\t' << "CPU Utilization: " << ((double) CPUutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "I/O Utilization: " << ((double) IOutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "Throughput: " << (process_num * ((double) 100 / (cycleNum - 1))) << endl;
	cout << '\t' << "Average turnaround time: " << ((double) totalTurnaround/process_num) << endl;
	cout << '\t' << "Average waiting time: " << ((double) totalWait / process_num) << endl;



	// reset everything: randomIndex, CPU utilization, IO utilization, etc. 

	randomIndex = -1;
	CPUutilization = 0;
	IOutilization = 0;
	
} // end lcfs


void roundRobin() {
	int current_quant = 0;
	int terminated = 0;
	randomIndex = -1;

	int running_process = -1; // indicates that there are no current processes running
	std::deque<int> ready_process;
	std::deque<int> blocked_process;

	int cycleNum = 0;

	for (int i = 0; i < process_num; i++) {
		processes[i].setCurrent_state("unstarted");
		processes[i].clearTotalIO();
		processes[i].clearCPUCycles();
		processes[i].clearWaiting();
	}

	cout << "The original input was: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << presort[i].getArrival() << "  " << left << presort[i].getCPUburst() << "  " << left << presort[i].getCPUTime() << "  " << left << presort[i].getIOtime() << '\t' << left;
	}

	cout << '\n';


	cout << "The (sorted) input is: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << processes[i].getArrival() << "  " << left << processes[i].getCPUburst() << "  " << left << processes[i].getCPUTime() << "  " << left << processes[i].getIOtime() << '\t' << left;
	}


	cout << '\n';
	cout << '\n';
	
	if (isVerbose) {
		cout << "This detailed printout gives the state and remaining burst for each process " << endl;
	}

	cout << '\n';

	while (terminated < process_num) {


		if (isVerbose) {

			// cout << "Before cycle" << setw(5) << cycleNum << ":";
			// for (int i = 0; i < process_num; i++){
			// 	cout << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
			// }
			// cout <<"."<< endl;

			stringstream buffer;

			buffer << "Before cycle" << setw(5) << cycleNum << ":";
			for (int i = 0; i < process_num; i++) {

	

				if (processes[i].getCurrent_state() == "running") {
					buffer << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << current_quant;
				}

				else if (processes[i].getCurrent_state() == "ready") {
					buffer << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << 0;
				}

				else {
				buffer << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
				}
			}

			buffer << ".";

			cout << buffer.str() << endl;

		}

			int numUnblockedorUnstarted = 0;

			for (int i = 0; i < blocked_process.size(); i++) {

				int getTR = processes[blocked_process[i]].getTimeRemaining();
				processes[blocked_process[i]].setTimeRemaining(getTR - 1);
				if (processes[blocked_process[i]].getTimeRemaining() == 0) {
					numUnblockedorUnstarted++;
					processes[blocked_process[i]].setCurrent_state("ready");
					ready_process.push_back(blocked_process[i]);
					blocked_process.erase(blocked_process.begin() + i);
					i--;
				}
			} // end blocked 	

			// unstarted and arriving in this cycle, put them into ready for now
		// will check for running later in the loop



		for (int i = 0; i < process_num; i++){

			if ((processes[i].getCurrent_state() == "unstarted") && (processes[i].getArrival() == cycleNum)) {

				//cout << "in unstarted loop" << endl;
				processes[i].setCurrent_state("ready");
				//ready_process.push_back(i);
				//ready_process.insert(ready_process.begin() + ready_process.end() - numUnblocked + 1, i);
				ready_process.push_back(i);
				numUnblockedorUnstarted++;

				} 
			} // end for loop checking arrivals

		if (running_process > -1) {
			processes[running_process].setTimeRemaining(processes[running_process].getTimeRemaining() - 1);
			current_quant = current_quant -1;
			processes[running_process].setCPUCycles();

			if (processes[running_process].getCPUCycles() == processes[running_process].getCPUTime()) {
				processes[running_process].setFinishingTime(cycleNum);
				processes[running_process].setTurnaroundTime();
				processes[running_process].setTimeRemaining(0);
				processes[running_process].setCurrent_state("terminated");

				terminated++;
				running_process = -1;
			}

			else if (processes[running_process].getTimeRemaining() == 0) {
				int IOBurst = randomOS(processes[running_process].getIOtime());
				processes[running_process].setTotalIO(IOBurst);
				processes[running_process].setTimeRemaining(IOBurst);
				processes[running_process].setCurrent_state("blocked");
				bool inBlocked = false;
				//blocked_process.push_back(running_process);
				for (int i = 0; i < blocked_process.size(); i++) {
					if (processes[running_process].getArrivalTiebreak() < processes[blocked_process[i]].getArrivalTiebreak()) {
						blocked_process.insert(blocked_process.begin() + i, running_process);
						inBlocked = true;
						break;
					}

					if (processes[running_process].getArrivalTiebreak() == processes[blocked_process[i]].getArrivalTiebreak()) {
						cout << "Arrival tie breaker match" << endl;
					}
				}

				if (!inBlocked) {
					blocked_process.push_back(running_process);
				}
				running_process = -1;
			}

			if (current_quant == 0 && running_process > -1) {

			// check if CPU burst was greater than the quantum? if so, add to ready
				processes[running_process].setCurrent_state("ready");
		        bool inReady = false;
		
				// for (int i = ready_process.size()-1; i >= (ready_process.size()-numUnblocked); i--) {
				// 	if (processes[running_process].getArrivalTiebreak() < processes[ready_process[i]].getArrivalTiebreak()) {
				// 		cout << "In If Statement" << endl;
				// 		ready_process.insert(ready_process.begin() + i, running_process);
				// 		inReady = true;
				// 		break;
				// 	}
				// }



				int startIndex = ready_process.size() - numUnblockedorUnstarted;
				int endIndex = ready_process.size();

				for (int i = startIndex; i < endIndex; i++) {
			
					if ((processes[running_process].getArrivalTiebreak()) < processes[ready_process[i]].getArrivalTiebreak()) {
						ready_process.insert(ready_process.begin() + i, running_process);
						inReady = true;
						//cout << "before break" << endl;
						break;
					}
				}				



			

				if (!inReady) {
					ready_process.push_back(running_process);
				}
				running_process = -1;

			}
		}
		

		if (running_process == -1 && !ready_process.empty()) {
			//cout << "inside of runing process check for -1" << endl;
			running_process = ready_process.front();
			if (processes[running_process].getTimeRemaining() == 0) {
			processes[running_process].setTimeRemaining(randomOS(processes[running_process].getCPUburst()));
			}
			current_quant = min(2, processes[running_process].getTimeRemaining());
			processes[running_process].setCurrent_state("running");
			ready_process.pop_front();

		} 


		for (int i = 0; i < ready_process.size(); i++) {
			processes[ready_process[i]].incrementWaiting();
		}

		if (running_process > -1) {
			CPUutilization++;
		}

		if (!blocked_process.empty()) {
			IOutilization++;
		}

		cycleNum++;

	} // end outer while loop



	cout << "The scheduling algorithm used was Round Robin" << endl;
	cout << '\n';
	// now print stats for each cycle.

	int totalTurnaround = 0;
	int totalWait = 0;

	for (int i = 0; i < process_num; i++) {
		cout << "Process " << i << ":" << endl;
		cout << '\t' << "(A,B,C,IO) = (" << processes[i].getArrival() << "," << processes[i].getCPUburst() << "," << processes[i].getCPUTime() << "," << processes[i].getIOtime() << ")" << endl;
		cout << '\t' << "Finishing time: " << processes[i].getFinishingTime() << endl;;
		cout << '\t' << "Turnaround time: " << processes[i].getTurnaroundTime() << endl;
		cout << '\t' << "I/O time: " << processes[i].getTotalIO() << endl;
		cout << '\t' << "Waiting time: " << processes[i].getWaitingTime() << endl;
		cout << '\n';

		totalTurnaround += processes[i].getTurnaroundTime();
		totalWait += processes[i].getWaitingTime();
	}

	// now print Summary Data 

	cout << "Summary Data: " << endl;
	cout << '\t' << "Finishing Time: " << cycleNum - 1 << endl;
	cout << '\t' << "CPU Utilization: " << ((double) CPUutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "I/O Utilization: " << ((double) IOutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "Throughput: " << (process_num * ((double) 100 / (cycleNum - 1))) << endl;
	cout << '\t' << "Average turnaround time: " << ((double) totalTurnaround/process_num) << endl;
	cout << '\t' << "Average waiting time: " << ((double) totalWait / process_num) << endl;



	// reset everything: randomIndex, CPU utilization, IO utilization, etc. 

	randomIndex = -1;
	CPUutilization = 0;
	IOutilization = 0;

} // end Round Robin


void hprn() {

	int terminated = 0;
	randomIndex = -1;

	int running_process = -1; // indicates that there are no current processes running
	std::deque<int> ready_process;
	std::deque<int> blocked_process;

	int cycleNum = 0;

	for (int i = 0; i < process_num; i++) {
		processes[i].setCurrent_state("unstarted");
		processes[i].clearTotalIO();
		processes[i].clearCPUCycles();
		processes[i].clearWaiting();
	}

	cout << "The original input was: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << presort[i].getArrival() << "  " << left << presort[i].getCPUburst() << "  " << left << presort[i].getCPUTime() << "  " << left << presort[i].getIOtime() << '\t' << left;
	}

	cout << '\n';


	cout << "The (sorted) input is: " << setw(3) << left << process_num << '\t';

	for (int i = 0; i < process_num; i++) {
		cout << processes[i].getArrival() << "  " << left << processes[i].getCPUburst() << "  " << left << processes[i].getCPUTime() << "  " << left << processes[i].getIOtime() << '\t' << left;
	}


	cout << '\n';
	cout << '\n';
	
	if (isVerbose) {
		cout << "This detailed printout gives the state and remaining burst for each process " << endl;
	}

	cout << '\n';

	while (terminated < process_num) {
	
		if (isVerbose) {

			// cout << "Before cycle" << setw(5) << cycleNum << ":";
			// for (int i = 0; i < process_num; i++){
			// 	cout << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
			// }
			// cout <<"."<< endl;

			stringstream buffer;

			buffer << "Before cycle" << setw(5) << cycleNum << ":";
			for (int i = 0; i < process_num; i++) {
				buffer << '\t' << setw(5) << right << processes[i].getCurrent_state() << setw(3) << right << processes[i].getTimeRemaining();
			}
			buffer << ".";

			cout << buffer.str() << endl;

		}


		for (int i = 0; i < blocked_process.size(); i++) {

				int getTR = processes[blocked_process[i]].getTimeRemaining();
				processes[blocked_process[i]].setTimeRemaining(getTR - 1);
				if (processes[blocked_process[i]].getTimeRemaining() == 0) {
					processes[blocked_process[i]].setCurrent_state("ready");
					ready_process.push_back(blocked_process[i]);
					blocked_process.erase(blocked_process.begin() + i);
					i--;
				}
			} // end blocked 

			// unstarted and arriving in this cycle, put them into ready for now
		// will check for running later in the loop

		for (int i = 0; i < process_num; i++){

			if ((processes[i].getCurrent_state() == "unstarted") && (processes[i].getArrival() == cycleNum)) {

				//cout << "in unstarted loop" << endl;
				processes[i].setCurrent_state("ready");
				ready_process.push_back(i);
				

				} 
			} // end for loop checking arrivals

		if (running_process > -1) {
			processes[running_process].setTimeRemaining(processes[running_process].getTimeRemaining() - 1);
			processes[running_process].setCPUCycles();

			if (processes[running_process].getCPUCycles() == processes[running_process].getCPUTime()) {
				processes[running_process].setFinishingTime(cycleNum);
				processes[running_process].setTurnaroundTime();
				processes[running_process].setTimeRemaining(0);
				processes[running_process].setCurrent_state("terminated");

				terminated++;
				running_process = -1;
			}

			else if (processes[running_process].getTimeRemaining() == 0) {
				int IOBurst = randomOS(processes[running_process].getIOtime());
				processes[running_process].setTotalIO(IOBurst);
				processes[running_process].setTimeRemaining(IOBurst);
				processes[running_process].setCurrent_state("blocked");
				bool inBlocked = false;
				//blocked_process.push_back(running_process);
				for (int i = 0; i < blocked_process.size(); i++) {
					if (processes[running_process].getArrivalTiebreak() < processes[blocked_process[i]].getArrivalTiebreak()) {
						blocked_process.insert(blocked_process.begin() + i, running_process);
						inBlocked = true;
						break;
					}

					if (processes[running_process].getArrivalTiebreak() == processes[blocked_process[i]].getArrivalTiebreak()) {
						cout << "Arrival tie breaker match" << endl;
					}
				}

				if (!inBlocked) {
					blocked_process.push_back(running_process);
				}
				running_process = -1;
			}
		}

		//cout << " before if block" << endl;

		if (running_process == -1 && !ready_process.empty()) {

			
			//double penaltyRatio = 0.0;
			int readyDequeIndex = 0;

			if (ready_process.size() > 1) {

				// for (int i = 0; i < ready_process.size(); i++) {
				// 	cout << "process " << i << " = " << ready_process[i] << endl;
				// 	cout << "stats: " << endl;
				// 	cout << "cpu cycles " << processes[ready_process[i]].getCPUCycles() << endl;
				// 	cout << "arrival " << processes[ready_process[i]].getArrival() << endl;
				// }

				//std::setprecision(20);
				
				double penaltyRatio = (cycleNum * 1.0 - processes[ready_process[0]].getArrival()) / (1.0 * max(1, processes[ready_process[0]].getCPUCycles()));



				for (int i = 1; i < ready_process.size(); i++) {
				
					if ((processes[ready_process[readyDequeIndex]].getCPUCycles() == processes[ready_process[i]].getCPUCycles()) 
						&& (processes[ready_process[readyDequeIndex]].getArrival() == processes[ready_process[i]].getArrival())) {

						if ((processes[ready_process[readyDequeIndex]].getArrivalTiebreak()) > processes[ready_process[i]].getArrivalTiebreak()) {
							penaltyRatio = ((cycleNum * 1.0 - processes[ready_process[i]].getArrival()) / (1.0 * max(1, processes[ready_process[i]].getCPUCycles())));
							readyDequeIndex = i;
							continue;
						}

					}
					
					else if (((cycleNum * 1.0 - processes[ready_process[i]].getArrival()) / 
						(1.0 * (max(1, processes[ready_process[i]].getCPUCycles())))) > penaltyRatio) {
						penaltyRatio = ((cycleNum * 1.0 - processes[ready_process[i]].getArrival()) / (1.0 * max(1, processes[ready_process[i]].getCPUCycles())));
						readyDequeIndex = i;
					}
				}

				running_process = ready_process[readyDequeIndex];

			}


			else {
				
		
				running_process = ready_process.front();
			}

		

			ready_process.erase(ready_process.begin() + readyDequeIndex);	


			processes[running_process].setTimeRemaining(randomOS(processes[running_process].getCPUburst()));
			processes[running_process].setCurrent_state("running");

			
	
		} 


		for (int i = 0; i < ready_process.size(); i++) {
			processes[ready_process[i]].incrementWaiting();
		}

		if (running_process > -1) {
			CPUutilization++;
		}

		if (!blocked_process.empty()) {
			IOutilization++;
		}

		cycleNum++;

	} // end outer while loop


	cout << "The scheduling algorithm used was Highest Penalty Ratio Next" << endl;
	cout << '\n';
	// now print stats for each cycle.

	int totalTurnaround = 0;
	int totalWait = 0;

	for (int i = 0; i < process_num; i++) {
		cout << "Process " << i << ":" << endl;
		cout << '\t' << "(A,B,C,IO) = (" << processes[i].getArrival() << "," << processes[i].getCPUburst() << "," << processes[i].getCPUTime() << "," << processes[i].getIOtime() << ")" << endl;
		cout << '\t' << "Finishing time: " << processes[i].getFinishingTime() << endl;;
		cout << '\t' << "Turnaround time: " << processes[i].getTurnaroundTime() << endl;
		cout << '\t' << "I/O time: " << processes[i].getTotalIO() << endl;
		cout << '\t' << "Waiting time: " << processes[i].getWaitingTime() << endl;
		cout << '\n';

		totalTurnaround += processes[i].getTurnaroundTime();
		totalWait += processes[i].getWaitingTime();
	}

	// now print Summary Data 

	cout << "Summary Data: " << endl;
	cout << '\t' << "Finishing Time: " << cycleNum - 1 << endl;
	cout << '\t' << "CPU Utilization: " << ((double) CPUutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "I/O Utilization: " << ((double) IOutilization/(cycleNum - 1)) << endl;
	cout << '\t' << "Throughput: " << (process_num * ((double) 100 / (cycleNum - 1))) << endl;
	cout << '\t' << "Average turnaround time: " << ((double) totalTurnaround/process_num) << endl;
	cout << '\t' << "Average waiting time: " << ((double) totalWait / process_num) << endl;



	// reset everything: randomIndex, CPU utilization, IO utilization, etc. 

	randomIndex = -1;
	CPUutilization = 0;
	IOutilization = 0;

} // end hprn


int main(int argc, char* argv[]) {

	if (argc > 1){
		isVerbose = true;
	}

	fileReader();

	if (processes.size() > 1){
		sortProcesses();


	}

	else{
		process temp(processes[0].getArrival(), processes[0].getCPUburst(), processes[0].getCPUTime(), processes[0].getIOtime(), 0, 0, 0, 0);
		presort.push_back(temp);
	}


	createRandomNumVector();




	 fcfs();
	 cout << '\n' << endl;
	 lcfs();
	 cout << '\n' << endl;
	 roundRobin();
	 cout << '\n' << endl;
	 hprn();

	return 0;
}





