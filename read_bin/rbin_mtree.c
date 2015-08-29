#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "forests.h"

int main(int argc, char **argv)
{
   FILE *file;
   char fname[255];
   char line[LINE_MAX];
   int64_t i64, ids[2], i;

   struct Halo_Data halo;

   if (argc != 2)
   {
      printf("Usage:\n./rbin_mtree <filename>\n");
      exit(EXIT_FAILURE);
   }

   sprintf(fname, "%s", argv[1]);

   if (!(file = fopen(fname, "r")))
   {
      fprintf(stderr, "Cannot open file: %s\n", fname);
      exit(EXIT_FAILURE);
   }

   // Reading the header:
   fread(line, sizeof(char), LINE_MAX, file);
   printf("%s", line);
   fread(line, sizeof(char), LINE_MAX, file);
   printf("%s", line);
   fread(&i64, sizeof(int64_t), 1, file);
   printf("%"PRId64"\n", i64);

   while (fread(ids, sizeof(int64_t), 2, file) != 0)
   {
      printf("%"PRId64" %"PRId64"\n", ids[0], ids[1]);
      for (i=0; i<ids[1]; i++)
      {
         fread(&i64, sizeof(int64_t), 1, file);
         printf("%"PRId64"\n", i64);
      }
   }

   return EXIT_SUCCESS;
}
