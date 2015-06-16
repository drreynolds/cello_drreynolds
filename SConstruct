import os
import sys
import subprocess
import time
import socket

#======================================================================
# USER CONFIGURATION
#======================================================================

#----------------------------------------------------------------------
# Whether to print out detailed messages with the TRACE() series of statements
#----------------------------------------------------------------------

trace = 0

#----------------------------------------------------------------------
# Whether to trace main phases
#----------------------------------------------------------------------

verbose = 0

#----------------------------------------------------------------------
# Whether to print out messages with the TRACE_CHARM() and TRACEPUP()
#  series of statements
#----------------------------------------------------------------------

trace_charm = 0

#----------------------------------------------------------------------
# Whether to enable displaying messages with the DEBUG() series of
# statements Also writes messages to out.debug.<P> where P is the
# (physical) process rank Still requires the "DEBUG" group to be
# enabled in Monitor (that is Monitor::is_active("DEBUG") mustb e true
# for any output)
#----------------------------------------------------------------------

debug = 0

#----------------------------------------------------------------------
# Whether to periodically print all field values.  See
# src/Field/field_FieldBlock.cpp
#----------------------------------------------------------------------

debug_verbose = 0

#----------------------------------------------------------------------
# Whether to track dynamic memory statistics.  Can be useful, but can
# cause problems on some systems that also override new [] () / delete
# [] ()
#----------------------------------------------------------------------

memory = 1

#----------------------------------------------------------------------
# Enable charm++ dynamic load balancing
#----------------------------------------------------------------------

balance = 1

# balancer =  # Uses a greedy algorithm that always assigns
# the heaviest object to the least loaded processor.

# balancer =  # Extends the greedy algorithm to take the
# communication graph into account.

# balancer = 'TopoCentLB' # Extends the greedy algorithm to take
# processor topology into account.

# balancer = 'RefineLB' # Moves objects away from the most overloaded
# processors to reach average, limits the number of objects migrated.

# balancer = 'RefineSwapLB' # Moves objects away from the most
# overloaded processors to reach average. In case it cannot migrate an
# object from an overloaded processor to an underloaded processor, it
# swaps objects to reduce the load on the overloaded processor. This
# strategy limits the number of objects migrated.

balancer = [
'GreedyCommLB',
'GreedyLB', 
'HybridLB',
'NeighborLB',
'RandCentLB',
'RefineCommLB',
'RefineLB',
'RotateLB']

#----------------------------------------------------------------------
# Whether to compile with -pg to use gprof for performance profiling
#----------------------------------------------------------------------

use_gprof = 0

#----------------------------------------------------------------------
# Whether to compile with the Grackle chemistry and cooling library
#----------------------------------------------------------------------

use_grackle = 0

#----------------------------------------------------------------------
# Whether to run the test programs using valgrind to check for memory leaks
#----------------------------------------------------------------------

use_valgrind = 0

#----------------------------------------------------------------------
# Whether to use Cello Performance class for collecting performance
# data (currently requires global reductions, and may not be fully
# functional) (basic time data on root processor is still output)
#----------------------------------------------------------------------

use_performance = 0

#----------------------------------------------------------------------
# Whether to compile the CHARM++ version for use with the Projections
# performance tool.
#----------------------------------------------------------------------

use_projections = 0

#----------------------------------------------------------------------
# How many processors to run parallel unit tests
#----------------------------------------------------------------------

ip_charm = '4'

#----------------------------------------------------------------------
# AUTO CONFIGURATION
#----------------------------------------------------------------------

# Whether the system has the PAPI performance API installed

use_papi = 0

env = Environment()

if not env.GetOption('clean'):

     configure = Configure(env)

     if not configure.CheckCHeader('papi.h'):
          print 'PAPI not installed'
          use_papi = 0
     else:
          print 'PAPI installed'
          use_papi = 1

     env = configure.Finish()

# use_papi = 0

#-----------------------------------------------------------------------
# COMMAND-LINE ARGUMENTS
#-----------------------------------------------------------------------

# scons command line (overrides CELLO_* environment variables)

