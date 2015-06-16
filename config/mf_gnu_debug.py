import os

is_arch_valid = 1

flags_arch = '-Wall -O0 -g'
flags_link_charm = ' -rdynamic' # required for backtraces

cc  = 'gcc'
f90 = 'gfortran'

flags_prec_single = '-fdefault-real-4 -fdefault-double-8'
flags_prec_double = '-fdefault-real-8 -fdefault-double-8'

libpath_fortran = '.'
libs_fortran    = ['gfortran']

home = os.environ['HOME']

charm_path   = home + '/sw/charm-6.6.1/gnu_debug'
#papi_path    = '/usr/local'
hdf5_path    = home + '/sw/yt_dev'
png_path     = home + '/sw/yt_dev'
#grackle_path = home + '/Software/Grackle/src/clib'

