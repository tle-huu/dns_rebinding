env = Environment(
                    CXXFLAGS = '-std=c++17'
                )

VariantDir('build' , 'src', duplicate=0)

env.Program('dns_server', Glob('build/*.cpp'))
