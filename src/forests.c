#ifndef _FORESTS_C
#define _FORESTS_C

#include "forests.h"

struct Rockstar_Data* 
open_catalogs(char *basename, float *snaps, int nsnaps, enum Status status)
{
   char fname[LINE_MAX];
   char st; 
   int s;
   struct Rockstar_Data *cat;

   cat = malloc(sizeof(struct Rockstar_Data));

   if (status == READ)
      st = 'r';
   if (status == WRITE)
      st = 'w';

   cat->status = status;

   cat->Nsnaps = nsnaps;
   
   cat->Z = malloc(nsnaps*sizeof(float));
   cat->Nhalos = malloc(nsnaps*sizeof(int64_t));
   cat->CatalogHeader = malloc(nsnaps*sizeof(char *));
   sprintf(cat->Path, "%s", basename);

   for (s=0; s<cat->Nsnaps; s++)
   {
      cat->CatalogHeader[s] = malloc(LINE_MAX*sizeof(char));
      cat->Z[s] = snaps[s];
   }


   /*** The forest file: ***/
   sprintf(fname, "%s/sussing_forests.list", cat->Path);
   if (!(cat->Forest = fopen(fname, &st)))
   {
      fprintf(stderr, "Cannot open forest file: %s\n", fname);
      exit(EXIT_FAILURE);
   }
   cat->FID = 0;
   cat->NhalosTot = 0;
   cat->CountHalos = 0;
   cat->CountForests = 0;
   
   if (status == READ)
      fgets(cat->ForestHeader, LINE_MAX, cat->Forest);

   /*** The catalogs ***/
   cat->Catalogs = malloc(cat->Nsnaps*sizeof(FILE *));
   for (s=0; s<cat->Nsnaps; s++)
   {
      sprintf(fname, "%s/sussing_%03d.z%.3f.AHF_halos", cat->Path, s, cat->Z[s]);
      if (!(cat->Catalogs[s] = fopen(fname, &st)))
      {
         fprintf(stderr, "Cannot open catalog file: %s\n", fname);
         exit(EXIT_FAILURE);
      }
      if (status == READ)
         fgets(cat->CatalogHeader[s], LINE_MAX, cat->Catalogs[s]);      
   }

   /*** The Merger Tree ***/
   sprintf(fname, "%s/sussing_tree.list", cat->Path);
   if (!(cat->MergerTree = fopen(fname, &st)))
   {
      fprintf(stderr, "Cannot open merger tree file: %s\n", fname);
      exit(EXIT_FAILURE);
   }

   if (status == READ)
   {
      // Name, version, numhalos
      fgets(cat->MergerTreeHeader[0], LINE_MAX, cat->MergerTree);
      fgets(cat->MergerTreeHeader[1], LINE_MAX, cat->MergerTree);
      fgets(cat->MergerTreeHeader[2], LINE_MAX, cat->MergerTree);
   }

   return cat;
}

void close_catalogs(struct Rockstar_Data *cat)
{
   int s;

   // If this is a new tree, we need to write the header of the merger tree.
   if (cat->status == WRITE)
   {
      fseek(cat->MergerTree, 0, SEEK_SET);
      fprintf(cat->MergerTree, "%s", cat->MergerTreeHeader[0]);
      fprintf(cat->MergerTree, "%s", cat->MergerTreeHeader[1]);
      fprintf(cat->MergerTree, "%"PRId64, cat->CountHalos);
   }

   fclose(cat->Forest); 
   fclose(cat->MergerTree);
   for (s=0; s<cat->Nsnaps; s++)
   {
      fclose(cat->Catalogs[s]);
      free(cat->CatalogHeader[s]);
   }
   free(cat->Catalogs);
   free(cat->CatalogHeader);

   free(cat->Z);
   free(cat->Nhalos);

   free(cat);
}


int64_t read_next_forest(struct Rockstar_Data *cat)
{
   int s;
   
   if (cat->status == READ)
   {
      if (!feof(cat->Forest))
      {
         fscanf(cat->Forest, "%"SCNd64, &cat->FID);       // id
         fscanf(cat->Forest, "%"SCNd64, &cat->NhalosTot); // nhalos

         for (s=0; s<cat->Nsnaps; s++)
            fscanf(cat->Forest, "%"SCNd64, &cat->Nhalos[s]);
         fscanf(cat->Forest, "\n");

         cat->CountForests++;
      }
      else
         cat->FID = -1;
   }
   else
   {
      fprintf(stderr, "ERROR: Cannot read forest in these catalogs: %s\n", cat->Path);
      exit(EXIT_FAILURE);
   }
   return cat->FID;
}

