#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#define MAXFORESTS 300000
#define OUTPUTS 126

FILE *Forest;
int64_t FID, FIDX;
int64_t NhalosTot, Nhalos[OUTPUTS];

int64_t LowerIdx, HigherIdx;

void open_forest(char *);
void next_forest();
bool check_halo(int64_t);

void main()
{
   FILE *forest, *list, *fout;
   char forestName[255];
   char listName[255];
   char outName[255];

   int64_t forestIDs[MAXFORESTS];
   int64_t forestIndex[MAXFORESTS];
   int64_t forestNhz0[MAXFORESTS];
   int64_t f, nf, numForest=0;

   int64_t hid, hidx;

   int64_t totHalos=0;

   return;

   sprintf(forestName, "/data/MDPL/CB_ascii/BOX_004/sussing_forests.list");
   sprintf(listName, "b04massive_mdpl.dat");
   sprintf(outName, "b04massive_forest_mdpl.list");

   if (!(list=fopen(listName, "r")))
   {
      fprintf(stderr, "Cannot open list file!\n");
      exit(EXIT_FAILURE);
   }
   if (!(fout=fopen(outName, "w")))
   {
      fprintf(stderr, "Cannot write output file!\n");
      exit(EXIT_FAILURE);
   }

   open_forest(forestName); 

   // We need to iterate over the halos of the list:
   while (!feof(list))
   {
      fscanf(list, " %"SCNd64" %"SCNd64" \n", &hid, &hidx);

      // Does it belong to this forest?
      if (!check_halo(hidx))
         while (!check_halo(hidx))
            next_forest();

      forestIDs[numForest] = FID;
      forestIndex[numForest] = FIDX;
      forestNhz0[numForest] = Nhalos[OUTPUTS-1];
      numForest++;
   }

   // Now we need to remove the repeated ones and write the list:
   fprintf(fout, "%"PRId64" %"PRId64"\n", forestIDs[0], forestIndex[0]);
   totHalos += forestNhz0[0];
   for (f=1, nf=0; f<numForest; f++)
      if (forestIDs[f] != forestIDs[nf])
      {
         fprintf(fout, "%"PRId64" %"PRId64"\n", forestIDs[f], forestIndex[f]);
         totHalos += forestNhz0[f];
         nf = f;
      }

   printf("NumForests: %"PRId64" \n", nf);
   printf("TotHalos at z=0: %"PRId64" \n", totHalos);

   fclose(list);
   fclose(Forest);
   fclose(fout);
}

void open_forest(char *fname)
{
   char stmp[1024];
   int s;

   if (!(Forest=fopen(fname, "r")))
   {
      fprintf(stderr, "Cannot open forest file: %s \n", fname);
      exit(EXIT_FAILURE);
   }

   // Reading the comments:
   fgets(stmp, 1024, Forest);

   // Reading the first forest:
   fscanf(Forest, "%"SCNd64" ", &FID);   
   fscanf(Forest, "%"SCNd64" ", &NhalosTot);

   for (s=0; s<OUTPUTS; s++)
      fscanf(Forest, "%"SCNd64" ", &Nhalos[s]);

   FIDX = 1;
   LowerIdx = 1;
   HigherIdx = Nhalos[OUTPUTS-1];
}

void next_forest()
{
   int s;

   fscanf(Forest, "%"SCNd64" ", &FID);   
   fscanf(Forest, "%"SCNd64" ", &NhalosTot);

   for (s=0; s<OUTPUTS; s++)
      fscanf(Forest, "%"SCNd64" ", &Nhalos[s]);

   FIDX++;

   LowerIdx = HigherIdx+1;
   HigherIdx = LowerIdx+Nhalos[OUTPUTS-1]-1;
}

bool check_halo(int64_t hidx)
{
   if (hidx >= LowerIdx && hidx <= HigherIdx)
      return true;
   else
      return false;
}


