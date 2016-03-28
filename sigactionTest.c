#include "sigactionTest.h"
pid_t pid;

static void sigHandler(int signal, siginfo_t *info, void *context){
  if(signal == SIGSEGV || signal == SIGILL || signal == SIGUSR1) printf("pid: %i got signal\n", pid);
}


int doStuff(){

  pid = fork();
  if(!pid){
    struct sigaction sa;
    sa.sa_sigaction = &sigHandler;/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_NODEFER;
    sigaction(SIGSEGV, &sa, NULL); /*assign SIGSEGV to be handled by our sigHandler*/
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    raise(SIGUSR1);
    exit(0);
  }
  if(pid){
    waitpid(pid, NULL, 0);
  }
  return 0;
}
