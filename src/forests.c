#ifndef _FORESTS_C
#define _FORESTS_C

#include "forests.h"

struct Rockstar_Data* 
open_catalogs(char *basename, float *snaps, int nsnaps, enum Status status)
{
   char fname[LINE_MAX];
   char st; 
   int s, i;
   struct Rockstar_Data *cat;
   int64_t i64tmp;

   cat = malloc(sizeof(struct Rockstar_Data));

   if (status == READ || status == READB )
      st = 'r';
   if (status == WRITE || status == WRITEB )
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
   if (status == READ  || status == WRITE )
      sprintf(fname, "%s/sussing_forests.list", cat->Path);
   if (status == READB || status == WRITEB )
      sprintf(fname, "%s/sussing_forests.dat", cat->Path);

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
   if (status == READB)
      fread(cat->ForestHeader, sizeof(char), LINE_MAX, cat->Forest);

   if (status == WRITEB)
      for (i=0; i<LINE_MAX; i++)
         cat->ForestHeader[i] = '\0';

   /*** The catalogs ***/
   cat->Catalogs = malloc(cat->Nsnaps*sizeof(FILE *));
   for (s=0; s<cat->Nsnaps; s++)
   {
      if (status == READ  || status == WRITE )
         sprintf(fname, "%s/sussing_%03d.z%.3f.AHF_halos", cat->Path, s, cat->Z[s]);
      if (status == READB || status == WRITEB )
         sprintf(fname, "%s/sussing_%03d.z%.3f.dat", cat->Path, s, cat->Z[s]);
      if (!(cat->Catalogs[s] = fopen(fname, &st)))
      {
         fprintf(stderr, "Cannot open catalog file: %s\n", fname);
         exit(EXIT_FAILURE);
      }
      if (status == READ)
         fgets(cat->CatalogHeader[s], LINE_MAX, cat->Catalogs[s]);      
      if (status == READB)
         fread(cat->CatalogHeader[s], sizeof(char), LINE_MAX, cat->Catalogs[s]);      
      if (status == WRITEB)
         for (i=0; i<LINE_MAX; i++)
            cat->CatalogHeader[s][i] = '\0';
   }

   /*** The Merger Tree ***/
   if (status == READ  || status == WRITE )
      sprintf(fname, "%s/sussing_tree.list", cat->Path);
   if (status == READB || status == WRITEB )
      sprintf(fname, "%s/sussing_tree.dat", cat->Path);
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
   if (status == READB)
   {
      // Name, version, numhalos
      fread(cat->MergerTreeHeader[0], sizeof(char), LINE_MAX, cat->MergerTree);
      fread(cat->MergerTreeHeader[1], sizeof(char), LINE_MAX, cat->MergerTree);
      fread(&i64tmp, sizeof(int64_t), 1, cat->MergerTree);
   }
   if (status == WRITEB)
      for (s=0; s<3; s++)
         for (i=0; i<LINE_MAX; i++)
            cat->MergerTreeHeader[s][i] = '\0';

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
   if (cat->status == WRITEB)
   {
      fseek(cat->MergerTree, 2*LINE_MAX*sizeof(char), SEEK_SET);
      fwrite(&cat->CountHalos, sizeof(int64_t), 1, cat->MergerTree);
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
      if (fscanf(cat->Forest, "%"SCNd64, &cat->FID) != EOF)  // id
      {
         fscanf(cat->Forest, "%"SCNd64, &cat->NhalosTot); // nhalos

         for (s=0; s<cat->Nsnaps; s++)
            fscanf(cat->Forest, "%"SCNd64, &cat->Nhalos[s]);
         fscanf(cat->Forest, "\n");

         cat->CountForests++;
      }
      else
         cat->FID = -1;
   }
   else if (cat->status == READB)
   {
      if (fread(&cat->FID, sizeof(int64_t), 1, cat->Forest) != 0)  // id
      {
         fread(&cat->NhalosTot, sizeof(int64_t), 1, cat->Forest); // nhalos
         fread(&cat->Nhalos[0], sizeof(int64_t), cat->Nsnaps, cat->Forest);

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
   int64_t i64tmp, nhalos, mtree[2];
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
   else if (cat->status == READB)
   {
      /* Catalogs: */
      for (s=0; s<cat->Nsnaps; s++)
         fseek(cat->Catalogs[s], cat->Nhalos[s]*sizeof(struct Halo_Data), SEEK_CUR);

      /* Merger Tree */
      for (s=0; s<cat->Nsnaps; s++)
         for (i=1; i<=cat->Nhalos[s]; i++)
         {
            fread(mtree, sizeof(int64_t), 2, cat->MergerTree);
            fseek(cat->MergerTree, mtree[1]*sizeof(int64_t), SEEK_CUR);
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
   int64_t nhalos;
   int64_t htmp, ids[2]; 
   char stmp[LINE_MAX];
   struct Halo_Data halo_data;

   if (src->status != READ && src->status != READB)
   {
      fprintf(stderr, "ERROR: Cannot read forest in these catalogs: %s\n", src->Path);
      exit(EXIT_FAILURE);
   }
   if (dest->status != WRITE && dest->status != WRITEB)
   {
      fprintf(stderr, "ERROR: Cannot write forest in these catalogs: %s\n", dest->Path);
      exit(EXIT_FAILURE);
   }

   // If this is the first forest, we must copy the headers:
   if (dest->CountForests == 0)
   {
      sprintf(dest->ForestHeader, "%s", src->ForestHeader);
      for (s=0; s<src->Nsnaps; s++)
         sprintf(dest->CatalogHeader[s], "%s", src->CatalogHeader[s]);
      sprintf(dest->MergerTreeHeader[0], "%s", src->MergerTreeHeader[0]);
      sprintf(dest->MergerTreeHeader[1], "%s", src->MergerTreeHeader[1]);

      if (dest->status == WRITE)
      {
         fprintf(dest->Forest, "%s", src->ForestHeader);
         for (s=0; s<src->Nsnaps; s++)
            fprintf(dest->Catalogs[s], "%s", src->CatalogHeader[s]);

         // The tree must be filled at the end. Now we just create enough empty space.
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
      if (dest->status == WRITEB)
      {
         // Here we include the original headers of the files. Those aren't precise
         // because we drop a couple of columns in the catalogs but allow us to 
         // have backwards compatibility in the conversions.

         fwrite(dest->ForestHeader, sizeof(char), LINE_MAX, dest->Forest);
         for (s=0; s<src->Nsnaps; s++)
            fwrite(dest->CatalogHeader[s], sizeof(char), LINE_MAX, dest->Catalogs[s]);
         
         fwrite(dest->MergerTreeHeader[0], sizeof(char), LINE_MAX, dest->MergerTree);
         fwrite(dest->MergerTreeHeader[1], sizeof(char), LINE_MAX, dest->MergerTree);
         nhalos = 0;
         fwrite(&nhalos, sizeof(int64_t), 1, dest->MergerTree);
      }
   }

   /* Write the forest file */
   if (dest->status == WRITE)
   {
      fprintf(dest->Forest, "%"PRId64" %"PRId64, src->FID, src->NhalosTot);
      for (s=0; s<src->Nsnaps; s++)
         fprintf(dest->Forest, " %"PRId64, src->Nhalos[s]);
      fprintf(dest->Forest, "\n");
   }
   if (dest->status == WRITEB)
   {
      fwrite(&src->FID, sizeof(int64_t), 1, dest->Forest);
      fwrite(&src->NhalosTot, sizeof(int64_t), 1, dest->Forest);
      fwrite(src->Nhalos, sizeof(int64_t), src->Nsnaps, dest->Forest);
   }

   for (s=0; s<src->Nsnaps; s++)
      for (i=1; i<=src->Nhalos[s]; i++)
      {
         /* The catalogs and merger halo's headers */
         if (src->status == READ)
         {
            fgets(stmp, LINE_MAX, src->Catalogs[s]); 
            fscanf(src->MergerTree, "%"SCNd64" %"SCNd64"\n", &ids[0], &ids[1]);

            if (dest->status == WRITE)
            {
               fputs(stmp, dest->Catalogs[s]);

               fprintf(dest->MergerTree, "%"PRId64" %"PRId64"\n", ids[0], ids[1]);
            }
            if (dest->status == WRITEB)
            {
               parse_string_to_halo(&halo_data, stmp);
               fwrite(&halo_data, sizeof(struct Halo_Data), 1, dest->Catalogs[s]);

               fwrite(ids, sizeof(int64_t), 2, dest->MergerTree);
            }
         }
         if (src->status == READB)
         {
            fread(&halo_data, sizeof(struct Halo_Data), 1, src->Catalogs[s]);
            fread(ids, sizeof(int64_t), 2, src->MergerTree);

            if (dest->status == WRITE)
            {
               parse_halo_to_string(stmp, &halo_data);
               fputs(stmp, dest->Catalogs[s]);

               fprintf(dest->MergerTree, "%"PRId64" %"PRId64"\n", ids[0], ids[1]);
            }
            if (dest->status == WRITEB)
            {
               fwrite(&halo_data, sizeof(struct Halo_Data), 1, dest->Catalogs[s]);

               fwrite(&ids, sizeof(int64_t), 2, dest->MergerTree);
            }
         }

         /* The merger tree progenitors: */ 
         for (p=1; p<=ids[1]; p++)
         {
            if (src->status == READ)
               fscanf(src->MergerTree, "%"SCNd64"\n", &htmp);
            else if (src->status == READB)
               fread(&htmp, sizeof(int64_t), 1, src->MergerTree);

            if (dest->status == WRITE)
               fprintf(dest->MergerTree, "%"PRId64"\n", htmp);
            else if (dest->status == WRITEB)
               fwrite(&htmp, sizeof(int64_t), 1, dest->MergerTree);
         }
      }

   dest->CountForests++;
   dest->CountHalos += src->NhalosTot;
}

void parse_string_to_halo(struct Halo_Data *halo, char *line)
{
   float fbuf[5];

   sscanf(line, 
         "%"SCNd64" %"SCNd64" %"SCNd64" %g %"SCNd64   // 1-5
         " %f %f %f %f %f %f %f %f %f %f"             // 6-15
         " %f %f %f %f %f %f %f %f %f %f",            // 16-25
         &(halo->ID), &(halo->hostHalo), &(halo->numSubStruct),  // 1, 2, 3
         &(halo->M200c), &(halo->npart),         // 4, 5
         &(halo->X[0]), &(halo->X[1]), &(halo->X[2]), // 6,7,8
         &(halo->Vc[0]), &(halo->Vc[1]), &(halo->Vc[2]), // 9,10,11
         &(halo->R200c), &(halo->Rmax), &fbuf[0], &fbuf[1], &fbuf[2],  //12,13,14,15,16 
         &(halo->Vmax), &fbuf[3], &(halo->sigV), &fbuf[4], &(halo->lambdaE), //17,18,19,21
         &(halo->L[0]), &(halo->L[1]), &(halo->L[2]), //22,23,24
         &(halo->cNFW));          // (25)
}

void parse_halo_to_string(char *line, struct Halo_Data *halo)
{
   float fbuf = BAD_VALUE;

   sprintf(line, 
         "%"SCNd64" %"SCNd64" %"SCNd64" %g %"SCNd64   // 1-5
         " %f %f %f %f %f %f %f %f %g %g"             // 6-15
         " %g %f %g %f %g %f %f %f %f %f\n",            // 16-25
         halo->ID, halo->hostHalo, halo->numSubStruct,  // 1, 2, 3
         halo->M200c, halo->npart,         // 4, 5
         halo->X[0], halo->X[1], halo->X[2], // 6,7,8
         halo->Vc[0], halo->Vc[1], halo->Vc[2], // 9,10,11
         halo->R200c, halo->Rmax, fbuf, fbuf, fbuf,  //12,13,14,15,16 
         halo->Vmax, fbuf, halo->sigV, fbuf, halo->lambdaE, //17,18,19,21
         halo->L[0], halo->L[1], halo->L[2], //22,23,24
         halo->cNFW);          // (25)
}

void skip_and_check_forest(struct Rockstar_Data *cat)
{
   int64_t i, p;
   int64_t i64tmp, nhalos, mtree[2];
   int64_t haloid;
   int s;
   char stmp[LINE_MAX];

   if (cat->status == READ)
   {
      for (s=0; s<cat->Nsnaps; s++)
         for (i=1; i<=cat->Nhalos[s]; i++)
         {
            /* Merger Tree */
            fscanf(cat->MergerTree, "%"SCNd64, &haloid);   
            fscanf(cat->MergerTree, "%"SCNd64"\n", &nhalos);
            
            for (p=1; p<=nhalos; p++)
               fscanf(cat->MergerTree, "%"SCNd64"\n", &i64tmp);   

            /* Catalog */
            fscanf(cat->Catalogs[s], "%"SCNd64, &i64tmp);   
            fgets(stmp, LINE_MAX, cat->Catalogs[s]);

            if (i64tmp != haloid)
            {
               fprintf(stderr,"Error: The merger tree is inconsistent!\n");
               fprintf(stderr,"       Snap: %d id-cat: %"PRId64" id-mt: %"PRId64"\n",s, i64tmp, haloid);
               exit(EXIT_FAILURE);
            }
         }

   }
   else if (cat->status == READB)
   {
      fprintf(stderr, "ERROR: Not implemented yet!\n");
      exit(EXIT_FAILURE);
   }
   else
   {
      fprintf(stderr, "ERROR: Cannot read forest in these catalogs: %s\n", cat->Path);
      exit(EXIT_FAILURE);
   }
}

#endif
