#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>

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
  int chromoLength;
  int numChromos;
  struct sigaction sa;/* Use the sa_sigaction field because the sigHandler has two additional parameters */
}chromosomeList;

typedef struct indexOffset{
  int B;
  int B_Index;
}indexOffset;
//------------------------------------------------------------------------------
// declare function prototypes
//------------------------------------------------------------------------------
static void flipBit(chromosome *c, indexOffset inOff, int index);
chromosomeList createChromosomeList(int chromoLength, int totalOrganisms);
static indexOffset findChromoOffsets(int index);
unsigned long getBits(int index, chromosome *c, int numBits);
void mutate(chromosome *c, int numChromos, indexOffset inOff);


static sigjmp_buf currentJump;
static void sigHandler(int signal, siginfo_t *info, void *context){
  longjmp(currentJump, 1);
}


/*******************************************************************************
* takes the number of bits for a chromosome and the number of chromosomes total
*******************************************************************************/
chromosomeList createChromosomeList(int chromoLength, int totalOrganisms){
  srand(time(NULL)); //seed random
  chromosomeList myChromosomeList;
  //allocate memory for each organism
  myChromosomeList.myChromosomes = malloc(sizeof(chromosome) * totalOrganisms);
  myChromosomeList.chromoLength = chromoLength;
  //figure out how many 'longs' needed per chromosome
  myChromosomeList.numChromos = chromoLength/(sizeof(unsigned long)*8) + 1;

  //allocate space for each organisms chromosome
  for(int x = 0; x < totalOrganisms; x++){
    myChromosomeList.myChromosomes[x].bits = malloc(sizeof(unsigned long) * myChromosomeList.numChromos);
  }
  myChromosomeList.sa.sa_sigaction = &sigHandler;/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  myChromosomeList.sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_NODEFER;
  sigaction(SIGSEGV, &myChromosomeList.sa, NULL); /*assign SIGSEGV to be handled by our sigHandler*/

  return myChromosomeList;
}


/*******************************************************************************
* finds a specific position in an unsigned long
*******************************************************************************/
static indexOffset findChromoOffsets(int index){
  int B_Index = index / (8 * sizeof(unsigned long) );
  int B = index % (8 * sizeof(unsigned long) );
  indexOffset returnVal;
  returnVal.B_Index = B_Index;
  returnVal.B = B;
  return returnVal;
}


/*******************************************************************************
* gets a specific bit set from a specified chromosome the index specifies the lowest
* bit in the sequence EX. index 4 and numBits 5 would give a copy of bits 4-8
*
* returns -1 if index is out of bounds
*******************************************************************************/
unsigned long getBits(int index, chromosome *c, int numBits){
  if(!setjmp(currentJump)){
    indexOffset inOff = findChromoOffsets(index);
    unsigned long temp = c->bits[inOff.B_Index] >> inOff.B;
    unsigned long compare = pow(2, numBits) -1;
    return (temp & compare);//doesn't shift bits back to position
  }else{
    fprintf(stderr, "index out of bounds or num bits invalid\n");
    exit(1);
  }

}


/*******************************************************************************
* attempts to mutate a chromosome's bits based on some small chance
*******************************************************************************/
void mutate(chromosome *c, int numChromos, indexOffset inOff){
  float multiplier = (numChromos * sizeof(unsigned long) * 8) / (float) MUTATION_RATE + 1;
  for(int x = 0; x < multiplier; x++){
    int value = rand() % MUTATION_RATE; //get a random value between 0 - 999
    value *= x;
    flipBit(c, inOff, value);

    //TODO do this a better way
  }

}


/*******************************************************************************
* flips a specified bit in a chromosome
*******************************************************************************/
static void flipBit(chromosome *c, indexOffset inOff, int index){
  if(!setjmp(currentJump)){
    findChromoOffsets(index); //TODO findChromoOffsets was edited change this
    c->bits[inOff.B_Index] = c->bits[inOff.B_Index] ^ (1 << inOff.B);
  }else{
    fprintf(stderr, "index out of bounds in flip bit call\n");
  }
}



/*******************************************************************************
* Main function for testing, requires preproccessor declaration in compilation
*******************************************************************************/
#ifdef UNIT_TEST
  void test_flipBit(chromosomeList *c){

  }

  void test_findChromoOffsets(chromosomeList *c){

  }

  void test_getBits(chromosomeList *c){
    c->myChromosomes[0].bits[1] = 1111;
    long temp;
    if((temp = getBits(66, &c->myChromosomes[0],5)) != 21){
      fprintf(stderr, "getBits should have returned 21, instead returned: %lu\n",temp);
    }

  }

  void test_mutate(chromosomeList *c){

  }

  int main(int argc, char *argv[]){
    chromosomeList myChromoList = createChromosomeList(100, 100);
    myChromoList.myChromosomes[0].bits[1] = 1111;
    test_getBits(&myChromoList);
    printf("%lu\n",getBits(66, &myChromoList.myChromosomes[0], 5));
    // printf("B_Index: %i\t B: %i\n",B_Index, B);
    //mutate(&myChromosomes[0]);
    //TODO change most of this it won't work
  }




#endif
