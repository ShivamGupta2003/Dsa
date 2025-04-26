#include <bits/stdc++.h>
using namespace std;

struct Process
{
    int pid;              // Process ID
    int arrival_time;     // Arrival Time
    int burst_time;       // Burst Time
    int remaining_time;   // Remaining Burst Time (for RR, SRTF)
    int priority;         // Priority (for Priority Scheduling)
    int completion_time;  // Completion Time
    int turn_around_time; // Turn Around Time
    int waiting_time;     // Waiting Time
    int start_time;       // Start Time (for SJF, SRTF)
};

// Function prototypes
void FCFS(Process proc[], int n);
void SJF(Process proc[], int n);
void SRTF(Process proc[], int n);
void RoundRobin(Process proc[], int n, int quantum);
void PriorityScheduling(Process proc[], int n, bool preemptive);
void printResults(Process proc[], int n);

// Utility function to sort processes by arrival time
bool compareArrival(Process a, Process b)
{
    return a.arrival_time < b.arrival_time;
}

// Utility function to sort processes by burst time (for SJF)
bool compareBurst(Process a, Process b)
{
    return a.burst_time < b.burst_time;
}

// Utility function to sort processes by priority (lower number = higher priority)
bool comparePriority(Process a, Process b)
{
    return a.priority < b.priority;
}

// Utility function to print results
void printResults(Process proc[], int n)
{
    cout << "\nP#\tAT\tBT\tPri\tCT\tTAT\tWT\n";
    for (int i = 0; i < n; i++)
    {
        cout << "P" << proc[i].pid << "\t"
             << proc[i].arrival_time << "\t"
             << proc[i].burst_time << "\t"
             << proc[i].priority << "\t"
             << proc[i].completion_time << "\t"
             << proc[i].turn_around_time << "\t"
             << proc[i].waiting_time << "\n";
    }

    double total_tat = 0.0, total_wt = 0.0;
    for (int i = 0; i < n; i++)
    {
        total_tat += proc[i].turn_around_time;
        total_wt += proc[i].waiting_time;
    }

    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time = " << (total_tat / n) << endl;
    cout << "Average Waiting Time    = " << (total_wt / n) << endl;
}

// 1. First Come First Serve (FCFS)
void FCFS(Process proc[], int n)
{
    // Sort processes by arrival time
    sort(proc, proc + n, compareArrival);

    // Calculate completion times
    proc[0].completion_time = proc[0].arrival_time + proc[0].burst_time;
    for (int i = 1; i < n; i++)
    {
        proc[i].completion_time = max(proc[i - 1].completion_time, proc[i].arrival_time) + proc[i].burst_time;
    }

    // Calculate TAT and WT
    for (int i = 0; i < n; i++)
    {
        proc[i].turn_around_time = proc[i].completion_time - proc[i].arrival_time;
        proc[i].waiting_time = proc[i].turn_around_time - proc[i].burst_time;
    }
}

// 2. Shortest Job First (SJF) - Non-preemptive
void SJF(Process proc[], int n)
{
    sort(proc, proc + n, compareArrival);

    int current_time = 0;
    vector<bool> completed(n, false);
    int completed_count = 0;

    while (completed_count < n)
    {
        int shortest_index = -1;
        int shortest_burst = INT_MAX;

        // Find the process with shortest burst time that has arrived
        for (int i = 0; i < n; i++)
        {
            if (!completed[i] && proc[i].arrival_time <= current_time && proc[i].burst_time < shortest_burst)
            {
                shortest_burst = proc[i].burst_time;
                shortest_index = i;
            }
        }

        if (shortest_index == -1)
        {
            current_time++;
            continue;
        }

        // Execute the shortest process
        proc[shortest_index].completion_time = current_time + proc[shortest_index].burst_time;
        proc[shortest_index].turn_around_time = proc[shortest_index].completion_time - proc[shortest_index].arrival_time;
        proc[shortest_index].waiting_time = proc[shortest_index].turn_around_time - proc[shortest_index].burst_time;

        completed[shortest_index] = true;
        completed_count++;
        current_time = proc[shortest_index].completion_time;
    }
}

