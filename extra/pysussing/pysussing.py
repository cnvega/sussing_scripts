#!/usr/bin/env python3

import os
import struct
import numpy as np

class Halo:
   def __init__(self, props=None):
      if not props: props = [0]*20
      self.ID = np.int64(props[0])
      self.hostHalo = np.int64(props[1])
      self.numSubStruct = np.int64(props[2])
      self.M200c = np.float32(props[3])
      self.npart = np.int64(props[4])
      self.X = [np.float32(props[5]), np.float32(props[6]), np.float32(props[7])]
      self.Vc = [np.float32(props[8]), np.float32(props[9]), np.float32(props[10])]
      self.R200c = np.float32(props[11])
      self.Rmax = np.float32(props[12])
      self.Vmax = np.float32(props[13])
      self.sigV = np.float32(props[14])
      self.lambdaE = np.float32(props[15])
      self.L = [np.float32(props[16]), np.float32(props[17]), np.float32(props[18])]
      self.cNFW = np.float32(props[19])

class _ascii_prop_type:
   def __init__(self):
      self.dtype = [('ID','i8'), ('hostHalo','i8'), ('numSubStruct','i8'), 
         ('M200c','f4'), ('npart','i8'), ('Xx','f4'), ('Xy','f4'), ('Xz','f4'), 
         ('Vcx','f4'), ('Vcy','f4'), ('Vcz','f4'), ('R200c','f4'), ('Rmax','f4'), 
         ('Vmax','f4'), ('sigV','f4'), ('lambdaE','f4'), 
         ('Lx','f4'), ('Ly','f4'), ('Lz','f4'), ('cNFW','f4')]
   def __getitem__(self, key):
      #if key in [0,1,2,4]: return np.int64
      #if key == 3 or (4 < key < 20): return np.float32
      if key in [0,1,2,4]: return np.int64
      elif key in [3,5,6,7,8,9,10,11,12,16,18,20,21,22,23,24]: return np.float32
      else: None


#htag = {'ID':0, 'hostHalo':1, 'numSubStruct':2, 'M200c':3, 'npart':4,
#      'X':(5,6,7), 'Vc':(8,9,10), 'R200c':11, 'Rmax':12, 'Vmax':13,
#      'sigV':14, 'lambdaE':15, 'L':(16,17,18), 'cNFW':19}
#
#asciipsr = np.array([True]*25, dtype=np.bool)
#asciipsr[np.array([14,15,16,18,20])] = False

class Catalog:
   def __init__(self, dirname, binary=False):
     
      filenames = os.listdir(dirname)
      self.binary = binary
      self.Nsnaps = 0

      self.ptype = _ascii_prop_type()

      if binary:
         catnames = [i for i in filenames if i[-4:]=='.dat' and i[11:13]=='.z']
      else:
         catnames = [i for i in filenames if i[-10:]=='.AHF_halos']
     
      if len(catnames) == 0:
         raise IOError("No catalogs found in "+dirname)
         exit()

      catnames.sort()
      
      if binary:
         forestname, treename = 'sussing_forests.dat', 'sussing_tree.dat'
      else:
         forestname, treename = 'sussing_forests.list', 'sussing_tree.list'

      if dirname[-1] != '/': dirname += '/'
      flag = 'r'
      if binary: flag += 'b'
      
      self.forests = open(dirname+forestname, flag)
      self.trees = open(dirname+treename, flag)
      
      self.catalogs = [None]*len(catnames)
      for i, fname in enumerate(catnames):
         self.catalogs[i] = open(dirname+fname, flag)
         if int(fname[8:11]) != i:
            raise RuntimeError("Catalog indexes are not consistent in internal array.")
      
      self.Nsnaps = len(catnames)

      # start the reading counter
      self.readcount = np.zeros(self.Nsnaps, dtype=np.int64)
      
      # skip the headers and load the first forest:
      self.reset()
    

   def __del__(self):
      if self.Nsnaps > 0:
         self.forests.close()
         self.trees.close()
         for cat in self.catalogs: cat.close()

   def reset(self):
      self.forests.seek(0)
      self.trees.seek(0)
      for cat in self.catalogs: cat.seek(0)
      # Skip the headers
      if self.binary:
         _ = self.forests.read(1024) 
         _ = self.trees.read(1024*3)
         for cat in self.catalogs: _ = cat.read(1024)
      else:
         _ = self.forests.readline()
         for i in range(3): _ = self.trees.readline()
         for cat in self.catalogs: _ = cat.readline()
      self._read_nhalos_forest()

   def _read_nhalos_forest(self):
      if self.binary:
         buf = self.forests.read(8*2)
         if not buf:
            self.forest_id, self.forest_tot, self.forest_nhs = 0,0,[None]
            return
         self.forest_id, self.forest_tot = struct.unpack("qq", buf)
         self.forest_nhs = struct.unpack("q"*self.Nsnaps, self.forests.read(8*self.Nsnaps))
      else:
         line = self.forests.readline()
         if not line: 
            self.forest_id, self.forest_tot, self.forest_nhs = 0,0,[None]
            return
         line = [int(i) for i in line.split()]
         self.forest_id, self.forest_tot = line[0], line[1]
         self.forest_nhs = line[2:]
      for i in range(self.Nsnaps):
         self.readcount[i] = self.forest_nhs[i]
         

   def _read_halo(self, snap):
      if self.readcount[snap]:
         if self.binary:
            # I needed to add the padding bytes explicitly here...
            halop = struct.unpack("qqqf4xq"+"f"*15+"4x", self.catalogs[snap].read(104))
            halop += tuple([snap])
         else:
            entries = self.catalogs[snap].readline().split()
            halop = [self.ptype[i](p) for i, p in enumerate(entries) if self.ptype[i]]
            halop = tuple(halop+[snap])
         self.readcount[snap] -= 1
         return halop

   def get_halos_snap(self, snap):
      h = []
      for _ in range(self.readcount[snap]):
         h.append(self._read_halo(snap))
      #return h
      return np.array(h, dtype=self.ptype.dtype+[('snapshot','i4')])

   def get_forest(self):
      h = []
      for snap in range(self.Nsnaps):
         for _ in range(self.readcount[snap]):
            h.append(self._read_halo(snap))
      return np.array(h, dtype=self.ptype.dtype+[('snapshot','i4')])

   def next_forest(self):
      # Read remaining halos:
      for snap in range(self.Nsnaps):
         for _ in range(self.readcount[snap]):
            _ = self._read_halo(snap)
      self._read_nhalos_forest()

