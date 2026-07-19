project('Teal', 'cpp', default_options: ['cpp_std=c++20'])

common_src_files = [
    'src/common/error.cpp',
    'src/common/istring.cpp'
]

frontend_src_files = [
    'src/frontend/lexer.cpp',
    'src/frontend/source_location.cpp',
    'src/frontend/token.cpp'
]

test_files = [
    'tests/lexer_test.cpp'
]

include_dirs = include_directories('include')

common_lib = static_library('tealc_common', sources: common_src_files, include_directories: include_dirs)
common_dep = declare_dependency(link_with: common_lib, include_directories: include_dirs)

frontend_lib = static_library('tealc_frontend', sources: frontend_src_files, include_directories: include_dirs, dependencies: common_dep)
frontend_dep = declare_dependency(link_with: frontend_lib, include_directories: include_dirs, dependencies: common_dep)

foreach test : test_files
    base_name = test.replace('.cpp',  '').replace('tests/', '')
    test(
        base_name,
        executable(
            base_name,
            test,
            dependencies: [common_dep, frontend_dep],
            cpp_args: ['-DDEBUG=1']
        ),
        workdir: meson.project_source_root()
    )
endforeach