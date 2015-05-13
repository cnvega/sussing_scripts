#ifndef _FORESTS_H
#define _FORESTS_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define LINE_MAX 1024

enum Status {READ, WRITE};

struct Rockstar_Data {
   int   Nsnaps;
   float *Z;                 // List of redshifts
   char  Path[LINE_MAX];     // Base path of the catalogs

   /* FILE pointers: */
   FILE *Forest;
   FILE *MergerTree;
   FILE **Catalogs;
  
   /* Headers: */
   char ForestHeader[LINE_MAX];
   char **CatalogHeader;     
   char MergerTreeHeader[3][LINE_MAX];
   
   /* Current forest: */
   int64_t FID;             // id
   int64_t NhalosTot;       // Number of halos 
   int64_t *Nhalos;         // Number of halos in each snapshot

   /* Globals */
   int64_t CountForests;
   int64_t CountHalos;
   enum Status status;
};

/* It creates or read an existing rockstar set of catalogs: */
struct Rockstar_Data* 
open_catalogs(char *basename, float *snaps, int nsnaps, enum Status status);

/* Close the full structure */
void close_catalogs(struct Rockstar_Data *cat);

int64_t read_next_forest(struct Rockstar_Data *cat);

void skip_forest_halos(struct Rockstar_Data *cat);

void copy_forest(struct Rockstar_Data *dest, struct Rockstar_Data *src);


#endif
