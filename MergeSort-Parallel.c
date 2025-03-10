#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIZE 1000000  


// Function that we will use to merge the two arrays together
 
void merge(int arr[], int left, int mid, int right) 
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // We used malloc to avoid stack overflow and allocate memory dynamically

    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) 
    {
        if (L[i] <= R[j]) 
        {
            arr[k] = L[i++];
        } else 
        {
            arr[k] = R[j++];
        }
        k++;
    }

    while (i < n1) 
    {
        arr[k++] = L[i++];
    }

    while (j < n2) 
    {
        arr[k++] = R[j++];
    }

   
    // Free dynamically allocated memory
    free(L);
    free(R);
}

// Main function where merge sort will occur using recursion

void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

       
        if (right - left > 1000) {
            #pragma omp task
            mergeSort(arr, left, mid);

            #pragma omp task
            mergeSort(arr, mid + 1, right);
        } else {
            mergeSort(arr, left, mid);
            mergeSort(arr, mid + 1, right);
        }

        // Ensure both tasks finish before merging

        #pragma omp taskwait  
        merge(arr, left, mid, right);
    }
}


int main() 
{
    int arr[SIZE];
    srand(time(NULL));

    omp_set_num_threads(4);


    // Parallelizing the array initialization using OpenMP
    // usinig static loop scheduling
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < SIZE; i++)
        arr[i] = rand() % 10000;

    double start_time, end_time;
    int temp[SIZE];

    #pragma omp parallel
    {
        #pragma omp for
        for (int j = 0; j < SIZE; j++)
            temp[j] = arr[j];

        #pragma omp single
        {
            start_time = omp_get_wtime();
            mergeSort(temp, 0, SIZE - 1);
            end_time = omp_get_wtime();
        }
    }

    printf("Execution time (Parallel): %f seconds\n", end_time - start_time);

    return 0;
    
}
