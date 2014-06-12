import os

is_arch_valid = 1

#flags_arch = '-Wall -O3'
flags_arch = '-Wall -O3 -g'
# flags_arch = '-Wall -g'       
# flags_arch = '-O3 -pg'       

# -rdynamic: required for backtraces
flags_link_charm = ' -rdynamic'

cc  = 'gcc '
f90 = 'gfortran'

libpath_fortran = '.'
libs_fortran    = ['gfortran']

home = os.environ['HOME']

charm_path  = home + '/Charm/charm'
papi_path   = '/usr/local'
hdf5_path   = '/usr'
png_path    = '/lib/x86_64-linux-gnu'

