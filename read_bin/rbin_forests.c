#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "forests.h"

int main(int argc, char **argv)
{
   FILE *file;
   char fname[255];
   char line[LINE_MAX];
   int  nsnaps, s;
   int64_t ids[2];
   int64_t *nhalos;

   struct Halo_Data halo;

   if (argc != 3)
   {
      printf("Usage:\n./rbin_forests <filename> <Num Snaps>\n");
      exit(EXIT_FAILURE);
   }

   sprintf(fname, "%s", argv[1]);

   if (!(file = fopen(fname, "r")))
   {
      fprintf(stderr, "Cannot open file: %s\n", fname);
      exit(EXIT_FAILURE);
   }

   nsnaps = atoi(argv[2]);

   // Reading the header:
   fread(line, sizeof(char), LINE_MAX, file);
   printf("%s", line);

   nhalos = malloc(nsnaps*sizeof(int64_t));

   while (fread(ids, sizeof(int64_t), 2, file) != 0)
   {
      fread(nhalos, sizeof(int64_t), nsnaps, file);
      printf("%"PRId64" %"PRId64, ids[0], ids[1]);
      for (s=0; s<nsnaps; s++)
         printf(" %"PRId64, nhalos[s]);
      printf("\n");
         
   }

   return EXIT_SUCCESS;
}
