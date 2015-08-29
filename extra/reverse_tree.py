#!/usr/bin/env python

import sys

class Halo:
   """A halo with their progenitors"""
   def __init__(self, hid):
      self.hid = hid
      self.nprog = 0
      self.plist = []

   def addProg(self, hid):
      self.nprog += 1
      self.plist.append(hid)

   def dumpHalo(self, f):
      f.write(self.hid + " " + str(self.nprog) + '\n') 
      for i in range(self.nprog):
         f.write(self.plist[i])

class MergerTree():
   """A merger tree"""
   def __init__(self):
      self.header = []
      self.halos = []
      self.nhalos = 0

   def addHalo(self, halo):
      self.halos.append(halo)
      self.nhalos += 1

   def readFile(self, fname):
      f= open(fname, 'r')
      self.header.append(f.readline())
      self.header.append(f.readline())
      self.header.append(f.readline())
      line = f.readline()
      while line != "END\n":
         hid, np = line.split()
         self.addHalo(Halo(hid))
         for i in range(int(np)):
            prog = f.readline()
            self.halos[self.nhalos-1].addProg(prog)
         line = f.readline()
      f.close()
   
   def writeFile(self, fname):
      f = open(fname, 'w')
      f.write(self.header[0])
      f.write(self.header[1])
      f.write(str(self.nhalos)+'\n')
      for halo in self.halos:
         halo.dumpHalo(f)
      f.write("END\n")

if __name__ == "__main__":
   mt = MergerTree()
   mt.readFile(sys.argv[1])
   mt.halos.reverse()
   mt.writeFile("newTree.list")

