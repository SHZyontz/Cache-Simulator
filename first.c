#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int offsetBits;
int setBits;

int getSetIndex(unsigned long);

unsigned long getTag(unsigned long);

struct Line{
  int valid;
  unsigned long address;
  long time;
};


int main(int argc, char* argv[argc+1]){


  if(argc!=6){
    printf("error\n");
    return 0;
  }
  if(floor(log2(atoi(argv[1])))!=ceil(log2(atoi(argv[1]))) || floor(log2(atoi(argv[4])))!=ceil(log2(atoi(argv[4])))){
    printf("error\n");
    return 0;
  }

  FILE* traceFile;
  if(!(traceFile=fopen(argv[5],"r"))){
    printf("error\n");
    return 0;
  }

  int cacheSize=atoi(argv[1]);
  
  char* assocType=malloc(10*sizeof(char));
  strcpy(assocType,argv[2]);

  char policy[5];
  strcpy(policy,argv[3]);

  int blockSize=atoi(argv[4]);

  int numSets;
  int numLines;

  if(strcmp(assocType,"direct")==0){
    numSets=cacheSize/blockSize;
    numLines=1;
  }
  else if(strcmp(assocType,"assoc")==0){
    numSets=1;
    numLines=cacheSize/blockSize;
  }
  else{
    assocType+=6;
    numLines=atoi(assocType);
    if(floor(log2(numLines))!=ceil(log2(numLines))){
      assocType-=6;
      free(assocType);
      printf("error\n");
      return 0;
    }
    numSets=cacheSize/(blockSize*numLines);
    assocType-=6;
  }
  free(assocType);

  offsetBits=log2(blockSize);
  setBits=log2(numSets);



  //cache setup

  struct Line*** cache=malloc(numSets*sizeof(struct Line**));
  
  for(int i=0; i<numSets; i++){
    cache[i]=malloc(numLines*sizeof(struct Line*));
    for(int p=0; p<numLines; p++){
      cache[i][p]=malloc(sizeof(struct Line));
      cache[i][p]->valid=0;
      cache[i][p]->address=0;
      cache[i][p]->time=0;
    }
  }//end line setup
  

  //printf("Offset bits=%d\nSet bits=%d\nNum sets=%d\nNum lines=%d\n",offsetBits,setBits,numSets,numLines);



  long memRead=0;
  long memWrite=0;
  long cacheHit=0;
  long cacheMiss=0;

  char command;
  unsigned long address;

  while(fscanf(traceFile,"%c %lx\n",&command,&address)!=EOF){

    if(command=='W')
      memWrite++;

    int setIndex=getSetIndex(address);
    int foundHome=0;

    for(int i=0; i<numLines; i++){

      if(cache[setIndex][i]->valid==0){
	cacheMiss++;
	memRead++;
	cache[setIndex][i]->valid=1;
	cache[setIndex][i]->time=0;
	cache[setIndex][i]->address=address;
	foundHome=1;
	break;
      }//end valid check
      else if(getTag(address)==getTag(cache[setIndex][i]->address)){
	foundHome=1;
	cacheHit++;
	if(strcmp(policy,"lru")==0)
	  cache[setIndex][i]->time=0;
	break;
      }//end tag equals

    }//end line for loop

    if(foundHome==0){
      cacheMiss++;
      memRead++;
      int oldestIndex=0;

      for(int i=1; i<numLines; i++){
	if(cache[setIndex][i]->time > cache[setIndex][oldestIndex]->time)
	  oldestIndex=i;
      }//end line for loop for eviction

      cache[setIndex][oldestIndex]->time=0;
      cache[setIndex][oldestIndex]->address=address;
      
    }//end eviction

    for(int i=0; i<numLines; i++){
      cache[setIndex][i]->time++;
    }//end time increment

  }//end file reading while loop

  printf("memread:%ld\n",memRead);
  printf("memwrite:%ld\n",memWrite);
  printf("cachehit:%ld\n",cacheHit);
  printf("cachemiss:%ld\n",cacheMiss);

  for(int i=0; i<numSets; i++){
    for(int p=0; p<numLines; p++)
      free(cache[i][p]);
    free(cache[i]);
  }
  free(cache);
  
}//end of main






int getSetIndex(unsigned long address){

  return (address>>offsetBits)&((1<<setBits)-1lu);
  
}//end getSetIndex

unsigned long getTag(unsigned long address){

  return (address>>(offsetBits+setBits));
  
}//end getTag
