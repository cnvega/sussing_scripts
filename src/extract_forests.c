#include "forests.h"

int main(int argc, char **argv)
{
   FILE *flist; 
   int iaux, s;
   float faux[4];
   char snapName[LINE_MAX], forestList[LINE_MAX];
   char inPath[LINE_MAX], outPath[LINE_MAX];
   char bintag;

   int numSnaps;
   float *ZZ;

   int64_t hid, hidx;

   struct Rockstar_Data *inCat, *outCat;
   enum Status stin, stout;

   if (argc != 6 && argc != 7)
   {
      printf("./extract_forests <inPath> <outPath> <snapList> <NumSnaps> <forestList> [i|b|o]\n"
             " binary format flag: 'i'(input), 'o'(output) or 'b'(both)\n");
      exit(EXIT_FAILURE);
   }
   sprintf(inPath, "%s", argv[1]);
   sprintf(outPath, "%s", argv[2]);
   sprintf(snapName, "%s", argv[3]);
   numSnaps = atoi(argv[4]);
   sprintf(forestList, "%s", argv[5]);

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
      fscanf(flist, " %d  %f  %f  %g %g\n", &iaux, &faux[0], &faux[1], &faux[2], &faux[3]);
      ZZ[iaux]  = faux[1];
   }
   fclose(flist);

   /* The list of forests */
   if (!(flist = fopen(forestList, "r")))
   {
      fprintf(stderr, "Cannot open file: %s\n", forestList);
      exit(EXIT_FAILURE);
   }

   /* The catalogs: */
   inCat = open_catalogs(inPath, ZZ, numSnaps, stin);
   outCat = open_catalogs(outPath, ZZ, numSnaps, stout);

   while (!feof(flist))
   {
      fscanf(flist, "%"SCNd64" %"SCNd64"\n", &hid, &hidx);

      while (read_next_forest(inCat) != hid)
         skip_forest_halos(inCat);

      copy_forest(outCat, inCat);
   }

   close_catalogs(inCat);
   close_catalogs(outCat);

   return(EXIT_SUCCESS);
}


