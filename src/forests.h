#ifndef _FORESTS_H
#define _FORESTS_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define LINE_MAX 1024
#define BAD_VALUE 2e38

enum Status {READ, WRITE, READB, WRITEB};

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

struct Halo_Data {
/*Los comentados son los que NO estan definidos. Asi ahorramos disco al reescribir:*/
   int64_t ID;           //(1) 
   int64_t hostHalo;     //(2)
   int64_t numSubStruct; //(3)
   float   M200c;        //(4)
   int64_t npart;  //(5)
   float   X[3]; //(6,7,8)
   float   Vc[3]; //(9,10,11)
   float   R200c; //(12)
   float   Rmax;  //(13)
//   float   r2;    //(14)
//   float   mbp_offset;  //(15) 
//   float   com_offset;  //(16)
   float   Vmax;        //(17) 
//   float   v_esc;       //(18) 
   float   sigV;     //(19)
//   float   lambda;   //(20) 
   float   lambdaE;  //(21)
   float   L[3];     //(22,23,24)
   float   cNFW;   //(25)
};

/* It creates or read an existing rockstar set of catalogs: */
struct Rockstar_Data* 
open_catalogs(char *basename, float *snaps, int nsnaps, enum Status status);

/* Close the full structure */
void close_catalogs(struct Rockstar_Data *cat);

int64_t read_next_forest(struct Rockstar_Data *cat);

void skip_forest_halos(struct Rockstar_Data *cat);

void copy_forest(struct Rockstar_Data *dest, struct Rockstar_Data *src);

void parse_string_to_halo(struct Halo_Data *halo, char *line);

void parse_halo_to_string(char *line, struct Halo_Data *halo);

#endif
