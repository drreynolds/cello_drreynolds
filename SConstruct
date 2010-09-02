import os
import sys

TOPDIR  = GetLaunchDir()
# charm_path = '/home/bordner/charm/charm-6.2.1'
charm_path = '/home/bordner/charm/charm'

# Initialize platform

platform='unknown'

platform = ARGUMENTS.get('platform','unknown')

if (platform == 'unknown' and "CELLO_PLATFORM" in os.environ):
     platform = os.environ["CELLO_PLATFORM"]

#==================================================
# Check code for revision updates and local modifications
#==================================================

repository = "svn+ssh://client65-88.sdsc.edu/usr/local/svn/cello/trunk"
get_revision = "| awk '/Revision:/ {print $2}'"

# TODO: TEST FOR INTERNET CONNECTION

# revision_new     = int(os.popen("svn info " + repository + get_revision).read())
revision_current = int(os.popen("svn info " + get_revision).read())
revision_changes = int(os.popen("svn status | grep -v '?' | wc -l").read())

if (revision_changes != 0):
   print "\nWARNING: Working directory has local modifications\n"
# elif (revision_new != revision_current):
#    print "\nWARNING: Working directory is not up-to-date with the repository\n"
else:
   print "\nWorking directory synched with repository\n"

# --------------------------------------------------

#==================================================
# Initialize environment according to platform
#==================================================

#--------------------------------------------------
if (platform == 'linux-serial'):
#--------------------------------------------------
   parallel_run = ""
   parallel_type = "serial"
   serial_run   = ""
   env = Environment (
      CC          = 'gcc',	
      CPPDEFINES = ['NO_FREETYPE'],
      CPPPATH     = '#/include',
      CXXFLAGS    = '-Wall -g  -m128bit-long-double',
      CFLAGS      = '-Wall -g  -m128bit-long-double',
      CXX         = 'g++',	
      ENV         = os.environ,
      FORTRAN     = 'gfortran',
      FORTRANLIBS = 'gfortran',
      FORTRANPATH = '#/include',
      LIBPATH     = '#/lib',
   )
#--------------------------------------------------
elif (platform == 'linux-mpi'):
#--------------------------------------------------
   parallel_run = "mpirun -np 4"
   serial_run   = ""
   parallel_type = "mpi"
   env = Environment (
      CC          = 'mpicc',	
      CPPDEFINES = ['NO_FREETYPE','CONFIG_USE_MPI'],
      CPPPATH     = '#/include',
      CXXFLAGS    = '-Wall -g  -m128bit-long-double',
      CFLAGS      = '-Wall -g  -m128bit-long-double',
      CXX         = 'mpiCC',	
      ENV         = os.environ,
      FORTRAN     = 'gfortran',
      FORTRANLIBS = 'gfortran',
      FORTRANPATH = '#/include',
      LIBPATH     = '#/lib',
   )
#--------------------------------------------------
elif (platform == 'linux-mpi-valgrind'):
#--------------------------------------------------
   parallel_run = "mpirun -np 4 valgrind"
   serial_run   = "valgrind "
   parallel_type = "mpi"
   env = Environment (
      CC          = 'mpicc',	
      CPPDEFINES = ['NO_FREETYPE','CONFIG_USE_MPI'],
      CPPPATH     = '#/include',
      CXXFLAGS    = '-Wall -g  -m128bit-long-double',
      CFLAGS      = '-Wall -g  -m128bit-long-double',
      CXX         = 'mpiCC',	
      FORTRAN     = 'gfortran',
      FORTRANLIBS = 'gfortran',
      FORTRANPATH = '#/include',
      LIBPATH     = '#/lib',
   )
#--------------------------------------------------
elif (platform == 'linux-ampi'):
#--------------------------------------------------
   parallel_run = charm_path + "/bin/charmrun +p4 "
   serial_run   = ""
   parallel_type = "ampi"
  
   env = Environment(
      CC          = charm_path + '/bin/charmc -language ampi',
      CPPDEFINES = ['NO_FREETYPE','CONFIG_USE_MPI'],
      CPPPATH     = '#/include',
      CXX         = charm_path + '/bin/charmc -language ampi',
      CXXFLAGS    = '-g',
      CFLAGS      = '-g',
      ENV         = os.environ,
      FORTRANFLAGS  = '-g',
      FORTRAN     = 'gfortran',
      FORTRANLIBS = 'gfortran',
      FORTRANPATH = '#/include',
      LINKFLAGS     = '-g',
      LIBPATH     = '#/lib',
   )
#--------------------------------------------------
elif (platform == 'linux-charm' or platform == 'linux-charm-perf'):
#--------------------------------------------------

   opt_flags = '-O3 '

#   debug_flags = '-memory charmdebug'
   debug_flags = ''

   if (platform == 'linux-charm-perf'):
	perf_flags = '-tracemode projections'
   else:
	perf_flags = ''

   flags = opt_flags + ' ' + debug_flags + ' ' + perf_flags + ' '
   parallel_run = charm_path + "/bin/charmrun +p4 "
   parallel_type = "charm"
   serial_run   = ""
  
   env = Environment(
      CC          = charm_path + '/bin/charmc -language charm++ '+flags,
      CPPDEFINES = ['NO_FREETYPE','CONFIG_USE_CHARM'],
      CPPPATH     = '#/include',
      CXX         = charm_path + '/bin/charmc -language charm++ '+flags,
      CXXFLAGS    = flags,
      CFLAGS      = flags,
      ENV         = os.environ,
      FORTRAN     = 'gfortran',
      FORTRANFLAGS = flags,
      FORTRANLIBS = 'gfortran',
      FORTRANPATH = '#/include',
      LIBFLAGS     = flags,
      LIBPATH     = '#/lib' )
   charm_builder = Builder (action="${CXX} $SOURCE; mv ${ARG}.*.h include")
   env.Append(BUILDERS = { 'CharmBuilder' : charm_builder })

