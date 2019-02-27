#include <sys/types.h>
#include <sys/stat.h>

#include "forests.h"

int main(int argc, char **argv)
{
   FILE *flist; 
   int iaux, s, box;
   float faux[4];
   char snapName[LINE_MAX]; 
   char inPath[LINE_MAX], outPath[LINE_MAX];
   char stmp[LINE_MAX];
   char bintag;

   int numSnaps, numParts=0;
   float *ZZ;

   int64_t maxForests=0, nForests=0;

   struct Rockstar_Data *inCat, *outCat;
   enum Status stin, stout;

   struct stat st = {0};

   if (argc != 6 && argc != 7)
   {
      printf("./split_forests_nf <inPath> <outPath> <snapList> <NumSnaps> <N Forests> [i|b|o]\n"
             " binary format flag: 'i'(input), 'o'(output) or 'b'(both)\n");
      exit(EXIT_FAILURE);
   }
   sprintf(inPath, "%s", argv[1]);
   sprintf(outPath, "%s", argv[2]);
   sprintf(snapName, "%s", argv[3]);
   numSnaps = atoi(argv[4]);
   maxForests = atoi(argv[5]);
 
   stin = READ;
   stout = WRITE;

   if (argc == 7)
   {
      bintag = argv[6][0];
      switch (bintag)
      {
         case 'i':
            stin = READB; 
            break;
         case 'o':
            stout = WRITEB; 
            break;
         case 'b':
            stin = READB; 
            stout = WRITEB;
            break;
      }
   }

   /*** The list of snapshots and times ***/
   
   ZZ = malloc(numSnaps*sizeof(float));

   if (!(flist = fopen(snapName, "r")))
   {
      fprintf(stderr, "Cannot open file: %s\n", snapName);
      exit(EXIT_FAILURE);
   }

   while (fgetc(flist) != '\n');  // skip the first line 
   for (s=0; s<numSnaps; s++) 
   {
      //fscanf(flist, " %d  %f  %f  %g %g\n", &iaux, &faux[0], &faux[1], &faux[2], &faux[3]);
      fscanf(flist, " %d  %f  %f\n", &iaux, &faux[0], &faux[1]);
      ZZ[iaux]  = faux[1];
   }
   fclose(flist);
   
   inCat = open_catalogs(inPath, ZZ, numSnaps, stin);
   
   box=1;
   sprintf(stmp, "%s/BOX_%.3d", outPath, box);
   if (stat(stmp, &st) == -1) mkdir(stmp, 0755);
   outCat = open_catalogs(stmp, ZZ, numSnaps, stout);
   
   while (read_next_forest(inCat) != -1) {
      if (nForests == maxForests) {
         close_catalogs(outCat);
         box++;
         sprintf(stmp, "%s/BOX_%.3d", outPath, box);
         if (stat(stmp, &st) == -1) mkdir(stmp, 0755);
         outCat = open_catalogs(stmp, ZZ, numSnaps, stout);
         nForests = 0;
      }
      copy_forest(outCat, inCat);
      nForests++;
   }

   close_catalogs(inCat);

   return(EXIT_SUCCESS);
}


