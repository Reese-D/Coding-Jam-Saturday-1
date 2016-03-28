#include "sigactionTest.h"

int main(int argc, char **argv){
  int x = doStuff();
  raise(SIGUSR1);
  raise(SIGUSR1);
  raise(SIGUSR1);
  raise(SIGUSR1);
  raise(SIGUSR1);
  raise(SIGUSR1);
  return 0;
}
