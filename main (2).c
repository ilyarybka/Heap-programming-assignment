/* COP 3502C Assignment 6
This program is written by: Iliya Klishin */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define SIZE 100000
#define MAXSTYLISTS 10

FILE* read;
FILE* write;

typedef struct customers
{
  unsigned int Arrtime;
  char CustName[21];
  char PrefStylistName[21];
  int loyaltyP;
  unsigned int HairCuttime;
  int timeOut;
  //int HairStation;
  int preferredStationIdx; //use it during heap
  int actualStationIdx; //use it during the final sorting process
  
}customers;

typedef struct BinaryHeap{
  struct customers** Custarr;
  char StylistName[21];
  int capacity; // Array size
  int size;//Heap size
  unsigned int timeC;
  int temp;
}BinaryHeap;

 void initHeap(struct BinaryHeap* p, int NumCust)// This function initializes the heap array and the heap itself.
{
  //p = (struct BinaryHeap*)malloc(NumStylists*sizeof(BinaryHeap));
  p->capacity = NumCust;
  p->Custarr = (struct customers**)malloc(sizeof(customers*)*(NumCust+1));
  for(int i = 1; i < NumCust + 1; i++)
  {
    p->Custarr[i] = NULL;
  }
  p->timeC = 0;
  p->temp = 0;
  /*for(int i = 0; i < SIZE; i++)
  {
    p->arr[i] = (struct customers*)malloc(sizeof(customers));
  }*/
  p->size = 0;
  return;

}

customers** createArray(customers** arr, int numCust)// This function allocates memory for the array of pointers for structs for customers.
{

  arr = (struct customers**)malloc(sizeof(struct customers*)*(numCust + 1));
  for(int i = 1 ; i < numCust + 1; i++)
  {
    arr[i] = (customers*)malloc(sizeof(customers));
  }

  return arr;
}


void swap(struct BinaryHeap* p, int index1, int index2)
{
  struct customers* temp = p->Custarr[index1];
  p->Custarr[index1] = p->Custarr[index2];
  p->Custarr[index2] = temp;
}

int CompareTo(customers* c1, customers* c2)// This function compares two customers with loyalty points, preffered stylists, and their names, and decided if c1 has a higher priority than c2.
{
  if(c1->loyaltyP > c2->loyaltyP)
    return 1;
  
  else if(c1->loyaltyP == c2->loyaltyP && c1->preferredStationIdx != -1)
    return 1;
  //check if only one of them got the preferred stylist, then that customer will get higher priority
  else if(c1->loyaltyP == c2->loyaltyP && c1->preferredStationIdx == c2->preferredStationIdx)
  {
   //if(strcmp(c1->CustName, c2->CustName) > 0)
  //return 1;
  return strcmp(c2->CustName, c1->CustName);
  }
  else
    return -1;

  return -1;
}

void PercolateDown(struct BinaryHeap* p, int index)
{
  int max_ind;

  if((2*index + 1) <= p->size)
  {
    //you need to find the index of the the max out of your two childern
    // have a varible called maxindex and update it based on the compareto FUnction call 
    //then swap the parent with the max
    if(CompareTo(p->Custarr[2*index], p->Custarr[2*index + 1]) < 0)
    {
      max_ind = 2*index;
    }
    else
      max_ind = 2*index + 1;

      if(CompareTo(p->Custarr[max_ind], p->Custarr[index]) > 0)
      {
        swap(p, max_ind, index);

        PercolateDown(p, max_ind);
      }
  }
  else if(p->size == 2*index)
  {// call compareto function to decide about swapping
    if(CompareTo(p->Custarr[2*index], p->Custarr[index]) > 0)                
      swap(p, (2*index), index);
  }
}

void PercolateUp(struct BinaryHeap* p, int index)
{
  if(index > 1)
  {
    if(CompareTo(p->Custarr[index], p->Custarr[(index)/2]) > 0){ //not correct. use compareto function
      swap(p, index, (index)/2);
      PercolateUp(p, (index)/2);
    }
  }
}

int PrefStylistStation(BinaryHeap* p, int NumStylists, char* PrefStyl)// This function returns the index of the station of the preffered stylists of the customer.It is not a station where the customer is inserted.
{
  for(int i = 0; i < NumStylists; i++)
  {
    if(strcmp(p[i].StylistName, PrefStyl) == 0)
      return i;
  }

  return -1;

}

