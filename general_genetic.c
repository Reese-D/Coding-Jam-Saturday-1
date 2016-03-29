#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define CROSSOVER_RATE            0.7
#define MUTATION_RATE             1000    //1 in 1000
#define MAX_ALLOWABLE_GENERATIONS	400
#define POP_SIZE                  100			//must be an even number

//TODO bring global variables into struct so that they can be passed through
//functions, allowing multiple instances to exist at once





//------------------------------------------------------------------------------
// structure that will define our chromosome for genetic algorithms
//------------------------------------------------------------------------------
typedef struct chromosome{
  unsigned long *bits;
  float fitness;
}chromosome;

typedef struct chromosomeList{
  chromosome *myChromosomes;
  int chromoLength; //the number of longs per chromosome
  int numChromos; //the number of chromosomes
  int pid;

}chromosomeList;

typedef struct indexOffset{
  int B;
  int B_Index;
}indexOffset;





//------------------------------------------------------------------------------
// declare function prototypes
//------------------------------------------------------------------------------
static int flipBit(chromosome *c, int index);
chromosomeList createChromosomeList(int chromoLength, int totalOrganisms);
static void findChromoOffsets(int index, indexOffset *returnVal);
unsigned long* getBits(int index, chromosome *c, int numBits, unsigned long* returnVal);
void mutate(chromosomeList *c, int numChromos);


static sigjmp_buf currentJump;
static void sigHandler(int signal, siginfo_t *info, void *context){
  if(signal == SIGSEGV || signal == SIGILL || signal == SIGUSR1) longjmp(currentJump, 1);
}
static struct sigaction sa;







/*******************************************************************************
* takes the number of bits for a chromosome and the number of chromosomes total
*******************************************************************************/
chromosomeList createChromosomeList(int chromoLength, int totalOrganisms){
  srand(time(NULL)); //seed random
  chromosomeList myChromosomeList;
  //allocate memory for each organism
  myChromosomeList.myChromosomes = malloc(sizeof(chromosome) * totalOrganisms);
  myChromosomeList.numChromos = totalOrganisms;
  //figure out how many 'longs' needed per chromosome
  myChromosomeList.chromoLength = chromoLength/(sizeof(unsigned long)*8) + 1;

  //allocate space for each organisms chromosome
  for(int x = 0; x < totalOrganisms; x++){
    myChromosomeList.myChromosomes[x].bits = malloc(sizeof(unsigned long) * chromoLength);
  }

  sa.sa_sigaction = &sigHandler;/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_NODEFER;
  return myChromosomeList;
}




/*******************************************************************************
* finds a specific position in an unsigned long
*******************************************************************************/
static void findChromoOffsets(int index, indexOffset *returnVal){
  if(index < 0){
    returnVal = NULL;
    return;
  }
  int B_Index = index / (8 * sizeof(unsigned long) );
  int B = index % (8 * sizeof(unsigned long) );
  returnVal->B_Index = B_Index;
  returnVal->B = B;
}




/*******************************************************************************
* gets a specific bit set from a specified chromosome the index specifies the lowest
* bit in the sequence EX. index 4 and numBits 5 would give a copy of bits 4-8
*
* returns -1 if index is out of bounds
*******************************************************************************/
unsigned long* getBits(int index, chromosome *c, int numBits, unsigned long* returnVal){
  sigaction(SIGSEGV, &sa, NULL); /*assign SIGSEGV to be handled by our sigHandler*/
  if(!setjmp(currentJump)){
    indexOffset *inOff = malloc(sizeof(indexOffset*));
    findChromoOffsets(index, inOff);
    if(inOff == NULL) return NULL;
    unsigned long temp = c->bits[inOff->B_Index] >> inOff->B;
    unsigned long compare = pow(2, numBits) -1;
    *returnVal =  (temp & compare);//doesn't shift bits back to position
    return returnVal;
  }else{
    return NULL;
  }
  signal(SIGSEGV, SIG_DFL);
}




/*******************************************************************************
* attempts to mutate a chromosome's bits based on some small chance
*******************************************************************************/
void mutate(chromosomeList *c, int numChromos){
  for(int x = 0; x < numChromos; x++){
    int mutType1 = rand() % MUTATION_RATE;
    int mutType2 = rand() % MUTATION_RATE;
    int temp = rand() % c->chromoLength;
    if(mutType1 == mutType2){
      int rightLeft = rand() % 2; //50 50 chance of left or right shift
      c->myChromosomes[x].bits[temp] = rightLeft ? c->myChromosomes[x].bits[temp] >> 1 : c->myChromosomes[x].bits[temp] << 1;
    }else if(mutType1 == mutType2 /2){
      c->myChromosomes[x].bits[temp] *= 2;
    }else{
      int totalBits = c->chromoLength * (sizeof(unsigned long)*8); //number of longs aka chromolength times numbits in each
      int mutRate = totalBits * MUTATION_RATE;
      for(int y = 0; y < totalBits; y++){
        int randNum = rand() % mutRate;
        if(randNum <= totalBits){ //1 in 1000 chance to occur each time around
          flipBit(&c->myChromosomes[randNum / sizeof(unsigned long) * 8], randNum % sizeof(unsigned long) * 8);
        }
      }
    }
  }
}




