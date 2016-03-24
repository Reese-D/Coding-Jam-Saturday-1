#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define CROSSOVER_RATE            0.7
#define MUTATION_RATE             1000    //1 in 1000
#define MAX_ALLOWABLE_GENERATIONS	400
#define POP_SIZE                  100			//must be an even number

int Chromo_Length = 300, Gene_Length = 4;
int B_Index, B;
float Total_Fitness = 0.0f;
int Num_Chromos = 1;


//------------------------------------------------------------------------------
// structure that will define our chromosome for genetic algorithms
//------------------------------------------------------------------------------
typedef struct chromosome{
  unsigned long *bits;
  float fitness;
}chromosome;
chromosome *myChromosomes;

//------------------------------------------------------------------------------
// declare function prototypes
//------------------------------------------------------------------------------
static void flipBit(chromosome *c, int index);
void initialize(int bitsPerOrganism, int totalOrganisms);
static int findChromoOffsets(int index);
unsigned long getBits(int index, chromosome *c, int numBits);
void mutate(chromosome *c);


/*******************************************************************************
* takes the number of bits for a chromosome and the number of chromosomes total
*******************************************************************************/
void initialize(int bitsPerOrganism, int totalOrganisms){
  srand(time(NULL)); //seed random
  Chromo_Length = bitsPerOrganism;
  myChromosomes = malloc(sizeof(chromosome) * totalOrganisms);
  Num_Chromos = bitsPerOrganism/(sizeof(unsigned long)*8) + 1;
  for(int x = 0; x < totalOrganisms; x++){
    myChromosomes[x].bits = malloc(sizeof(unsigned long) * Num_Chromos);
  }
}


/*******************************************************************************
* finds a specific position in an unsigned long and sets the global variables
* to these values, and checks whether values are in bounds
*******************************************************************************/
static int findChromoOffsets(int index){
  B_Index = index / (8 * sizeof(unsigned long) );
  B = index % (8 * sizeof(unsigned long) );
  return B_Index > Num_Chromos - 1;
}


/*******************************************************************************
* gets a specific bits from a specified chromosome the index specifies the lowest
* bit in the sequence EX. index 4 and numBits 5 would give a copy of bits 4-8
*
* returns -1 if index is out of bounds
*******************************************************************************/
unsigned long getBits(int index, chromosome *c, int numBits){
  if(!findChromoOffsets(index) || numBits < 1){
    return 0;
  };

  unsigned long temp = c->bits[B_Index] >> B;
  unsigned long compare = pow(2, numBits) -1;
  return (temp & compare);//doesn't shift bits back to position
}


/*******************************************************************************
* attempts to mutate a chromosome's bits based on some small chance
*******************************************************************************/
void mutate(chromosome *c){
  float multiplier = (Num_Chromos * sizeof(unsigned long) * 8) / (float) MUTATION_RATE + 1;

  for(int x = 0; x < multiplier; x++){
    int value = rand() % MUTATION_RATE; //get a random value between 0 - 999
    value *= x;
    flipBit(c, value);
  }

}


/*******************************************************************************
* flips a specified bit in a chromosome
*******************************************************************************/
static void flipBit(chromosome *c, int index){
  findChromoOffsets(index);
  c->bits[B_Index] = c->bits[B_Index] ^ (1 << B);
}



/*******************************************************************************
* Main function for testing, requires preproccessor declaration in compilation
*******************************************************************************/
#ifdef UNIT_TEST
int main(int argc, char *argv[]){
  printf("%i\n", 31/33);
  initialize(100, 100);
  myChromosomes[0].bits[1] = 1111;
  printf("%lu\n",getBits(66, &myChromosomes[0], 5));
  printf("B_Index: %i\t B: %i\n",B_Index, B);
  //mutate(&myChromosomes[0]);
}
#endif
