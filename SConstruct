class CONFIG:
    DEBUG = 0
    RELEASE = 1

type = CONFIG.RELEASE

if type == CONFIG.DEBUG:
    env = Environment(CPPFLAGS="-g -Wall -O0")
else:
    env = Environment(CPPFLAGS="-O3")

env['CPPFLAGS'] += ' -fmodules-ts -std=c++23 -fcoroutines'
env['CPPPATH'] = '#/gtest/include'
env['LIBPATH'] = '#/gtest/lib'
env['LIBS'] = 'gtest'
env['CPPDEFINES'] = {'GTEST_HAS_PTHREAD': 1}

env.Tool('compilation_db')
env.CompilationDatabase()

binDir = '#/bin'
Export('env binDir')

env.VariantDir('build', 'src')
env.SConscript('build/SConscript')
