#!/usr/bin/env python3

import h5py
import sys
import pysussing as ps

#dirname = "/home/cnvega/Desktop/Astro/Projects/Caterpillar/Sussing_ascii/H1725272/LX11"
#lastsnap = 239
#haloID = 239000000252179
dirname = "/home/cnvega/Desktop/Astro/Projects/Caterpillar/Sussing_ascii/H1725272/LX13"
lastsnap = 250
haloID = 250000011393997
cat = ps.Catalog(dirname)

fout = open(str(haloID)+".dat", "w")

while(cat.forest_id):
   # Load last snapshot:
   hl = cat.get_halos_snap(lastsnap)
   if haloID in hl['ID']:
      hrem = cat.get_forest()
      
      fout.write("# "+str(cat.forest_id)+" "+str(cat.forest_tot)+"\n")
      for halo in hrem: 
         fout.write(str(halo['ID'])+"   "+str(halo['snapshot'])+"\n")
      for halo in hl: 
         fout.write(str(halo['ID'])+"   "+str(halo['snapshot'])+"\n")
      break 
   cat.next_forest()  

fout.close()

