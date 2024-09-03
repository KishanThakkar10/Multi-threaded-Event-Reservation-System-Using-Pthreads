#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <semaphore.h>
#include <vector>
#include <set>
using namespace std;

int s, e, MAX, T, c; // Global variables remain unchanged

sem_t max_query_semaphore;
pthread_mutex_t shared_table_mutex;
set<vector<int>> shared_table;
vector<int> events;

// Function to check availability for read queries
bool check_read_availability(int event_number, int query_type, int thread_id){
    pthread_mutex_lock(&shared_table_mutex);    

    if(shared_table.size() == MAX){
        cout << "[Thread " << thread_id << "] Error: Active queries limit exceeded for read." << endl;
        return false;
    }
    
    bool can_make_query = true;

    for(auto q: shared_table){
        if(q[0] == event_number && q[1] != 0){
            can_make_query = false;
            cout << "[Thread " << thread_id << "] Error: Conflicting entry already present in shared table for read, event " << event_number << "." << endl;
            break;
        }
    }
    if(can_make_query){
        shared_table.insert({event_number, query_type, thread_id});
    }

    pthread_mutex_unlock(&shared_table_mutex);
    return can_make_query;
}

// Function to check availability for write queries
bool check_write_availability(int event_number, int query_type, int thread_id){
    pthread_mutex_lock(&shared_table_mutex);

    if(shared_table.size() == MAX) {
        cout << "[Thread " << thread_id << "] Error: Active queries limit exceeded for write." << endl;
        return false;
    }

    bool can_make_query = true;

    for(auto q: shared_table){
        if(q[0] == event_number){
            can_make_query = false;
            cout << "[Thread " << thread_id << "] Error: Conflicting entry already present in shared table for write, event " << event_number << "." << endl;
            break;
        }
    }
    if(can_make_query){
        shared_table.insert({event_number, query_type, thread_id});
    }

    pthread_mutex_unlock(&shared_table_mutex);
    return can_make_query;
}

// Function to remove entry from shared table
void remove_entry_in_shared_table(int event_number, int query_type, int thread_id){
    pthread_mutex_lock(&shared_table_mutex);
    
    shared_table.erase({event_number, query_type, thread_id});
    
    pthread_mutex_unlock(&shared_table_mutex);
}

// Function to book tickets
void book_ticket(int event_no, int thread_id, vector<vector<int>> &booked_seats_details){
    int k = rand()%6 + 5;
    int available_seats = events[event_no];
    if(k > available_seats){
        cout << "[Thread " << thread_id << "] Error: Unable to book ticket. Only " << available_seats << " seats available for event " << event_no << " (requested " << k << ")." << endl;
        return;
    }
    events[event_no] -= k;
    booked_seats_details.push_back({thread_id, event_no, k});
    cout << "[Thread " << thread_id << "] Success: Booked " << k << " seats for event " << event_no << "." << endl;
}

// Function to cancel tickets
void cancel_ticket(int thread_id, vector<vector<int>> &booked_seats_details){
    if(booked_seats_details.size() == 0){
        cout << "[Thread " << thread_id << "] No existing booking available." << endl;
        return;
    }
    int index = rand() % booked_seats_details.size();
    events[booked_seats_details[index][1]] += booked_seats_details[index][2];
    cout << "[Thread " << booked_seats_details[index][0] << "] Success: Cancelled " << booked_seats_details[index][2] << " seats for event " << booked_seats_details[index][1] << "." << endl;
    booked_seats_details.erase(booked_seats_details.begin() + index);
}

// Function to perform main functionality based on query type
void main_functionality(int event_number, int query_type, int thread_id, vector<vector<int>> &booked_seats_details){
    if(query_type == 0){
        cout << "[Thread " << thread_id << "] Info: Available seats " << events[event_number] << " for event " << event_number << "." << endl;
    } else if (query_type == 1){
        book_ticket(event_number, thread_id, booked_seats_details);
    } else {
        cancel_ticket(thread_id, booked_seats_details);
    }
}

// Function to generate queries
void make_query(int thread_id, vector<vector<int>> &booked_seats_details){
    int event_number, query_type;
    event_number = rand() % e;
    query_type = rand() % 3;

    cout << "[Thread " << thread_id << "] Info: Made query " << query_type << " for event " << event_number << "." << endl;
    
    bool can_make_query = false;
    if(query_type == 0){
        can_make_query = check_read_availability(event_number, query_type, thread_id);
    } else {
        can_make_query = check_write_availability(event_number, query_type, thread_id);
    }
    sleep((rand() % 5) + 1);
    if(can_make_query){
        main_functionality(event_number, query_type, thread_id, booked_seats_details);
        remove_entry_in_shared_table(event_number, query_type, thread_id);
    }
}

// Function executed by threads
void *thread_function(void *t_id) {
    time_t start = time(NULL), secs = T;
    int *thread_id = (int *)t_id;

    srand((unsigned)time(NULL) + *thread_id + 1);

    vector<vector<int>> booked_seats_details;

    while (time(NULL) - start < secs) {
        cout << "[Thread " << *thread_id << "] Info: Waiting in query queue" << endl;
        sem_wait(&max_query_semaphore);

        make_query(*thread_id, booked_seats_details);

        sem_post(&max_query_semaphore);
        sleep((rand() % 4) + 1);
    }

    free(t_id);
    pthread_exit(NULL);
}

int main() {

    cout << "Enter the number of events: ";
    cin >> e;
    cout << "Enter the capacity of auditorium: ";
    cin >> c;
    cout << "Enter the number of worker threads: ";
    cin >> s;
    cout << "Enter the maximum number of concurrent active queries: ";
    cin >> MAX;
    cout << "Enter the total running time in seconds: ";
    cin >> T;

    events.resize(e, c);

    pthread_t threads[s];
    srand(time(NULL));

    sem_init(&max_query_semaphore, 0, MAX);
    pthread_mutex_init(&shared_table_mutex, NULL);
    for (int i = 0; i < s; i++) {
        int *thread_id = (int *)malloc(sizeof(int));
        *thread_id = i;
        if (pthread_create(&threads[i], NULL, thread_function, thread_id)) {
            cout << "Error while creating threads." << endl;
            exit(-1);
        }
    }

    for (int i = 0; i < s; i++) {
        pthread_join(threads[i], NULL);
    }
    cout << "All threads have finished." << endl;

    cout << "Booked seats for Events" << endl;
    for (int i = 0; i < events.size(); i++) {
        cout << "Event " << i << ", booked seats " << c - events[i] << endl;
    }

    sem_destroy(&max_query_semaphore);
    pthread_mutex_destroy(&shared_table_mutex);

    return 0;
}
