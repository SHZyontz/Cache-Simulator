#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int offsetBits;
int setBits;

int offsetBits2;
int setBits2;

int getSetIndex(unsigned long);

unsigned long getTag(unsigned long);

int getSetIndex2(unsigned long);

unsigned long getTag2(unsigned long);

struct Line{
  int valid;
  unsigned long address;
  long time;
};


int main(int argc, char* argv[argc+1]){


  if(argc!=9){
    printf("error\n");
    return 0;
  }
  if(floor(log2(atoi(argv[1])))!=ceil(log2(atoi(argv[1]))) || floor(log2(atoi(argv[4])))!=ceil(log2(atoi(argv[4]))) || floor(log2(atoi(argv[5])))!=ceil(log2(atoi(argv[5])))){
    printf("error\n");
    return 0;
  }

  FILE* traceFile;
  if(!(traceFile=fopen(argv[8],"r"))){
    printf("error\n");
    return 0;
  }

  int cacheSize=atoi(argv[1]);
    int cacheSize2=atoi(argv[5]);
  
  char* assocType=malloc(10*sizeof(char));
  strcpy(assocType,argv[2]);

    char* assocType2=malloc(10*sizeof(char));
    strcpy(assocType2,argv[6]);

  char policy[5];
  strcpy(policy,argv[3]);

    char policy2[5];
    strcpy(policy2,argv[7]);

  int blockSize=atoi(argv[4]);

  int numSets;
  int numLines;

    int numSets2;
    int numLines2;

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



      if(strcmp(assocType2,"direct")==0){
    numSets2=cacheSize2/blockSize;
    numLines2=1;
  }
  else if(strcmp(assocType2,"assoc")==0){
    numSets2=1;
    numLines2=cacheSize2/blockSize;
  }
  else{
    assocType2+=6;
    numLines2=atoi(assocType2);
    if(floor(log2(numLines2))!=ceil(log2(numLines2))){
      assocType2-=6;
      free(assocType2);
      printf("error\n");
      return 0;
    }
    numSets2=cacheSize2/(blockSize*numLines2);
    assocType2-=6;
  }
  free(assocType2);

  offsetBits2=log2(blockSize);
  setBits2=log2(numSets2);



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


    struct Line*** cacheL2=malloc(numSets2*sizeof(struct Line**));

    for(int i=0; i<numSets2; i++){
      cacheL2[i]=malloc(numLines2*sizeof(struct Line*));
      for(int p=0; p<numLines2; p++){
	cacheL2[i][p]=malloc(sizeof(struct Line));
	cacheL2[i][p]->valid=0;
	cacheL2[i][p]->address=0;
	cacheL2[i][p]->time=0;
      }
    }
  

    //printf("Offset bits=%d\nSet bits=%d\nNum sets=%d\nNum lines=%d\n\n",offsetBits,setBits,numSets,numLines);
    //printf("L2 Offset Bits=%d\nL2 Set bits=%d\nL2 Num sets=%d\nL2 Num lines=%d\n\n",offsetBits2,setBits2,numSets2,numLines2);



  long memRead=0;
  long memWrite=0;
  long cacheHit=0;
  long cacheMiss=0;
    long cacheHit2=0;
    long cacheMiss2=0;

  char command;
  unsigned long address;

  while(fscanf(traceFile,"%c %lx\n",&command,&address)!=EOF){

    if(command=='W')
      memWrite++;

    int foundTag=0;
    int setIndex1=getSetIndex(address);

    for(int i=0; i<numLines; i++){

      if(cache[setIndex1][i]->valid==0)
	break;
      if(getTag(cache[setIndex1][i]->address)==getTag(address)){
	cacheHit++;
	foundTag=1;
	if(strcmp(policy,"lru")==0)
	  cache[setIndex1][i]->time=0;
	break;
      }//end L1 hit case
      
    }//end L1 check

    if(foundTag==1){
      for(int i=0; i<numLines; i++){
	if(cache[setIndex1][i]->valid==0)
	  break;
	cache[setIndex1][i]->time++;
      }//end time increment
	
    }//end L1 cacheHit case

    else if(foundTag==0){

      cacheMiss++;

      int foundTag2=0;
      int setIndex2=getSetIndex2(address);
      int tagIndex2=0;

      for(int i=0; i<numLines2; i++){
	if(cacheL2[setIndex2][i]->valid==0)
	  break;
	if(getTag2(cacheL2[setIndex2][i]->address)==getTag2(address)){
	  cacheHit2++;
	  foundTag2=1;
	  tagIndex2=i;
	  if(strcmp(policy2,"lru")==0)
	    cacheL2[setIndex2][i]->time=0;
	  break;
	}//end L2 hit case
      }//end L2 check

      if(foundTag2==1){
	for(int i=0; i<numLines2; i++){
	  if(cacheL2[setIndex2][i]->valid==0)
	    break;
	  cacheL2[setIndex2][i]->time++;
	}

	unsigned long evictedAddressL2=cacheL2[setIndex2][tagIndex2]->address;
	
	cacheL2[setIndex2][tagIndex2]->valid=0;
	cacheL2[setIndex2][tagIndex2]->time=0;
	cacheL2[setIndex2][tagIndex2]->address=0;
	for(int i=tagIndex2; i<numLines2-1; i++){
	  if(cacheL2[setIndex2][i]->valid<cacheL2[setIndex2][i+1]->valid){

	    long tempTime=cacheL2[setIndex2][i]->time;
	    unsigned long tempAddress=cacheL2[setIndex2][i]->address;
	    int tempValid=cacheL2[setIndex2][i]->valid;

	    cacheL2[setIndex2][i]->time=cacheL2[setIndex2][i+1]->time;
	    cacheL2[setIndex2][i]->address=cacheL2[setIndex2][i+1]->address;
	    cacheL2[setIndex2][i]->valid=cacheL2[setIndex2][i+1]->valid;

	    cacheL2[setIndex2][i+1]->time=tempTime;
	    cacheL2[setIndex2][i+1]->address=tempAddress;
	    cacheL2[setIndex2][i+1]->valid=tempValid;

	    
	    /*struct Line* temp=cacheL2[setIndex2][i];
	    cacheL2[setIndex2][i]=cacheL2[setIndex2][i+1];
	    cacheL2[setIndex2][i+1]=temp;*/
	  }
	  else break;
	}//end L2 remove shuffle

	int foundHome1=0;
	int evictedL2Set=getSetIndex(evictedAddressL2);

	for(int i=0; i<numLines; i++){
	  if(cache[evictedL2Set][i]->valid==0){
	    foundHome1=1;
	    cache[evictedL2Set][i]->valid=1;
	    cache[evictedL2Set][i]->time=0;
	    cache[evictedL2Set][i]->address=evictedAddressL2;
	    break;
	  }
	}//end find home in L1

	if(foundHome1==1){
	  for(int i=0; i<numLines; i++){
	    if(cache[evictedL2Set][i]->valid==0)
	      break;
	    cache[evictedL2Set][i]->time++;
	  }
	}//end found home in L1 case
	else if(foundHome1==0){

	  int oldestIndex1=0;
	  for(int i=1; i<numLines; i++){
	    if(cache[evictedL2Set][i]->time>cache[evictedL2Set][oldestIndex1]->time)
	      oldestIndex1=i;
	  }//end L1 evict loop

	  unsigned long evictedAddress=cache[evictedL2Set][oldestIndex1]->address;
	  cache[evictedL2Set][oldestIndex1]->address=evictedAddressL2;
	  cache[evictedL2Set][oldestIndex1]->time=0;

	   for(int i=0; i<numLines; i++){
	    if(cache[evictedL2Set][i]->valid==0)
	      break;
	    cache[evictedL2Set][i]->time++;
	  }

	   int evictedSet=getSetIndex2(evictedAddress);
	   int foundHome2=0;

	   for(int i=0; i<numLines2; i++){
	     if(cacheL2[evictedSet][i]->valid==0){
	       foundHome2=1;
	       cacheL2[evictedSet][i]->valid=1;
	       cacheL2[evictedSet][i]->time=0;
	       cacheL2[evictedSet][i]->address=evictedAddress;
	       break;
	     }
	   }//end find home in L2

	   if(foundHome2==1){
	     for(int i=0; i<numLines2; i++){
	       if(cacheL2[evictedSet][i]->valid==0)
		 break;
	       cacheL2[evictedSet][i]->time++;
	     }
	   }//end found home in L2 case
	   else if(foundHome2==0){
	     int oldestIndex2=0;
	     for(int i=1; i<numLines2; i++){
	       if(cacheL2[evictedSet][i]->time>cacheL2[evictedSet][oldestIndex2]->time)
		 oldestIndex2=i;
	     }
	     cacheL2[evictedSet][oldestIndex2]->time=0;
	     cacheL2[evictedSet][oldestIndex2]->address=evictedAddress;

	     for(int i=0; i<numLines2; i++){
	       if(cacheL2[evictedSet][i]->valid==0)
		 break;
	       cacheL2[evictedSet][i]->time++;
	     }
	     
	   }//end no room in L2 case
	  
	}//end no room in L1 case
	
      }//end L2 cacheHit case
      else if(foundTag2==0){

	cacheMiss2++;
	memRead++;

	int foundHome1=0;

	for(int i=0; i<numLines; i++){
	  if(cache[setIndex1][i]->valid==0){
	    foundHome1=1;
	    cache[setIndex1][i]->valid=1;
	    cache[setIndex1][i]->time=0;
	    cache[setIndex1][i]->address=address;
	    break;
	  }
	}//end find home in L1

	if(foundHome1==1){
	  for(int i=0; i<numLines; i++){
	    if(cache[setIndex1][i]->valid==0)
	      break;
	    cache[setIndex1][i]->time++;
	  }
	}//end found home in L1
	else if(foundHome1==0){

	  int oldestIndex1=0;
	  unsigned long evictedAddress=0;

	  for(int i=1; i<numLines; i++){
	    if(cache[setIndex1][i]->time>cache[setIndex1][oldestIndex1]->time)
	      oldestIndex1=i;
	  }//end find oldestIndex

	  evictedAddress=cache[setIndex1][oldestIndex1]->address;
	  
	  cache[setIndex1][oldestIndex1]->address=address;
	  cache[setIndex1][oldestIndex1]->time=0;

	   for(int i=0; i<numLines; i++){
	    if(cache[setIndex1][i]->valid==0)
	      break;
	    cache[setIndex1][i]->time++;
	  }

	   int evictedSet=getSetIndex2(evictedAddress);
	   int foundHome2=0;

	   for(int i=0; i<numLines2; i++){
	     if(cacheL2[evictedSet][i]->valid==0){
	       foundHome2=1;
	       cacheL2[evictedSet][i]->valid=1;
	       cacheL2[evictedSet][i]->address=evictedAddress;
	       cacheL2[evictedSet][i]->time=0;
	       break;
	     }
	   }//end find home in L2 case

	   if(foundHome2==1){
	     for(int i=0; i<numLines2; i++){
	       if(cacheL2[evictedSet][i]->valid==0)
		 break;
	       cacheL2[evictedSet][i]->time++;
	     }
	   }//end found home in L2 case

	   else if(foundHome2==0){

	     int oldestIndex2=0;

	     for(int i=1; i<numLines2; i++){
	       if(cacheL2[evictedSet][i]->time>cacheL2[evictedSet][oldestIndex2]->time)
		 oldestIndex2=i;
	     }//end find oldest loop

	     cacheL2[evictedSet][oldestIndex2]->time=0;
	     cacheL2[evictedSet][oldestIndex2]->address=evictedAddress;

	      for(int i=0; i<numLines2; i++){
	    if(cacheL2[evictedSet][i]->valid==0)
	      break;
	    cacheL2[evictedSet][i]->time++;
	  }
	     
	   }//end no room in L2 case
	  
	  
	}//end no room in L1 case

      }//end L2 cacheMiss case

    }//end L1 cacheMiss case

  }//end file reading while loop

  printf("memread:%ld\n",memRead);
  printf("memwrite:%ld\n",memWrite);
  printf("l1cachehit:%ld\n",cacheHit);
  printf("l1cachemiss:%ld\n",cacheMiss);
  printf("l2cachehit:%ld\n",cacheHit2);
  printf("l2cachemiss:%ld\n",cacheMiss2);

  for(int i=0; i<numSets; i++){
    for(int p=0; p<numLines; p++)
      free(cache[i][p]);
    free(cache[i]);
  }
  free(cache);

  for(int i=0; i<numSets2; i++){
    for(int p=0; p<numLines2; p++)
      free(cacheL2[i][p]);
    free(cacheL2[i]);
  }
  free(cacheL2);
}//end of main






int getSetIndex(unsigned long address){

  return (address>>offsetBits)&((1<<setBits)-1lu);
  
}//end getSetIndex

unsigned long getTag(unsigned long address){

  return (address>>(offsetBits+setBits));
  
}//end getTag

int getSetIndex2(unsigned long address){

  return (address>>offsetBits2)&((1<<setBits2)-1lu);
  
}//end getSetIndex

unsigned long getTag2(unsigned long address){

  return (address>>(offsetBits2+setBits2));
  
}//end getTag
