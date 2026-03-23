#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define     MAX_LINE_LENGTH     4096


/*
 * A structure to store lines and a count.
 * This will be useful when spawning the threads.
 */

struct lines_and_count {
    char    **lines;
    int     num_lines;
};


/*
 * TODO: define your global locks here.
 */
int remaining_threads = 0;
pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * Merge sort the lines.
 */
void merge_sort(char **lines, int num_lines);
void *merge_sort_thread(void *arg);

/*
 * Merge two sorted subsets of lines.
 * The first subset of lines is from indices 0 to num_lines1-1.
 * The second is from num_lines-1 to (num_lines1 + num_lines2 - 1)
 */
void merge(char **lines, int num_lines1, int num_lines2);


/*
 * Reads lines in a file into an array of char *'s.
 * The array is terminated by a NULL pointer.
 */
char **read_lines(char *path);

/*
 * Frees an array of char *'s.
 * Assumes that the array is terminated by a NULL pointer.
 */
void free_lines(char **lines);


/*
 * Main method
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s file num_threads\n", argv[0]);
        exit(1);
    }
    char **lines = read_lines(argv[1]);
    // int max_threads = atoi(argv[2]) - 1;
    int num_threads = atoi(argv[2]);
    if (num_threads < 1) {
    fprintf(stderr, "num_threads has to be 1> \n");
    exit(1);
    }
    remaining_threads = num_threads - 1;

    /* TODO: initialize your locks here */

    /* count the number of lines */
    int n = 0;
    for (n = 0; lines[n] != NULL; n++)
        ;

    merge_sort(lines, n);

    char **lp;
    for (lp = lines; *lp != NULL; lp++) {
        printf("%s", *lp);
    }
    
    free_lines(lines);
    return 0;
}

// merge sort thread function
void *merge_sort_thread(void *arg) {
    struct lines_and_count *data = (struct lines_and_count *)arg;
    merge_sort(data->lines, data->num_lines);
    free(data);
    return NULL;
}

/*
 * Merge sort the lines.
 * TODO: fixme!
 */
void merge_sort(char **lines, int num_lines) {
    if (num_lines <= 1) {
        return;
    }
    
    // split the lines into two halves
    int left_count = num_lines / 2;
    int right_count = num_lines - left_count;       // to handle odd number of lines

    // create threads for left and right halves if there are remaining threads available
    pthread_t left_thread, right_thread;
    int left_appear = 0;
    int right_appear = 0;

    // lock the thread lock to check and update the remaining threads count
    pthread_mutex_lock(&thread_lock);
    if (remaining_threads > 0) {
        remaining_threads--;
        left_appear = 1;
    }
    pthread_mutex_unlock(&thread_lock);

    // if left_appear is true, create a thread for the left half, otherwise sort it in the current thread
    if (left_appear) {
        struct lines_and_count *left_args = malloc(sizeof(struct lines_and_count));
        left_args->lines = lines;
        left_args->num_lines = left_count;
        pthread_create(&left_thread, NULL, merge_sort_thread, left_args);
    } else {
        merge_sort(lines, left_count);
    }
    
    // lock the thread lock again to check and update the remaining threads count for the right half
    pthread_mutex_lock(&thread_lock);
    if (remaining_threads > 0) {
        remaining_threads--;
        right_appear = 1;
    }
    pthread_mutex_unlock(&thread_lock);

    // if right_appear is true, create a thread for the right half, otherwise sort it in the current thread
    if (right_appear) {
        struct lines_and_count *right_args = malloc(sizeof(struct lines_and_count));
        right_args->lines = lines + left_count;
        right_args->num_lines = right_count;
        pthread_create(&right_thread, NULL, merge_sort_thread, right_args);
    } else {
        merge_sort(lines + left_count, right_count);
    }

    // wait for the left thread to finish if they were created
    if (left_appear) {
        pthread_join(left_thread, NULL);
        pthread_mutex_lock(&thread_lock);
        remaining_threads++;
        pthread_mutex_unlock(&thread_lock);
    }
    // wait for the right thread to finish if it was created
    if (right_appear) {
        pthread_join(right_thread, NULL);
        pthread_mutex_lock(&thread_lock);
        remaining_threads++;
        pthread_mutex_unlock(&thread_lock);
    }

    merge(lines, left_count, right_count);
}



/*
 * Merge two sorted subsets of lines.
 * The first subset of lines is from indices 0 to num_lines1-1.
 * The second is from num_lines-1 to (num_lines1 + num_lines2 - 1)
 */
void merge(char **lines, int num_lines1, int num_lines2) {
    char    **lp1 = lines,
            **lp2 = lines + num_lines1,
            **ep1 = lp1 + num_lines1,
            **ep2 = lp2 + num_lines2,
            **rp,
            **result;

    result = malloc(sizeof(*result) * (num_lines1 + num_lines2));
    rp = result;
    /* loop while there are lines remaining in either segment */
    while (lp1 < ep1 && lp2 < ep2) {
        if (strcmp(*lp1, *lp2) < 0) {   /* line in first segment is first */
            *(rp++) = *(lp1++);
        } else {                        /* line in second segment is first */
            *(rp++) = *(lp2++);
        }
    }
    /* tack on lines remaining in first segment */
    while (lp1 < ep1) {
        *(rp++) = *(lp1++);
    }
    /* tack on lines remaining in second segment */
    while (lp2 < ep2) {
        *(rp++) = *(lp2++);
    }
    assert(rp - result == (num_lines1 + num_lines2));
    memcpy(lines, result, sizeof(*lines) * (num_lines1 + num_lines2));
    free(result);
}


/*
 * Reads lines in a file into an array of char *'s.
 * The array is terminated by a NULL pointer.
 */
char **read_lines(char *path) {
    int     num_line_spaces = 2,    /* number of allocated spaces */
            num_lines = 0;          /* number of used spaces */
    char    **lines,
            buff[MAX_LINE_LENGTH];
    FILE    *fp;

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "couldn't open %s: %s\n", path, strerror(errno));
        exit(1);
    }
    
    lines  = malloc(num_line_spaces * sizeof(*lines));
    while (fgets(buff, MAX_LINE_LENGTH, fp) != NULL) {
        if (num_lines + 1 == num_line_spaces) {
            num_line_spaces *= 2;
            lines = realloc(lines, num_line_spaces * sizeof(*lines));
        }
        lines[num_lines++] = strdup(buff);
    }
    lines[num_lines] = NULL;
    if (!feof(fp)) {
        fprintf(stderr, "read of %s failed: %s\n", path, strerror(errno));
        exit(1);
    }
    return lines;
}


/*
 * Frees an array of char *'s.
 * Assumes that the array is terminated by a NULL pointer.
 */
void free_lines(char **lines) {
    char **lp;

    for (lp = lines; *lp != NULL; lp++) {
        free(*lp);
    }
    free(lines);
}