/*******************************************************************************
* flips a specified bit in a chromosome
*******************************************************************************/
static int flipBit(chromosome *c, int index){
  if(index < 0 || c == NULL) return -1;
  sigaction(SIGSEGV, &sa, NULL); /*assign SIGSEGV to be handled by our sigHandler*/
  if(!setjmp(currentJump)){
    indexOffset *inOff = malloc(sizeof(indexOffset*));
    findChromoOffsets(index, inOff);
    c->bits[inOff->B_Index] = c->bits[inOff->B_Index] ^ (1 << inOff->B);
  }else{
    return -1;
  }
  signal(SIGSEGV, SIG_DFL);
  return 1;
}




/*******************************************************************************
* Prints out chromosomes for testing and visualization
*******************************************************************************/
void printAllChromosomes(chromosomeList *c){
  for(int x = 0; x < c->numChromos; x++){
    printf("Chromosome: %i\t", x);
    for(int y = 0; y < c->chromoLength; y++){
      printf("%lu\t", c->myChromosomes[x].bits[y]);
    }
    printf("\n");
  }
}









/*******************************************************************************
* functions for testing, requires preproccessor declaration in compilation
*******************************************************************************/
#ifdef UNIT_TEST
  void test_flipBit(chromosomeList *c){
    c->myChromosomes[0].bits[1] = 1111;
    unsigned long *temp = malloc(sizeof(temp));
    flipBit(&c->myChromosomes[0], 67);
    if((*temp = *getBits(66, &c->myChromosomes[0],5,temp)) != 23){
      fprintf(stderr, "getBits should have returned 23, instead returned: %lu\n",*temp);
    }
    c->myChromosomes[55].bits[0] = 204;
    flipBit(&c->myChromosomes[55], 50);
    if((*temp = *getBits(0, &c->myChromosomes[55],2,temp)) != 0){
      fprintf(stderr, "getBits should have returned 0, instead returned: %lu\n",*temp);
    }
    flipBit(&c->myChromosomes[55], 2);
    if((*temp = *getBits(0, &c->myChromosomes[55],3,temp)) != 0){
      fprintf(stderr, "getBits should have returned 0, instead returned: %lu\n",*temp);
    }
    flipBit(&c->myChromosomes[55], 3);
    if((*temp = *getBits(3, &c->myChromosomes[55],4,temp)) != 8){
      fprintf(stderr, "getBits should have returned 8, instead returned: %lu\n",*temp);
    }

    if(flipBit(&c->myChromosomes[105], 3) != -1){
      printf("flip bit didn't catch an out of bounds call\n");
    }
    if(flipBit(&c->myChromosomes[55], -1) != -1){
      printf("flip bit didn't catch a negative index value\n");
    }

    if(flipBit(&c->myChromosomes[-1], 3) != -1){
      printf("flip bit didn't catch an out of bounds call in the negative direction\n");
    }


  }

  void test_findChromoOffsets(chromosomeList *c){

  }

  void test_getBits(chromosomeList *c){
    c->myChromosomes[0].bits[1] = 1111;
    unsigned long *temp = malloc(sizeof(unsigned long*));
    if((*temp = *getBits(66, &c->myChromosomes[0],5,temp)) != 21){
      fprintf(stderr, "getBits should have returned 21, instead returned: %lu\n",*temp);
    }
    c->myChromosomes[55].bits[0] = 204;
    if((*temp = *getBits(0, &c->myChromosomes[55],2,temp)) != 0){
      fprintf(stderr, "getBits should have returned 0, instead returned: %lu\n",*temp);
    }
    if((*temp = *getBits(0, &c->myChromosomes[55],3,temp)) != 4){
      fprintf(stderr, "getBits should have returned 4, instead returned: %lu\n",*temp);
    }
    if((*temp = *getBits(3, &c->myChromosomes[55],4,temp)) != 9){
      fprintf(stderr, "getBits should have returned 9, instead returned: %lu\n",*temp);
    }
  }

  void test_mutate(chromosomeList *c){

  }

  int main(int argc, char *argv[]){
    chromosomeList myChromoList = createChromosomeList(100, 100);
    myChromoList.myChromosomes[0].bits[1] = 1111;
    test_getBits(&myChromoList);
    test_flipBit(&myChromoList);
    chromosomeList myChromoList2 = createChromosomeList(200, 5);
    for(int x = 0; x < 5; x++){
      for(int y = 0; y < 4; y++){
          myChromoList2.myChromosomes[x].bits[y] = x*10 + y;
      }
    }
    printAllChromosomes(&myChromoList2);
  }




#endif
