#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "../performance.h"

#define N 1000 // number of threads
#define M 10000 // number of iterations per thread
#define V 1 // value added to the balance by each thread at each iteration

unsigned long int shared_variable;
unsigned long int lista[N]; //

int n = N, m = M, v = V;

void* thread_work(void *arg) {
	int indice = *((int*) arg);
	
	int i;
	for (i = 0; i < m; i++){
		//shared_variable += v;
		lista[indice]+=v;//
	}
		

	return NULL;
}

int main(int argc, char **argv)
{
	if (argc > 1) n = atoi(argv[1]);
	if (argc > 2) m = atoi(argv[2]);
	if (argc > 3) v = atoi(argv[3]);
	shared_variable = 0;
//===
	timer t;
	begin(&t);
//=====
	
	int indexes[N];//
	
	printf("Going to start %d threads, each adding %d times %d to a shared variable initialized to zero...", n, m, v); fflush(stdout);
	pthread_t* threads = (pthread_t*)malloc(n * sizeof(pthread_t)); // also calloc(n,sizeof(pthread_t))
	int i;
	for (i = 0; i < n; i++){
		lista[i]=0;
		indexes[i]=i;  // cosi non rischio che passandogli l indirizzo di i gli venga cambiato il suo valore prima di fare quell thread perche index non la tocco
		if (pthread_create(&threads[i], NULL, thread_work, (void *) &indexes[i]) != 0) {  // i al posto di NULL
			fprintf(stderr, "Can't create a new thread, error %d\n", errno);
			exit(EXIT_FAILURE);
		}
	
	}
	printf("ok\n");


	printf("Waiting for the termination of all the %d threads...", n); fflush(stdout);
	for (i = 0; i < n; i++){
		pthread_join(threads[i], NULL);
		shared_variable+=lista[i];//

	}
		
	printf("ok\n");

	unsigned long int expected_value = (unsigned long int)n*m*v;
	printf("The value of the shared variable is %lu. It should have been %lu\n", shared_variable, expected_value);
	if (expected_value > shared_variable) {
		unsigned long int lost_adds = (expected_value - shared_variable) / v;
		printf("Number of lost adds: %lu\n", lost_adds);
	}

//===
	end(&t);
	printf("Tempo impiegato: %ld secondi\n", get_seconds(&t));////////////////
//===

    free(threads);


	return EXIT_SUCCESS;
}

