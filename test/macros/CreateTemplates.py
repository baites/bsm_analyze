#! /usr/bin/env python
# Create tables 
#
# Developers:
#   Victor Eduardo Bazterra 2011 (UIC)
#
# Descrition:
#   Script to execute bsm_template over all the samples.

import os, shutil, sys
from optparse import OptionParser

usage = 'Usage: %s [options]' %sys.argv[0]
parser = OptionParser(usage=usage)

parser.add_option('-i', '--input', dest='input', default='SkimmedPBFileLists', help='Input directory with all the filelists.')
parser.add_option('-o', '--output', dest='output', default='output', help='Input directory with all the filelists.')
parser.add_option('-t', '--type', dest='type', default='nominal', help='Create templates for a given selection type (defalut = nominal).')
parser.add_option('-w', '--overwrite', action='store_true', dest='overwrite', default=False, help='Overwrite preexistent output files.')
parser.add_option('-r', '--recover', action='store_true', dest='recover', default=False, help='Recover missing root files.')

(options, args) = parser.parse_args()

# Sample files

datafiles = (
  'golden_single_el_2011a_aug5_rereco_v1',
  'golden_single_el_2011a_aug5_rereco_v1', 
  'golden_single_el_2011a_may10_rereco', 
  'golden_single_el_2011a_prompt_v4',
  'golden_single_el_2011a_prompt_v6', 
  'golden_single_el_2011b_prompt_v1',
)

mcfiles = (
  'qcd_bc_pt20to30',
  'qcd_bc_pt30to80',
  'qcd_bc_pt80to170',
  'qcd_em_pt20to30',
  'qcd_em_pt30to80',
  'qcd_em_pt80to170',
  'ttjets',
  'zjets',
  'wjets',
  'stop_s',
  'stop_t',
  'stop_tw',
  'satop_s',
  'satop_t',
  'satop_tw',
  'zprime_m1000_w10',
  'zprime_m1500_w15',
  'zprime_m2000_w20',
  'zprime_m3000_w30',
  'zprime_m4000_w40'  
)

#datafiles = ()
#mcfiles = (
#  'qcd_bc_pt20to30',
#  'qcd_bc_pt30to80',
#  'qcd_bc_pt80to170',
#  'qcd_em_pt20to30',
#  'qcd_em_pt30to80',
#  'qcd_em_pt80to170'
#)

# bsm_template options

nominal = '--leading-jet 250 --region signal --trigger hlt_ele45_caloidvt_trkidt --trigger hlt_ele65_caloidvt_trkidt --pileup Pileup/weight3d.root'

dataops = '--multi-thread 4'
datajec = '--l1 jec/data/L1FastJet.txt --l2 jec/data/L2Relative.txt --l3 jec/data/L3Absolute.txt --l2l3 jec/data/L2L3Residual.txt'

mcops = '--multi-thread 2'
mcjec = '--l1 jec/mc/L1FastJet.txt --l2 jec/mc/L2Relative.txt --l3 jec/mc/L3Absolute.txt'

input = options.input
output = options.output
recover = options.recover
overwrite = options.overwrite

# Create the output directory

type = ''
arguments = ''
if options.type == 'nominal':
  type = 'nominal'
  arguments = nominal
elif options.type == 'qcd':
  type = 'qcd'
  arguments = '--qcd-template 1 ' + nominal 
else:
  print 'Error: Undefined type.'
  sys.exit(1)

output = output+'/'+type

if not os.path.exists(output):
  os.makedirs(output)
elif overwrite:
  shutil.rmtree(output)
  os.makedirs(output)
elif recover:
  print 'Recovering missing files.'
else:
  print 'Overwrite protection: output file exists.'
  sys.exit(2)

# Run over the data samples

for datafile in datafiles:

  if not os.path.isfile(output+'/'+datafile+'.root'):
    command="bsm_template %s %s %s --output %s %s >& %s" % (
      dataops, datajec, arguments, output+'/'+datafile+'.root', input+'/'+datafile+'.txt', output+'/'+datafile+'.log'
    )
    print 'Executing: %s' % command
    os.system(command)

# Run over mc samples

for mcfile in mcfiles:

  if not os.path.isfile(output+'/'+mcfile+'.root'):  
    command="bsm_template %s %s %s --output %s %s >& %s" % (
      mcops, mcjec, arguments, output+'/'+mcfile+'.root', input+'/'+mcfile+'.txt', output+'/'+mcfile+'.log'
    )
    print 'Executing: %s' % command
    os.system(command)

