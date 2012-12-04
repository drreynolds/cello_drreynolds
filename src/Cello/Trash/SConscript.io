#------------------------------
# DIRECTORIES
#------------------------------

#------------------------------
# IMPORTS
#------------------------------

Import('env')
Import('type')
Import('use_papi')

Import('bin_path')
Import('inc_path')
Import('lib_path')

#------------------------------
# DEPENDENCIES
#------------------------------

components = [ 'io',
              'simulation', 'method',
               'mesh', 'field',
              'disk','memory','parallel',
              'parameters', 
              'test','cello','external',
              'error','monitor','performance' ]
if (use_papi):
    components = components + ['papi','pfm']

libraries  = ['png','hdf5']

#------------------------------
# SOURCE
#------------------------------

includes = [Glob('io*hpp')]
sources  = [Glob('io*cpp')]

#------------------------------
# ENVIRONMENTS
#------------------------------

env = env.Clone(LIBS=[components,libraries])

#------------------------------
# TARGETS
#------------------------------

libraries = env.Library ('io', [sources])

if (type == "charm"):
	sources = sources + ['../Main/main_simulation.cpp']

binaries = [env.Program (['test_ItReduce.cpp',sources])]

#------------------------------
# INSTALL
#------------------------------

env.Alias('install-bin',env.Install (bin_path,binaries))
env.Alias('install-inc',env.Install (inc_path,includes))
env.Alias('install-lib',env.Install (lib_path,libraries))