int SmallStation(BinaryHeap* p, customers* cust, int NumStylists) // This function finds the station of the index when the customer does not have preference.
{
  int min_ind = 0;

  for(int i = 0; i < NumStylists;i++)
  {
    p[i].temp = p[i].size;
    if(p[i].timeC < cust->Arrtime)
    {
      p[i].temp++;
    }

    if(p[i].temp < p[min_ind].temp){
      min_ind = i;
    }
   // if(p[i].size < p[min_ind].size)
   //   min_ind = i;

   // else if (p[i].size == p[min_ind].size){
   //   if(min_ind < i)
   //     min_ind = i;
   // }
        
    else if(p[i].timeC < p[min_ind].timeC)
      min_ind = i;
    
  }

  return min_ind;
}

int GetStation(BinaryHeap* p, customers* cust, int NumStylists)// This function calculates the station for the customer we want to insert to the heap
{
  int Station_ind = PrefStylistStation(p, NumStylists, cust->PrefStylistName);

  if(Station_ind == -1)
  {
    return SmallStation(p, cust, NumStylists);
  }

  else
    return Station_ind;
}
//Insert customer into the heap
void insert(BinaryHeap* p, customers* Cust)// This function inserts the customer to the heap which is assigned by Stylist's name and station
{
  customers* temp, remove;
  int i;
  //int indDave = 0, indSandy = 1, indBella = 2;
  //if(strcmp(Cust->PrefStylistName, p->))
  if(p->size == p->capacity)
  {
    p->Custarr = (customers**)realloc(p->Custarr, sizeof(customers*)*(2*p->capacity));

    if(p->Custarr == NULL)
    {
      printf("Reallocation failed");
      return;
    }

    p->capacity *= 2;
  }
  
  p->size++;
  p->Custarr[p->size] = Cust;
  PercolateUp(p, p->size);
  //for(int i = 0; i < NumStylists ;i++)
  //{
   // for(int j = 1; p[i].Custarr[j] != NULL;j++){
     // int min_ind = 0;
      //if(p[i].Custarr[j]->time > p[i].Custarr[min_ind]->time){

     // }
    //}
  //}
}
//Remove customer from the Heap
customers* removeMaxPriority(BinaryHeap* p)
{
  customers* retval;

  if (p->size > 0)
  {
    retval = p->Custarr[1];
    
    p->Custarr[1] = p->Custarr[p->size];

    p->size--;

    if(p->size > 0)
    PercolateDown(p, 1);

    return retval;
  }

  else
  return NULL;

}

void UpdateHeap(BinaryHeap* p, int NumStylists, customers** custArr, int NumCust)
{
  customers* deleted;

   for(int i = 1; i < NumCust + 1; i++)
   { // NumCust
       int StationIn = GetStation(p, custArr[i], NumStylists);
      //for (int j = 0; j < NumStylists ; j++){ // NumStylists
      //printf("%d Station for customer %dth\n ", StationIn, i);
          
          //int StationIn = GetStation(p, custArr[i], NumStylists);
        if(p[StationIn].timeC < custArr[i]->Arrtime){
          
          if(p[StationIn].size == 0)
          {
            insert(&p[StationIn], custArr[i]);
           // printf("%s inserted as a first customer\n", custArr[i]->CustName);
          }
          else
          {

          deleted = removeMaxPriority(&p[StationIn]);
          //printf("%s is deleted\n", deleted->CustName);

          p[StationIn].timeC = deleted->Arrtime + deleted->HairCuttime;

          //deleted->timeOut = deleted->Arrtime + deleted->HairCuttime;
          deleted->timeOut = p[StationIn].timeC;
          deleted->actualStationIdx = StationIn;
          deleted->loyaltyP = deleted->loyaltyP + (deleted->HairCuttime/10);

          insert(&p[StationIn], custArr[i]);
          //printf("%s is inserted after deletion\n", custArr[i]->CustName);
          }
          //p[StationIn].Custarr[j]->actualStationIdx = i
          //insert(&p[StationIn], custArr[i]);
          

        }

        else{
          insert(&p[StationIn], custArr[i]);
          //printf("%s when the arrival time is not greater than timeline\n", custArr[i]->CustName);

          //p[StationIn].timeC += p[StationIn].Custarr[i]->HairCuttime;
          
        }

      //}
      
    }
 //for(int j = 1; j < NumCust + 1;j++)
 //{
   //for(int j1 = 0; j1 < NumStylists; j1++)
   //{
   //if(p[j1].timeC < custArr[j]->Arrtime){
  //   if(p[j1].Custarr != NULL)
    //  removeMaxPriority(&p[j1]);
  //}
  

   //}
 //}
}

