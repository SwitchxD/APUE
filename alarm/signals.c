#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


//Usage: ./signals

// Signal Handler
void SendSignal(int signal){

	if (signal == SIGALRM){
		printf("Ding!\n");
		wait(NULL);
		}

}

int main(){
	
	// initializing delay of 5 seconds
	int delay=5;
	
	// starting child process
	pid_t child = fork();

	if (child == 0){
	
		sleep(delay);
		kill(getppid(), SIGALRM);
		exit(0);
		}
	else{
		signal(SIGALRM, SendSignal);
		wait(NULL);
		}
		
    return 0;
}