void skip_forest_halos(struct Rockstar_Data *cat)
{
   int64_t i, p;
   int64_t i64tmp, nhalos;
   int s;
   char stmp[LINE_MAX];

   if (cat->status == READ)
   {
      /* Catalogs: */
      for (s=0; s<cat->Nsnaps; s++)
         for (i=1; i<=cat->Nhalos[s]; i++)
            fgets(stmp, LINE_MAX, cat->Catalogs[s]);

      /* Merger Tree */
      for (s=0; s<cat->Nsnaps; s++)
         for (i=1; i<=cat->Nhalos[s]; i++)
         {
            fscanf(cat->MergerTree, "%"SCNd64, &i64tmp);   
            fscanf(cat->MergerTree, "%"SCNd64"\n", &nhalos);

            for (p=1; p<=nhalos; p++)
               fscanf(cat->MergerTree, "%"SCNd64"\n", &i64tmp);   
         }
   }
   else
   {
      fprintf(stderr, "ERROR: Cannot read forest in these catalogs: %s\n", cat->Path);
      exit(EXIT_FAILURE);
   }
}

void copy_forest(struct Rockstar_Data *dest, struct Rockstar_Data *src)
{
   int s;
   int64_t i, p;
   int64_t id, nhalos;
   char stmp[LINE_MAX];

   if (src->status != READ)
   {
      fprintf(stderr, "ERROR: Cannot read forest in these catalogs: %s\n", src->Path);
      exit(EXIT_FAILURE);
   }
   if (dest->status != WRITE)
   {
      fprintf(stderr, "ERROR: Cannot write forest in these catalogs: %s\n", dest->Path);
      exit(EXIT_FAILURE);
   }

   // If this is the first forest, we must copy the headers:
   if (dest->CountForests == 0)
   {
      fprintf(dest->Forest, "%s", src->ForestHeader);
      sprintf(dest->ForestHeader, "%s", src->ForestHeader);
      for (s=0; s<src->Nsnaps; s++)
      {
         fprintf(dest->Catalogs[s], "%s", src->CatalogHeader[s]);
         sprintf(dest->CatalogHeader[s], "%s", src->CatalogHeader[s]);
      }

      // The tree must be filled at the end. Now we just create enough empy space.
      sprintf(dest->MergerTreeHeader[0], "%s", src->MergerTreeHeader[0]);
      sprintf(dest->MergerTreeHeader[1], "%s", src->MergerTreeHeader[1]);
      s=i=0;
      while (src->MergerTreeHeader[0][s] != '\0') s++;
      i+=s; s=0;
      while (src->MergerTreeHeader[1][s] != '\0') s++;
      i += s;
      i += 22;
      for (s=0; s<i; s++)
         fprintf(dest->MergerTree, " ");
      fprintf(dest->MergerTree, "\n");
   }

   /* The forest file */
   fprintf(dest->Forest, "%"PRId64" %"PRId64, src->FID, src->NhalosTot);
   for (s=0; s<src->Nsnaps; s++)
      fprintf(dest->Forest, " %"PRId64, src->Nhalos[s]);
   fprintf(dest->Forest, "\n");

   for (s=0; s<src->Nsnaps; s++)
      for (i=1; i<=src->Nhalos[s]; i++)
      {
         /* The catalogs */
         fgets(stmp, LINE_MAX, src->Catalogs[s]); 
         fputs(stmp, dest->Catalogs[s]); 

         /* The merger tree */ 
         fscanf(src->MergerTree, "%"SCNd64" %"SCNd64"\n", &id, &nhalos);
         fprintf(dest->MergerTree, "%"PRId64" %"PRId64"\n", id, nhalos);

         for (p=1; p<=nhalos; p++)
         {
            fgets(stmp, LINE_MAX, src->MergerTree);
            fputs(stmp, dest->MergerTree);
         }
      }

   dest->CountForests++;
   dest->CountHalos += src->NhalosTot;
}


#endif
