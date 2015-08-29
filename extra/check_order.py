#! /usr/bin/env python

import sys

def check_order(fname):
   try: 
      f = open(fname, "r")
   except IOError:
      print ("Cannot open file!")
      return

   for i, line in enumerate(f.readlines()):
      hid, idx = line.split()
      if i > 0 and idx < idx_old:
         print ("Unordered halos!")
         return
      idx_old = idx
   f.close()
   print("Check passed!")

if __name__ == "__main__":
   check_order(sys.argv[1]) 
      
