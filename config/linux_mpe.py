f90 = {}
cxx = {}
cc  = {}

is_arch_valid = 1

#flags_arch = '-g -Wall'
flags_arch = '-O3 -Wall'
flags_link = '-rdynamic -lpthread'

cc   = 'gcc'
f90  = 'gfortran'

libpath_fortran = ''
libs_fortran    = ['gfortran','lmpe','mpe']

charm_path  = '/home/bordner/Charm/charm'
papi_path   = '/usr/local'
hdf5_path   = '/usr'
