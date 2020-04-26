fips_begin_module(PDCurses)
    fips_vs_warning_level(3)
    fips_vs_disable_warnings(4996 4267 4244)

    fips_include_directories(.)

    add_definitions(-DPDC_WIDE)
    
    fips_files_ex(. *.h GROUP "Header Files")
    fips_files_ex(common *.h GROUP "common")
    fips_files_ex(pdcurses *.c GROUP "pdcurses")
    fips_files_ex(wincon *.c *.h GROUP "wincon")
fips_end_module()

fips_begin_unittest(PDCurses)
    fips_vs_warning_level(3)
    fips_vs_disable_warnings(4819 4996)  # 为了中文注释 cp936
    
    fips_dir(UnitTests)
    if(FIPS_WINDOWS)
    endif()
    fips_files(
        PDCursesTest.cc
    )
    fips_deps(PDCurses)
fips_end_unittest()

