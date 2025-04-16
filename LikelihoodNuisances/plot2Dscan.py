#!/usr/bin/env python
import ROOT
import math
import json
import argparse
import os


parser = argparse.ArgumentParser()
parser.add_argument('--input', '-i', help='input ROOT file')
parser.add_argument('--poi',   '-p', help='the name of the POI')
args = parser.parse_args()

# get list of nuisances
print " input: ", args.input
print " poi: ", args.poi
f_input = ROOT.TFile.Open(args.input, "read")
tree = f_input.Get("limit")

list_branches = tree.GetListOfBranches()
#print "branches = ", list_branches

for branches in list_branches:
  print branches.GetName()
   
  if branches.GetName() != "limit"  and branches.GetName() != "mh" \
         and branches.GetName() != "limitErr"   and branches.GetName() != "syst"   and branches.GetName() != "iToy" \
         and branches.GetName() != "iSeed"      and branches.GetName() != "iChannel" \
         and branches.GetName() != "t_cpu" \
         and branches.GetName() != "t_real" \
             :
    toDo = "root -l -q -b " +  args.input + "  Draw2DLine.cxx\(\\\"poi\\\",\\\""  +   branches.GetName()  + "\\\",\\\""+ args.poi +"\\\",\\\"" +  branches.GetName()   + "\\\",\\\"" +   branches.GetName()   + "\\\"\) "
    #toDo = "root -l -q -b " +  args.input + "  Draw2DLine.cxx\(\\\"#mu\\\",\\\""  +   branches.GetName()  + "\\\",\\\""+ args.poi +"\\\",\\\"" +  branches.GetName()   + "\\\",\\\"" +   branches.GetName()   + "\\\"\) "
    print toDo
    os.system (toDo)
  
  