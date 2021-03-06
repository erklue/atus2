#!/usr/bin/env python

# ATUS2 - The ATUS2 package is atom interferometer Toolbox developed at ZARM
# (CENTER OF APPLIED SPACE TECHNOLOGY AND MICROGRAVITY), Germany. This project is
# founded by the DLR Agentur (Deutsche Luft und Raumfahrt Agentur). Grant numbers:
# 50WM0942, 50WM1042, 50WM1342.
# Copyright (C) 2017 Želimir Marojević, Ertan Göklü, Claus Lämmerzahl
#
# This file is part of ATUS2.
#
# ATUS2 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ATUS2 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ATUS2.  If not, see <http://www.gnu.org/licenses/>.

import os,sys,struct 

import numpy as np

noargs = len(sys.argv)

if ( noargs != 2 ):
  print( "No filename specified." )
  exit()

fh = open(sys.argv[1], 'rb' )

header_raw = fh.read(struct.calcsize("llllllliidddddddddddddd"))
header = struct.unpack( "llllllliidddddddddddddd", header_raw )
nDims = header[3]
nDimX = header[4]
nDimY = header[5]
nDimZ = header[6]
bCmpx = header[8]
t     = header[9]
xMin  = header[10]
xMax  = header[11]
yMin  = header[12]
yMax  = header[13]
zMin  = header[14]
zMax  = header[15]
dx    = header[16]
dy    = header[17]
dz    = header[18]
dkx   = header[19]
dky   = header[20]
dkz   = header[21]
dt    = header[22]

#print("%s\n" % type(header_raw))

if ( header[0] != 1380 ):
  print( "Invalid file format." )
  exit()

if ( bCmpx != 1 ):
  print( "File does not contain complex data." )
  exit()

print("sizeof(generic_header) = %ld\n" % (header[0]))
print("dims = (%ld,%ld,%ld)\n" % (nDimX,nDimY,nDimZ))
print("xrange = (%g,%g)\n" % (xMin,xMax))
print("yrange = (%g,%g)\n" % (yMin,yMax))
print("zrange = (%g,%g)\n" % (zMin,zMax))
print("t = %g\n" % (t))

fh.seek(header[0],0)

cmplxsize = struct.calcsize("dd")

if (nDims == 1):
  data = np.zeros((nDimX),dtype=np.complex_) 
  for i in range(0, nDimX):
    rawcplxno = fh.read(cmplxsize)
    cmplxno = struct.unpack( "dd", rawcplxno )
    #print("(%g,%g)\n" % (cmplxno[0],cmplxno[1]))
    data[i] = complex(cmplxno[0],cmplxno[1])

if (nDims == 2):
  data = np.zeros((nDimX,nDimY),dtype=np.complex_) 
  for i in range(0, nDimX):
    for j in range(0, nDimY):
      rawcplxno = fh.read(cmplxsize)
      cmplxno = struct.unpack( "dd", rawcplxno )
      #print("%s\n" % type(cmplxno))
      #print("(%d,%d) (%g,%g)\n" % (i,j,cmplxno[0],cmplxno[1]))
      data[i,j] = complex(cmplxno[0],cmplxno[1])

if (nDims == 3):
  data = np.zeros((nDimX,nDimY,nDimZ),dtype=np.complex_) 
  for i in range(0, nDimX):
    for j in range(0, nDimY):
      for k in range(0, nDimZ):
        rawcplxno = fh.read(cmplxsize)
        cmplxno = struct.unpack( "dd", rawcplxno )
        #print("(%d,%d,%d) (%g,%g)\n" % (i,j,k,cmplxno[0],cmplxno[1]))
        data[i,j,k] = complex(cmplxno[0],cmplxno[1])

fh.close()