arch = ARGUMENTS.get('arch','unknown')
prec = ARGUMENTS.get('prec','unknown')

# use environment variable if scons command line not provided

if (arch == 'unknown' and "CELLO_ARCH" in os.environ):
     arch = os.environ["CELLO_ARCH"]
if (prec == 'unknown' and "CELLO_PREC" in os.environ):
     prec = os.environ["CELLO_PREC"]

print 
print "    CELLO_ARCH scons arch=",arch
print "    CELLO_PREC scons prec=",prec
print 

#----------------------------------------------------------------------
# CONFIGURATION DEFINES
#----------------------------------------------------------------------

define = {}

# Temporary defines

# Global defines

define_cello =        ['CONFIG_USE_CELLO']

# Precision defines

define["single"] =    ['CONFIG_PRECISION_SINGLE']
define["double"] =    ['CONFIG_PRECISION_DOUBLE']
define_int_size  =    ['SMALL_INTS']

# Grackle defines

define_grackle   = ['CONFIG_USE_GRACKLE']
#  default GRACKLE path set to avoid "Export of non-existent variable
#  ''grackle_path'" error even when use_grackle is 0
grackle_path     = 'grackle_path_not_set'

# Performance defines

define_memory =       ['CONFIG_USE_MEMORY']
define_projections =  ['CONFIG_USE_PROJECTIONS']
define_performance =  ['CONFIG_USE_PERFORMANCE']
define_papi  =        ['CONFIG_USE_PAPI','PAPI3']

#  default PAPI path set to avoid "Export of non-existent variable
#  ''papi_path'" error even when use_papi is 0

papi_path           = 'papi_path_not_set'  

# Debugging defines

define_trace =        ['CELLO_TRACE']
define_verbose =      ['CELLO_VERBOSE']
define_trace_charm =  ['CELLO_TRACE_CHARM']
define_debug =        ['CELLO_DEBUG']

define_debug_verbose = ['CELLO_DEBUG_VERBOSE']

# Library defines

define_hdf5  =        ['H5_USE_16_API']

define_png   =        ['NO_FREETYPE']

# Charm defines

define_charm =        ['CONFIG_USE_CHARM']  # used for Grackle 

# 
#----------------------------------------------------------------------
# ASSEMBLE DEFINES
#----------------------------------------------------------------------

defines     = []

# Precision configuration

if (prec == 'single' or prec == 'double'):
     defines = defines + define[prec]
else:
     print "Unrecognized precision ",prec
     print
     print "Valid precisions are 'single' and 'double'"
     print
     print "The precision is set using the environment variable $CELLO_PREC"
     print "or by using 'scons prec=<precision>"
     sys.exit(1)

defines = defines + define_int_size

defines = defines + define_hdf5
defines = defines + define_png

charm_perf = ''

if (use_projections == 1):
     defines = defines + define_projections
     charm_perf = '-tracemode projections'

if (use_performance == 1):
     defines = defines + define_performance

flags_arch_cpp = ''
flags_config = ''
flags_cxx = ''
flags_cc = ''
flags_fc = ''
flags_link = ''
flags_cxx_charm = ''
flags_cc_charm = ''
flags_fc_charm = ''
flags_link_charm = ''

if (use_gprof == 1):
     flags_config = flags_config + ' -pg'

if (use_papi != 0):      defines = defines + define_papi
if (use_grackle != 0):   defines = defines + define_grackle
if (trace != 0):         defines = defines + define_trace
if (verbose != 0):       defines = defines + define_verbose
if (trace_charm != 0):   defines = defines + define_trace_charm
if (debug != 0):         defines = defines + define_debug
if (debug_verbose != 0): defines = defines + define_debug_verbose
if (memory != 0):        defines = defines + define_memory

defines = defines + define_charm
defines = defines + define_cello

#======================================================================
# ARCHITECTURE SETTINGS
#======================================================================

mpi_path = '' # this is a hack to compile on gordon with parallel HDF5

is_arch_valid = 0
sys.path.append("./config");