#--------------------------------------------------
elif (platform == 'triton'):
#--------------------------------------------------
   parallel_run = "/opt/openmpi_pgimx/bin/mpirun -np 4 "
   serial_run   = ""
   parallel_type = "mpi"
   env = Environment (
      CC      = 'mpicc',	
      CPPDEFINES = ['NO_FREETYPE','CONFIG_USE_MPI'],
      CPPPATH = ['#/include', '/opt/pgi/hdf5_pgi/include'],
      CXXFLAGS = '-g -DH5_USE_16_API',
      CFLAGS   = '-g -DH5_USE_16_API',
      CXX     = 'mpicxx',	
      FORTRAN = 'mpif90',
      FORTRANPATH = '#/include',
      LIBPATH = ['#/lib', '/opt/pgi/hdf5_pgi/lib'],
      LINKFLAGS = '-pgf90libs',
      ENV = {'PATH' : os.environ['PATH'], 'LM_LICENSE_FILE' : os.environ['LM_LICENSE_FILE']},
   )
#--------------------------------------------------
elif (platform == 'ncsa-bd'):
#--------------------------------------------------
   parallel_run = charm_path + "/bin/charmrun +p4 "
   opt_flags = '-O3 -qhot -q64'
   debug_flags = ''

   flags = opt_flags + ' ' + debug_flags + ' '
   serial_run    = ""
   cc = '/opt/ibmcmp/vac/11.1/bin/xlc_r'
   fc = '/opt/ibmcmp/xlf/13.1/bin/xlf_r'

   parallel_type = "charm"
   env = Environment (
      ARFLAGS  = 'r',
      CCFLAGS = flags,
      CC      = cc,
      CPPDEFINES = ['NO_FREETYPE','H5_USE_16_API'],
#      CPPDEFPREFIX = '-WF,-D',
      CPPPATH = ['/home/bordner/include', '#/include'],
      CXX     = cc,	
      DEFINES = '',
      FORTRANFLAGS = flags + '-I include',
      FORTRANLIBS = 'xlf90',
      FORTRAN = fc,
      LIBPATH = ['#/lib','/home/bordner/lib','/opt/ibmcmp/xlf/13.1/lib64'],
      LINKFLAGS  = flags
   )
#--------------------------------------------------
elif (platform == 'ncsa-bd-serial'):
#--------------------------------------------------
   opt_flags = '-O3 -qhot -q64'
   debug_flags = ''
   defines = '-D NO_FREETYPE -DH5_USE_16_API'
   flags = opt_flags + ' ' + debug_flags + ' '
   parallel_run = ""
   serial_run    = ""
   cc = '/opt/ibmcmp/vac/11.1/bin/xlc'
   fc = '/opt/ibmcmp/xlf/13.1/bin/xlf'

   parallel_type = "serial"
   env = Environment (
      ARFLAGS  = 'r',
      CCFLAGS = flags + defines,
      CC      = cc,
#      CPPDEFINES = ['NO_FREETYPE','H5_USE_16_API'],
#      CPPDEFPREFIX = '-WF,-D',
      CPPPATH = ['/home/bordner/include', '#/include'],
      CXX     = cc,	
      DEFINES = '',
      FORTRANFLAGS = flags + '-I include',
      FORTRANLIBS = 'xlf90',
      FORTRAN = fc,
      LIBPATH = ['#/lib','/home/bordner/lib','/opt/ibmcmp/xlf/13.1/lib64'],
      LINKFLAGS  = flags
   )
else:
   print
   print "**********************************************************************"
   print
   print "Platform '",platform,"' is not recognized.  To specify the platform, either:"
   print
   print "1) Set the 'CELLO_PLATFORM' environment variable,"
   print
   print "   or"
   print
   print "2) Use the 'platform=<platform>' scons argument"
   print
   print "Recognized platforms are:"
   print
   print "   linux-mpi    Linux with MPI"
   print "   linux-ampi   Linux with AMPI (Charm++)"
   print "   triton       SCSD Triton Resource"
   print "   ncsa-bd      NCSA Blue Drop"
   print
   print "**********************************************************************"
   print
   sys.exit()
   env = Environment (
      CPPPATH = ['#/include'],
      LIBPATH = ['#/lib'],
   )

# Generate the CELLO_PLATFORM file (should be a target)

f = open('CELLO_PLATFORM','w')
f.write(platform + '\n')
f.close()


Export('env')
Export('platform')
Export('parallel_type')
Export('parallel_run')
Export('serial_run')

SConscript('src/SConscript')
SConscript('test/SConscript')

# Build tarball

# creates cello-#.#.#.tar.gz with (bin include lib)

# env = Environment(tools=['default', 'packaging'])
# env.Package( NAME           = 'cello',
#              VERSION        = '0.1.0',
#              PACKAGEVERSION = 0,
#              PACKAGETYPE    = 'targz',
#              LICENSE        = 'New BSD',
#              SUMMARY        = 'Cello Extreme AMR Framework',
#              DESCRIPTION    = 'Cello Extreme AMR Framework'
#         )