void DeleteAndUpdateArray(BinaryHeap* p, int NumStylists)
{
  customers* deleteCust;
  for(int i = 0; i < NumStylists; i++)
  {
    for(int j = 1; p[i].Custarr[j] != NULL; j++){
      if(p[i].size != 0)
      {
        deleteCust = removeMaxPriority(&p[i]);

          p[i].timeC = p[i].timeC + deleteCust->HairCuttime;

          deleteCust->timeOut = p[i].timeC;
          deleteCust->actualStationIdx = i;
          deleteCust->loyaltyP = deleteCust->loyaltyP + (deleteCust->HairCuttime/10);
      }
    }
  }
}

void merge(customers** custArr, int left, int mid, int right)
{

  int i,j,k;

  int leftsize = mid - left + 1;

  int r1 = right - mid;

  customers** L = malloc(sizeof(customers*)*leftsize);
  customers** R = malloc(sizeof(customers*)*r1);

  for(i = 0; i<leftsize; i++){

    L[i] = custArr[left+i];
  }

  for(i = 0; i < r1;i++){

    R[i] = custArr[mid+1+i];
  }

  i = 0;
  j = 0;
  k = left;

  while(i< leftsize && j< r1)
  {
    if(L[i]->timeOut < R[j]->timeOut)
    {
      custArr[k] = L[i];
      i++;
      k++;
    }

    else if(L[i]->timeOut < R[j]->timeOut && L[i]->actualStationIdx < R[j]->actualStationIdx)
    {
      custArr[k] = L[i];
      i++;
      k++;
    }

    else {
      custArr[k] = R[j];
      j++;
      k++;
    }
  }

  while( i < leftsize)
  {
    custArr[k] = L[i];
      k++;
      i++;
  }

  while(j < r1)
  {
    custArr[k] = R[j];
      j++;
      k++;
  }

 free(L);
 free(R);
  
}

void mergeSort(customers** custArr, int l, int r)
{
  if (l<r)
  {
    int mid = (l+r)/ 2;

    mergeSort( custArr, l, mid);

    mergeSort( custArr, mid+1, r);

    merge(custArr, l, mid, r);
  }
}

void printCustArr(BinaryHeap* p, customers** custArr, int numCust)
{
  for(int i = 1; i < numCust + 1; i++)
  {
    printf("%s %d %u %s\n", custArr[i]->CustName, custArr[i]->timeOut, custArr[i]->loyaltyP, p[custArr[i]->actualStationIdx].StylistName);
    fprintf(write,"%s %d %u %s\n", custArr[i]->CustName, custArr[i]->timeOut, custArr[i]->loyaltyP, p[custArr[i]->actualStationIdx].StylistName);
  }
}

void freeMemory(BinaryHeap* p, customers** CustArr, int numCust, int NumStylists)
{
  for(int i = 0; i < NumStylists; i++)
  {
    free(p[i].Custarr);
  }

  for(int j1 = 0; j1 < numCust + 1; j1++)
  {
    free(CustArr[j1]);
  }

  free(CustArr);
}

// iif strcmp(stylistname, "NONE") == 0  -1
//loop through the heap and return the correct index of the heap
//if the stylistname not found, return -1