if   (arch == "gordon_gnu"):   from gordon_gnu   import *
elif (arch == "gordon_intel"): from gordon_intel import *
elif (arch == "gordon_pgi"):   from gordon_pgi   import *
elif (arch == "linux_gnu"):    from linux_gnu    import *
elif (arch == "linux_gprof"):  from linux_gprof  import *
elif (arch == "linux_mpe"):    from linux_mpe    import *
elif (arch == "linux_tau"):    from linux_tau    import *
elif (arch == "ncsa_bw"):      from ncsa_bw      import *
elif (arch == "faraday_gnu"):  from faraday_gnu  import *
elif (arch == "faraday_gnu_debug"):  from faraday_gnu_debug  import *

#======================================================================
# END ARCHITECTURE SETTINGS
#======================================================================

if (not is_arch_valid):
   print "Unrecognized architecture ",arch
   sys.exit(1)

#======================================================================
# FINAL CHARM SETUP
#======================================================================

charmc = charm_path + '/bin/charmc -language charm++ '

cxx = charmc + charm_perf + ' '

if (balance == 1):
     flags_cxx_charm = flags_cxx_charm + " -balancer " + " -balancer ".join(balancer)
     flags_link_charm = flags_link_charm + " -module " + " -module ".join(balancer)

#======================================================================
# UNIT TEST SETTINGS
#======================================================================

serial_run   = ""
parallel_run = charm_path + "/bin/charmrun +p" + ip_charm

if (use_valgrind):
     valgrind = "valgrind --leak-check=full"
     serial_run   = valgrind + " " + serial_run
     parallel_run = parallel_run + " " + valgrind

#======================================================================
# CELLO PATHS
#======================================================================

bin_path = '#/bin'
lib_path = '#/lib'
inc_path = '#/include'
test_path= 'test'

Export('bin_path')
Export('grackle_path')
Export('use_grackle')
Export('lib_path')
Export('inc_path')
Export('test_path')
Export('ip_charm')


cpppath     = [inc_path]
fortranpath = [inc_path]
libpath     = [lib_path]

#----------------------------------------------------------------------
# PAPI PATHS
#----------------------------------------------------------------------

if (use_papi):
     cpppath = cpppath + [papi_path + '/include']
     libpath = libpath + [papi_path + '/lib']

#----------------------------------------------------------------------
# HDF5 PATHS
#----------------------------------------------------------------------

cpppath = cpppath + [hdf5_path + '/include' ]
libpath = libpath + [hdf5_path + '/lib']

if (mpi_path != ''):
   cpppath = cpppath + [mpi_path + '/include']

#----------------------------------------------------------------------
# LIBPNG PATHS
#----------------------------------------------------------------------

libpath = libpath + [png_path + '/lib']

#----------------------------------------------------------------------
# FORTRAN LINK PATH
#----------------------------------------------------------------------

libpath = libpath + [libpath_fortran]

# set the Cello binary and library paths

#======================================================================
# ENVIRONMENT
#======================================================================

environ  = os.environ

cxxflags = flags_arch + ' ' + flags_arch_cpp
cxxflags = cxxflags + ' ' + flags_cxx
cxxflags = cxxflags + ' ' + flags_config
cxxflags = cxxflags + ' ' + flags_cxx_charm

cflags   = flags_arch
cflags   = cflags + ' ' + flags_cc
cflags   = cflags + ' ' + flags_config
cflags   = cflags + ' ' + flags_cc_charm

fortranflags = flags_arch
fortranflags = fortranflags + ' ' + flags_fc
fortranflags = fortranflags + ' ' + flags_config
fortranflags = fortranflags + ' ' + flags_fc_charm

linkflags    = flags_arch + ' ' + flags_arch_cpp
linkflags    = linkflags + ' ' + flags_link
linkflags    = linkflags + ' ' + flags_config
linkflags    = linkflags + ' ' + flags_link_charm

if (prec == 'double'):
    fortranflags = fortranflags + ' ' + flags_prec_double
if (prec == 'single'):
    fortranflags = fortranflags + ' ' + flags_prec_single

if not os.path.exists("include"):
     os.makedirs("include")
cello_def = open ("include/auto_config.def", "w")


