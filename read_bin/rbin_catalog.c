#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "forests.h"

int main(int argc, char **argv)
{
   FILE *file;
   char fname[255];
   char line[LINE_MAX];

   struct Halo_Data halo;

   if (argc != 2)
   {
      printf("Usage:\n./rbin_catalog <filename>\n");
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

   while (fread(&halo, sizeof(struct Halo_Data), 1, file) != 0)
   {
      parse_halo_to_string(line, &halo);
      printf("%s", line);
   }

   return EXIT_SUCCESS;
}
