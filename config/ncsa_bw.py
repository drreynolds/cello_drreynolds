f90 = {}
cxx = {}
cc  = {}

is_arch_valid = 1

flags_arch = '-O3'
flags_link  = '-O3'

cc   = 'cc'
f90  = 'ftn'

libpath_fortran = ''
libs_fortran    = ['gfortran']

#libpng_path  = '/u/sciteam/bordner'

charm_path = '/u/sciteam/bordner/Charm/charm'
papi_path  = '/u/sciteam/bordner'
hdf5_path  = '/opt/cray/hdf5/default/cray/74'

if (type == "mpi"):
   parallel_run = "aprun -n 8"