env = Environment (
     CC           = cc,
     CFLAGS       = cflags,
     CPPDEFINES   = defines,
     CPPPATH      = cpppath,
     CXX          = cxx,
     CXXFLAGS     = cxxflags,
     ENV          = environ,
     FORTRANFLAGS = fortranflags,
     FORTRAN      = f90,
     FORTRANLIBS  = libs_fortran,
     FORTRANPATH  = fortranpath,
     LIBPATH      = libpath,
     LINKFLAGS    = linkflags )

cello_def.write ("#define CELLO_ARCH "
		"\""+arch+"\"\n")
cello_def.write ("#define CELLO_PREC "
		"\""+prec+"\"\n")
cello_def.write ("#define CELLO_CC "
		"\""+cc+"\"\n")	
cello_def.write ("#define CELLO_CFLAGS "
		"\""+cflags+"\"\n")
cello_def.write ("#define CELLO_CPPDEFINES "
		"\""+" ".join(map(str,defines))+"\"\n")
cello_def.write ("#define CELLO_CPPPATH "
		"\""+" ".join(map(str,cpppath))+"\"\n")
cello_def.write ("#define CELLO_CXX "
		"\""+cxx+"\"\n")	
cello_def.write ("#define CELLO_CXXFLAGS "
		"\""+cxxflags+"\"\n")
cello_def.write ("#define CELLO_FORTRANFLAGS "
		"\""+fortranflags+"\"\n")
cello_def.write ("#define CELLO_FORTRAN "
		"\""+f90+"\"\n")
cello_def.write ("#define CELLO_FORTRANLIBS "
		"\""+" ".join(map(str,libs_fortran))+"\"\n")
cello_def.write ("#define CELLO_FORTRANPATH "
		"\""+" ".join(map(str,fortranpath))+"\"\n")
cello_def.write ("#define CELLO_LIBPATH "
		"\""+" ".join(map(str,libpath))+"\"\n")
cello_def.write ("#define CELLO_LINKFLAGS "
		"\""+linkflags+"\"\n" )
cello_def.write ("#define CELLO_HOST "
		"\""+socket.gethostname()+"\"\n" )
cello_def.write ("#define CELLO_DIR "
		"\""+Dir('.').abspath+"\"\n" )
cello_def.write ("#define CELLO_DATE "
		"\""+time.strftime("%Y-%m-%d",time.gmtime())+"\"\n" )
cello_def.write ("#define CELLO_TIME "
		"\""+time.strftime("%H:%M:%S",time.gmtime())+"\"\n" )
cello_def.write ("#define CELLO_CHANGESET "
	"\""+subprocess.check_output(["hg", "id", "-n"]).rstrip()+"\"\n" )

env.Command ('hgid.out', [], 'hg --id > $TARGETS')
env.AlwaysBuild('hgid.out')
cello_def.close()
#======================================================================
# BUILDERS
#======================================================================

charm_builder = Builder (action="${CXX} $SOURCE; mv ${ARG}.*.h `dirname $SOURCE`")
env.Append(BUILDERS = { 'CharmBuilder' : charm_builder })

Export('env')
Export('parallel_run')
Export('serial_run')
Export('use_papi')

SConscript( 'src/SConscript',variant_dir='build')
SConscript('test/SConscript')

#======================================================================
# CLEANING
#======================================================================

Clean('.','bin')
Clean('.','lib')

if (use_projections == 1):
   Clean('.',Glob('bin/*.projrc'))
   Clean('.',Glob('bin/*.log'))
   Clean('.',Glob('bin/*.sts'))
   Clean('.','charmrun')

#======================================================================
# PACKAGING
#======================================================================

# env = Environment(tools=['default', 'packaging'])
# title = 'Enzo-P / Cello Extreme AMR Astrophysics and Cosmology'
# env.Package( NAME           = 'cello',
#              VERSION        = '0.5.0',
#              PACKAGEVERSION = 0,
#              PACKAGETYPE    = 'targz',
#              LICENSE        = 'New BSD',
#              SUMMARY        = title,
#              DESCRIPTION    = title
#         )
