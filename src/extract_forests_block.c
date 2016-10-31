#include "forests.h"

int main(int argc, char **argv)
{
   FILE *fin; 
   int iaux, s;
   float faux[4];
   char snapName[LINE_MAX];
   char inPath[LINE_MAX], outPath[LINE_MAX];
   char bintag;

   int numSnaps;
   float *ZZ;

   int firstForest;
   int numForests, nf;

   int64_t hid, hidx;

   struct Rockstar_Data *inCat, *outCat;
   enum Status stin, stout;

   if (argc != 7 && argc != 8)
   {
      printf("./extract_forests <inPath> <outPath> <snapList> <NumSnaps> <forest_idx> <Num_forests> [i|b|o]\n"
             " binary format flag: 'i'(input), 'o'(output) or 'b'(both)\n");
      exit(EXIT_FAILURE);
   }
   sprintf(inPath, "%s", argv[1]);
   sprintf(outPath, "%s", argv[2]);
   sprintf(snapName, "%s", argv[3]);
   numSnaps = atoi(argv[4]);
   firstForest = atoi(argv[5]);
   numForests = atoi(argv[6]);

   stin = READ;
   stout = WRITE;

   if (argc == 8)
   {
      bintag = argv[7][0];
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

   if (!(fin = fopen(snapName, "r")))
   {
      fprintf(stderr, "Cannot open file: %s\n", snapName);
      exit(EXIT_FAILURE);
   }

   while (fgetc(fin) != '\n');  // skip the first line 
   for (s=0; s<numSnaps; s++) 
   {
      fscanf(fin, " %d  %f  %f  %g %g\n", &iaux, &faux[0], &faux[1], &faux[2], &faux[3]);
      ZZ[iaux]  = faux[1];
   }
   fclose(fin);

   /* The catalogs: */
   inCat = open_catalogs(inPath, ZZ, numSnaps, stin);
   outCat = open_catalogs(outPath, ZZ, numSnaps, stout);

   /* skipping */
   for (nf=1; nf<firstForest; nf++)
   {
      read_next_forest(inCat);
      skip_forest_halos(inCat);
   }

   for (nf=1; nf<=numForests; nf++)
   {
      read_next_forest(inCat);
      copy_forest(outCat, inCat);
   }
    
   close_catalogs(inCat);
   close_catalogs(outCat);

   return(EXIT_SUCCESS);
}


