#include <bits/stdc++.h>
using namespace std;

struct Process
{
    int pid; // Process ID
    int arrival_time;
    int burst_time;
    int completion_time;
    int turn_around_time;
    int waiting_time;
};

// 1. Function to calculate completion times
void findCompletionTime(Process proc[], int n)
{
    // For the first process
    proc[0].completion_time = proc[0].arrival_time + proc[0].burst_time;

    // For the rest of the processes
    for (int i = 1; i < n; i++)
    {
        // The process starts either when the previous one completes,
        // or at its arrival time if the CPU was idle.
        int start_time = max(proc[i - 1].completion_time, proc[i].arrival_time);
        proc[i].completion_time = start_time + proc[i].burst_time;
    }
}

// 2. Function to calculate turn around times
void findTurnAroundTime(Process proc[], int n)
{
    for (int i = 0; i < n; i++)
    {
        proc[i].turn_around_time = proc[i].completion_time - proc[i].arrival_time;
    }
}

// 3. Function to calculate waiting times
void findWaitingTime(Process proc[], int n)
{
    for (int i = 0; i < n; i++)
    {
        proc[i].waiting_time = proc[i].turn_around_time - proc[i].burst_time;
    }
}

// 4. Function to calculate and print average times
void printAverageTimes(Process proc[], int n)
{
    double total_ct = 0.0, total_tat = 0.0, total_wt = 0.0;

    for (int i = 0; i < n; i++)
    {
        total_ct += proc[i].completion_time;
        total_tat += proc[i].turn_around_time;
        total_wt += proc[i].waiting_time;
    }

    cout << fixed << setprecision(2);
    cout << "\nAverage Completion Time  = " << (total_ct / n) << endl;
    cout << "Average Turnaround Time  = " << (total_tat / n) << endl;
    cout << "Average Waiting Time     = " << (total_wt / n) << endl;
}

int main()
{
    int n;
    cout << "Enter the number of processes: ";
    cin >> n;

    Process proc[n];

    // Input processes
    for (int i = 0; i < n; i++)
    {
        proc[i].pid = i + 1;
        cout << "Enter Arrival Time for P" << proc[i].pid << ": ";
        cin >> proc[i].arrival_time;
        cout << "Enter Burst Time for P" << proc[i].pid << ": ";
        cin >> proc[i].burst_time;
    }

    // Sort processes by arrival time (FCFS requirement)
    sort(proc, proc + n, [](Process &a, Process &b)
         { return a.arrival_time < b.arrival_time; });

    // Calculate all required times
    findCompletionTime(proc, n);
    findTurnAroundTime(proc, n);
    findWaitingTime(proc, n);

    // Print table of results
    cout << "\nP#\tAT\tBT\tCT\tTAT\tWT\n";
    for (int i = 0; i < n; i++)
    {
        cout << "P" << proc[i].pid << "\t"
             << proc[i].arrival_time << "\t"
             << proc[i].burst_time << "\t"
             << proc[i].completion_time << "\t"
             << proc[i].turn_around_time << "\t"
             << proc[i].waiting_time << "\n";
    }

    // Print average times using the separate function
    printAverageTimes(proc, n);

    return 0;
}
