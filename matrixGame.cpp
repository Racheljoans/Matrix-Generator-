#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <thread>

using namespace std;
pthread_mutex_t mutex;
bool win = false;
bool sorted = false;
//tokenize function
void tokenize(char *input,int &argc,  char **argv){
    //checks for the spaces in the user input
    argc = 0;
    char *token = strtok(input, " ");
    while (token != NULL ) {
        //each token is stored in argv array
        *argv++ = token;
        argc++;
        //calls again with null argument to get next token
        token = strtok(NULL, " ");
    }
    *argv = NULL;
}
//define matrix size in struct
typedef struct myStruct_
{
    int Name;
    int Id;
    int* Arr;
} myStruct;
int count_neighbors(int arr[], int i, int j, int n) {
    int count1 = 0;
    int count0 = 0;
    int count = 0;
    //counts only the int directly around [i][j]
    for (int k = -1; k <= 1; k++) {
        for (int l = -1; l <= 1; l++) {
            int x = i + k;
            int y = j + l;
            if (x >= 0 && x < n && y >= 0 && y < n && (k != 0 || l != 0)) {
                if(arr[y*n+x] == 0){
                    count0++;
                }
                else{
                    count1++;
                }

            }
        }
    }
    if (count0> count1){
        count = 0;
    }
    else if( count0 < count1){
        count =1;
    }
    else{
        count = 2;
    }
    return count;
}
//all created pthreads for Problem 1 will be running here
void *Supervisor(void *arg) {
    myStruct * new_arg = (myStruct *)arg;
    int n = new_arg->Id;
    int* arr = new_arg->Arr;
    while (!win) {
        int count = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (arr[i * n + j] == 0) {
                    count++;
                }
            }
        }
        if (count == 0 || count == n * n) {
            win = true;
        }
    }
    return (void*)0;
}
void *Supervisor2(void *arg) {
    myStruct * new_arg = (myStruct *)arg;
    int n = new_arg->Id;
    int* arr = new_arg->Arr;
    bool l = true;
    for (int k = 0; k < n-1 ; k++) {
        if (arr[k] > arr[k + 1]) {
            l = false;
        }
    }
    sorted = l;
    return (void*)0;
}