// 3. Shortest Remaining Time First (SRTF) - Preemptive
void SRTF(Process proc[], int n)
{
    sort(proc, proc + n, compareArrival);

    for (int i = 0; i < n; i++)
    {
        proc[i].remaining_time = proc[i].burst_time;
    }

    int current_time = 0;
    int completed_count = 0;
    vector<bool> completed(n, false);

    while (completed_count < n)
    {
        int shortest_index = -1;
        int shortest_remaining = INT_MAX;

        // Find process with shortest remaining time
        for (int i = 0; i < n; i++)
        {
            if (!completed[i] && proc[i].arrival_time <= current_time && proc[i].remaining_time < shortest_remaining)
            {
                shortest_remaining = proc[i].remaining_time;
                shortest_index = i;
            }
        }

        if (shortest_index == -1)
        {
            current_time++;
            continue;
        }

        // If this is the first time the process is getting CPU
        if (proc[shortest_index].remaining_time == proc[shortest_index].burst_time)
        {
            proc[shortest_index].start_time = current_time;
        }

        // Execute for 1 unit of time
        proc[shortest_index].remaining_time--;
        current_time++;

        // If process is completed
        if (proc[shortest_index].remaining_time == 0)
        {
            proc[shortest_index].completion_time = current_time;
            proc[shortest_index].turn_around_time = proc[shortest_index].completion_time - proc[shortest_index].arrival_time;
            proc[shortest_index].waiting_time = proc[shortest_index].turn_around_time - proc[shortest_index].burst_time;

            completed[shortest_index] = true;
            completed_count++;
        }
    }
}

