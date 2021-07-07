#!/usr/bin/env python3

import h5py
import sys
import pysussing as ps
import numpy as np

#dirname = "/home/cnvega/Desktop/Astro/Projects/Caterpillar/Sussing_ascii/H1725272/LX11"
#lastsnap = 239
#haloID = 239000000252179

dirname = "/home/cnvega/Desktop/Astro/Projects/Caterpillar/Sussing_ascii/H1725272/LX13"
lastsnap = 250
haloID = 250000011393997

cat = ps.Catalog(dirname, readMergerTree=True)

fout = open(str(haloID)+"_progs.dat", "w")

while(cat.forest_id):
   
   halos = cat.get_forest()
   
   if haloID in halos['ID']:
      mt = cat.get_progenitors()
      
      fout.write("# HaloID   Snapshot   NumProgs \n")
      
      hid = haloID
      nprogs = 1
      
      while(nprogs):
         idx = mt['HaloID'] == hid
         
         nprogs = mt['NumProgs'][idx][0]
         
         fout.write("{0:d}   {1:d}   {2:d} \n".format(hid, 
            halos['snapshot'][idx][0], nprogs))
         
         # next halo:
         if nprogs:
            hid = mt['Progenitors'][mt['FirstProg'][idx]][0]

      break
   cat.next_forest()  

fout.close()