void *worker_thread(void *arg) {
    //gets values specified for array and n
    myStruct * new_arg = (myStruct *)arg;
    int* arr = new_arg->Arr;
    int n = new_arg->Id;
    //while loop until all values are 0 or 1
        int x = rand() % n;
        int y = rand() % n;
        pthread_mutex_lock(&mutex);

        //should return 0, 1, or 2 based on more 0's more 1's or even
        int neighbors = count_neighbors(arr, x, y, n);
        //checks for majority to change arr[x*n+y]
        if (neighbors == 0){
            arr[x*n +y] = 0;
        }
        else if(neighbors == 1){
            arr[x*n+y]=1;
        }

        //reprints the matrix
        if (!win) {

            cout << y << ", " << x << "\n";
            cout<<"this is thread: ["<< new_arg->Name<<"]\n";
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    //index = i*n+j;
                    cout << "[" << arr[i * n + j] << "]";
                }
                cout << "\n";
            }


        //checks if all elements are 0's or 1's to end the while loop
    }
    pthread_mutex_unlock(&mutex);
    return (void*)0;
}
void *worker_thread2(void *arg) {

    myStruct * new_arg = (myStruct *)arg;
    int n = new_arg->Id;
    int* arr = new_arg->Arr;

    //create random i 0<=i<n

    //loop to sort the array

    while(!sorted) {
        int temp;
        int x = rand() % n ;
        temp = arr[n-1];
        arr[n-1]= arr[x];
        arr[x] = temp;
        int key = arr[n-1];
        if(!sorted) {
            pthread_mutex_lock(&mutex);
            cout << "this is thread: [" << new_arg->Name << "]\n";
            cout << "index i : " << x << " \n";
            int start = 0;
            int end = n - 2;

            while (start < end) {
                if (arr[start] < arr[n - 1]) {
                    start++;
                } else if (arr[end] > arr[n - 1]) {
                    end--;
                } else if (arr[start] > arr[n - 1]) {
                    temp = arr[start];
                    arr[start] = arr[end];
                    arr[end] = temp;
                    start++;
                    end--;
                }
            }
            for (int i = 0; i<n ; i++) {
                if (arr[n - 1] < arr[i]) {
                    temp = arr[n - 1];
                    arr[n - 1] = arr[i];
                    arr[i] = temp;
                }
            }
        }
        for (int i = 0; i < n; i++) {
            cout << "[" << arr[i] << "]";
        }
            cout << "\n";
        pthread_mutex_unlock(&mutex);
        sleep(.00000002);
        }


    return (void*)0;
}
//Problem 1 function
void Problem1(int n, int argc, char **argv, char *input){
    // create the square matrix with ones and zeros
    int arr[n*n];
    int index;
    win = false;
    //populates the array randomly
    for (int i = 0; i<n; i++){
        for (int j = 0; j<n; j++){
            index = i*n+j;
           arr[index] = rand() % 2;
        }
    }
    //prints the array
    for (int i = 0; i<n; i++){
        for (int j =0; j<n;j++){
            index = i*n+j;
            cout <<"["<<arr[index]<<"]";
        }
        cout<<"\n";
    }
    //getting user input for thread count
    cout<< "how many threads would you like to create: ";
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    tokenize(input, argc, argv);
    int test;
    try{
        test = stoi(input);
        if (test <1){
            cout<<"The input must be a positive number greater than 0.\n";
            exit(0);
        }
    }
    catch(...){
        cout<<"The input must be a positive number.\n";
        exit(0);
    }
    int m = stoi(input);
    //getting struct variable initialized with the pthread
    pthread_t threads[m];
    pthread_t thread_Supervisor;


    // create one pthread just to check values

    //getting pthreads to be created m times
    //pthreads_create[pointer to thread id,
    //pointer to pthread_attr_t data type(Null set this to default treat attributes,
    //pointer to function to be executed by new thread,
    //argument to be passed to start_routine function
    while (!win) {
        for (int i = 0; i < m; i++) {
            myStruct *v = (myStruct *) malloc(sizeof(myStruct));
            v->Id = n;
            v->Arr = arr;
            v->Name = i;
            pthread_create(&threads[i], NULL, &worker_thread, (void *) v);
            pthread_create(&thread_Supervisor, NULL, &Supervisor, (void *) v);
        }

        //join the threads ?
        for (int i = 0; i < m; i++) {
            pthread_join(threads[i], NULL);
        }
    }
}
void Problem2(int n, int argc, char **argv, char *input){
    //take user input for threads
    int arr[n];
    sorted = false;
    for (int i = 0; i<n; i++){
            arr[i] = rand() ;
    }
    //get from the user the amount of inputs
    cout<< "how many threads would you like to create: ";
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    tokenize(input, argc, argv);
    int test;
    try{
        test = stoi(input);
        if (test <1){
            cout<<"The input must be a positive number greater than 0.\n";
            exit(0);
        }
    }
    catch(...){
        cout<<"The input must be a positive number.\n";
        exit(0);
    }
    int m = stoi(input);
    pthread_t thread[m];
    pthread_t thread_Supervisor2;
    //create and use instances of myStruct
   // while(!sorted) {
        for (int x = 0; x < m; x++) {
            myStruct *v = (myStruct *) malloc(sizeof(myStruct));
            v->Id = n;
            v->Arr = arr;
            v->Name = x;
            pthread_create(&thread[x], NULL, &worker_thread2, (void *) v);
            pthread_create(&thread_Supervisor2, NULL, &Supervisor2, (void *) v);
        }
        //create m number of worker threads

        for (int i = 0; i < m; i++) {
            pthread_join(thread[i], NULL);
        }
    }

int main(int argc, char *argv[]){
    char input[100];
    if (argc > 2){
        cout<<"Please input only one number\n";
        exit(1);
    }
    if (argc == 1){
        cout<<"Please provide number of elements\n";
        exit(1);
    }
    int N;
    int test;
    bool play = true;
    try {
        N = stoi(argv[1]);

    }
    catch(...){
        cout << "You have to enter a number\n";
        exit(1);
    }

    //get the user input for problem 1 or problem 2
    while (play) {
        cout << "1 Problem 1\n2 Problem 2\n3 exit ";
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        tokenize(input, argc, argv);
        try {
            test = stoi(input);
            if (test > 3 || test < 1) {
                cout << "You have to enter a 1, 2, or 3\n";
                exit(0);
            }
        }
        catch(...){
            cout << "You have to enter a 1, 2, or 3\n";
            exit(1);
        }

        if (strcmp(input, "1") == 0) {
            Problem1(N, argc, argv, input);
        } else if (strcmp(input, "2") == 0) {
            Problem2(N, argc, argv, input);
        }
        else if (strcmp(input, "3") == 0){
            cout<<"goodbye :( \n";
            play = false;
            exit(0);
        }
    }
    return 0;
}