int main(void) {

  atexit(report_mem_leak);

  read = fopen("in.txt", "r");

  write = fopen("out.txt","w");

  struct BinaryHeap p[10];

  customers** CustArr;

  int numCust, NumStylists;

  fscanf(read, "%d %d", &numCust, &NumStylists);

 // printf("%d %d", numCust, NumStylists);

  char StylName[21];

// Initialize heap
  for(int i1 = 0; i1 < NumStylists; i1++)
  {
    initHeap(&p[i1], numCust);
  }
// Put stylists' names into the heap
  for (int i = 0; i < NumStylists; i++)
  {
    fscanf(read, "%s", StylName);
    strcpy(p[i].StylistName, StylName);
    //printf("%s\n", StylName);
    //printf("%s\n", p[i].StylistName);
  }

  CustArr = createArray(CustArr,numCust);
  // Putting customers' information in the customers array
  for(int j = 1; j < numCust + 1;j++)
  {
    unsigned int time, haircut;
    int loyP;
    char CustName[21], prefStylName[21];
    int HairStation;

    fscanf(read, "%u %s %s %d %u", &time, CustName, prefStylName, &loyP, &haircut);

    CustArr[j]->Arrtime = time;
    strcpy(CustArr[j]->CustName, CustName);
    strcpy(CustArr[j]->PrefStylistName, prefStylName);
    CustArr[j]->loyaltyP = loyP;
    CustArr[j]->HairCuttime = haircut;
    CustArr[j]->preferredStationIdx= PrefStylistStation(p, NumStylists,CustArr[j]->PrefStylistName);//getIndexForStylist(prefStlName); //it will return -1 if it is none or not in the list of the heaps, or it will return -1
    CustArr[j]->timeOut = 0;
    CustArr[j]->actualStationIdx = -1;


  }
//now start processing the customers and use the strategy mentioned in the hints.
  for(int j1 = 1; j1 < numCust + 1; j1++)
  {
    int StationInd = GetStation(p, CustArr[j1], NumStylists);

    //printf("Station index for %d customer: %d \n", j1, StationInd);

    //insert(&p[StationInd], CustArr[j1]);
  }

  //printf("Heap size %d\n", p[1].size);

  //put them in another loop
  // int stationIndex = GetStation(p, CustArr[j], NumStylists);
  //   CustArr[j]->HairStation = stationIndex;
  
  //   //insert(&p[stationIndex], CustArr[j]);


  for( int i2 = 1; i2 < numCust + 1; i2++)
  {
    // printf("index %d: %u time, %s Customer's name, %s Pref. Stylist, %d Lotalty points, %u haircut time, %d HairStation\n\n", i2, CustArr[i2]->Arrtime, CustArr[i2]->CustName, CustArr[i2]->PrefStylistName, CustArr[i2]->loyaltyP, CustArr[i2]->HairCuttime, CustArr[i2]->preferredStationIdx);
  }

  for (int j2 = 0; j2 < NumStylists; j2++)
  {
    //while(p[j2].Custarr != NULL)
    for(int j3 = 1; p[j2].Custarr[j3] != NULL; j3++)
    {
      //printf("Heap-> index %d: %u time, %s Customer's name, %s Pref. Stylist, %d Lotalty points, %u haircut time, %d HairStation\n\n", j2, p[j2].Custarr[j3]->time, p[j2].Custarr[j3]->CustName, p[j2].Custarr[j3]->PrefStylistName, p[j2].Custarr[j3]->loyaltyP, p[j2].Custarr[j3]->HairCuttime, p[j2].Custarr[j3]->preferredStationIdx);
    }

  //  break;
  }

  for(int i4 = 1; p[1].Custarr[i4]!=NULL; i4++){
    //removeMaxPriority(&p[1]);
    // printf("Heap size %d\n", p[1].size);
  }

  //printf("\n\n");

  //removeMaxPriority(&p[1]);

  UpdateHeap(p, NumStylists, CustArr, numCust);

  DeleteAndUpdateArray(p, NumStylists);

  mergeSort(CustArr, 1, numCust);

  printCustArr(p,CustArr, numCust);

  freeMemory(p, CustArr, numCust, NumStylists);

  fclose(read);
  fclose(write);



  for (int j2 = 0; j2 < NumStylists; j2++)
  {
    //while(p[j2].Custarr != NULL)
    for(int j3 = 1; p[j2].Custarr[j3] != NULL; j3++)
    {
      //printf("Heap-> index %d: %u time, %s Customer's name, %s Pref. Stylist, %d Lotalty points, %u haircut time, %d HairStation\n\n", j2, p[j2].Custarr[j3]->Arrtime, p[j2].Custarr[j3]->CustName, p[j2].Custarr[j3]->PrefStylistName, p[j2].Custarr[j3]->loyaltyP, p[j2].Custarr[j3]->HairCuttime, p[j2].Custarr[j3]->preferredStationIdx);
    }

  //  break;
  }


  //for( int i2 = 1; i2 < numCust + 1; i2++)
    //{
    // printf("index %d: %u time, %s Customer's name, %s Pref. Stylist, %d Lotalty points, %u haircut time, %d HairStation, %d timeOut\n\n", i2, CustArr[i2]->Arrtime, CustArr[i2]->CustName, CustArr[i2]->PrefStylistName, CustArr[i2]->loyaltyP, CustArr[i2]->HairCuttime, CustArr[i2]->preferredStationIdx, CustArr[i2]->timeOut);
    //}

  //printf("Hello World\n");
  return 0;
}