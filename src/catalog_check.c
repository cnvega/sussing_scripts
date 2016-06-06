
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

   struct Rockstar_Data *inCat;
   enum Status stin;

   if (argc != 4)
   {
      printf("./catalog_check <inPath> <snapList> <NumSnaps>\n");
      exit(EXIT_FAILURE);
   }
   sprintf(inPath, "%s", argv[1]);
   sprintf(snapName, "%s", argv[2]);
   numSnaps = atoi(argv[3]);

   stin = READ;

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


   /* The catalogs: */
   inCat = open_catalogs(inPath, ZZ, numSnaps, stin);

   while (read_next_forest(inCat) != -1)
      skip_and_check_forest(inCat);

   close_catalogs(inCat);

   printf("Done!\nThe merger tree is consistent with the order of the halos in the catalogs.\n");

   return(EXIT_SUCCESS);
}