// 4. Round Robin (RR)
void RoundRobin(Process proc[], int n, int quantum)
{
    sort(proc, proc + n, compareArrival);

    for (int i = 0; i < n; i++)
    {
        proc[i].remaining_time = proc[i].burst_time;
    }

    queue<int> ready_queue;
    int current_time = 0;
    int completed_count = 0;
    vector<bool> in_queue(n, false);

    // Push first process
    ready_queue.push(0);
    in_queue[0] = true;

    while (completed_count < n)
    {
        if (ready_queue.empty())
        {
            current_time++;
            // Check if new processes arrived
            for (int i = 0; i < n; i++)
            {
                if (!in_queue[i] && proc[i].arrival_time <= current_time && proc[i].remaining_time > 0)
                {
                    ready_queue.push(i);
                    in_queue[i] = true;
                    break;
                }
            }
            continue;
        }

        int current_index = ready_queue.front();
        ready_queue.pop();
        in_queue[current_index] = false;

        // Execute for quantum or remaining time, whichever is smaller
        int execution_time = min(quantum, proc[current_index].remaining_time);
        proc[current_index].remaining_time -= execution_time;
        current_time += execution_time;

        // Check for new arrivals during this execution
        for (int i = 0; i < n; i++)
        {
            if (i != current_index && !in_queue[i] && proc[i].arrival_time <= current_time && proc[i].remaining_time > 0)
            {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        // If process is not finished, put it back in queue
        if (proc[current_index].remaining_time > 0)
        {
            ready_queue.push(current_index);
            in_queue[current_index] = true;
        }
        else
        {
            // Process completed
            proc[current_index].completion_time = current_time;
            proc[current_index].turn_around_time = proc[current_index].completion_time - proc[current_index].arrival_time;
            proc[current_index].waiting_time = proc[current_index].turn_around_time - proc[current_index].burst_time;
            completed_count++;
        }
    }
}

// 5. Priority Scheduling (both preemptive and non-preemptive)
void PriorityScheduling(Process proc[], int n, bool preemptive)
{
    sort(proc, proc + n, compareArrival);

    if (!preemptive)
    {
        // Non-preemptive version
        int current_time = 0;
        vector<bool> completed(n, false);
        int completed_count = 0;

        while (completed_count < n)
        {
            int highest_priority_index = -1;
            int highest_priority = INT_MAX;

            // Find the process with highest priority that has arrived
            for (int i = 0; i < n; i++)
            {
                if (!completed[i] && proc[i].arrival_time <= current_time && proc[i].priority < highest_priority)
                {
                    highest_priority = proc[i].priority;
                    highest_priority_index = i;
                }
            }

            if (highest_priority_index == -1)
            {
                current_time++;
                continue;
            }

            // Execute the highest priority process
            proc[highest_priority_index].completion_time = current_time + proc[highest_priority_index].burst_time;
            proc[highest_priority_index].turn_around_time = proc[highest_priority_index].completion_time - proc[highest_priority_index].arrival_time;
            proc[highest_priority_index].waiting_time = proc[highest_priority_index].turn_around_time - proc[highest_priority_index].burst_time;

            completed[highest_priority_index] = true;
            completed_count++;
            current_time = proc[highest_priority_index].completion_time;
        }
    }
    else
    {
        // Preemptive version
        for (int i = 0; i < n; i++)
        {
            proc[i].remaining_time = proc[i].burst_time;
        }

        int current_time = 0;
        int completed_count = 0;
        vector<bool> completed(n, false);

        while (completed_count < n)
        {
            int highest_priority_index = -1;
            int highest_priority = INT_MAX;

            // Find process with highest priority
            for (int i = 0; i < n; i++)
            {
                if (!completed[i] && proc[i].arrival_time <= current_time && proc[i].priority < highest_priority)
                {
                    highest_priority = proc[i].priority;
                    highest_priority_index = i;
                }
            }

            if (highest_priority_index == -1)
            {
                current_time++;
                continue;
            }

            // If this is the first time the process is getting CPU
            if (proc[highest_priority_index].remaining_time == proc[highest_priority_index].burst_time)
            {
                proc[highest_priority_index].start_time = current_time;
            }

            // Execute for 1 unit of time
            proc[highest_priority_index].remaining_time--;
            current_time++;

            // If process is completed
            if (proc[highest_priority_index].remaining_time == 0)
            {
                proc[highest_priority_index].completion_time = current_time;
                proc[highest_priority_index].turn_around_time = proc[highest_priority_index].completion_time - proc[highest_priority_index].arrival_time;
                proc[highest_priority_index].waiting_time = proc[highest_priority_index].turn_around_time - proc[highest_priority_index].burst_time;

                completed[highest_priority_index] = true;
                completed_count++;
            }
        }
    }
}

int main()
{
    int n, choice, quantum;
    bool preemptive;

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
        cout << "Enter Priority for P" << proc[i].pid << " (lower number = higher priority): ";
        cin >> proc[i].priority;
    }

    cout << "\nScheduling Algorithms:\n";
    cout << "1. FCFS (First Come First Serve)\n";
    cout << "2. SJF (Shortest Job First - Non-preemptive)\n";
    cout << "3. SRTF (Shortest Remaining Time First - Preemptive)\n";
    cout << "4. Round Robin\n";
    cout << "5. Priority Scheduling (Non-preemptive)\n";
    cout << "6. Priority Scheduling (Preemptive)\n";
    cout << "Enter your choice (1-6): ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        FCFS(proc, n);
        cout << "\nFCFS Scheduling Results:\n";
        break;
    case 2:
        SJF(proc, n);
        cout << "\nSJF (Non-preemptive) Scheduling Results:\n";
        break;
    case 3:
        SRTF(proc, n);
        cout << "\nSRTF (Preemptive) Scheduling Results:\n";
        break;
    case 4:
        cout << "Enter time quantum for Round Robin: ";
        cin >> quantum;
        RoundRobin(proc, n, quantum);
        cout << "\nRound Robin Scheduling Results (Quantum = " << quantum << "):\n";
        break;
    case 5:
        PriorityScheduling(proc, n, false);
        cout << "\nPriority Scheduling (Non-preemptive) Results:\n";
        break;
    case 6:
        PriorityScheduling(proc, n, true);
        cout << "\nPriority Scheduling (Preemptive) Results:\n";
        break;
    default:
        cout << "Invalid choice!";
        return 1;
    }

    printResults(proc, n);
    return 0;
}