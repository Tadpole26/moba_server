# CMake generated Testfile for 
# Source directory: D:/work_mine/3rd_libs/libevent-2.1.12-stable
# Build directory: D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test-changelist__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-changelist")
set_tests_properties(test-changelist__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-eof__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-eof")
set_tests_properties(test-eof__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-closed__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-closed")
set_tests_properties(test-closed__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-fdleak__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-fdleak")
set_tests_properties(test-fdleak__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-init__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-init")
set_tests_properties(test-init__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-time__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-time")
set_tests_properties(test-time__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-weof__WIN32 "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-weof")
set_tests_properties(test-weof__WIN32 PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1254;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-dumpevents__WIN32_no_check "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-dumpevents")
set_tests_properties(test-dumpevents__WIN32_no_check PROPERTIES  ENVIRONMENT "" _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1278;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1320;add_backend_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-ratelim__group_lim "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-ratelim" "-g" "30000" "-n" "30" "-t" "100" "--check-grouplimit" "1000" "--check-stddev" "100")
set_tests_properties(test-ratelim__group_lim PROPERTIES  _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1331;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-ratelim__con_lim "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-ratelim" "-c" "1000" "-n" "30" "-t" "100" "--check-connlimit" "50" "--check-stddev" "50")
set_tests_properties(test-ratelim__con_lim PROPERTIES  _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1340;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-ratelim__group_con_lim "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-ratelim" "-c" "1000" "-g" "30000" "-n" "30" "-t" "100" "--check-grouplimit" "1000" "--check-connlimit" "50" "--check-stddev" "50")
set_tests_properties(test-ratelim__group_con_lim PROPERTIES  _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1349;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
add_test(test-ratelim__group_con_lim_drain "D:/work_mine/3rd_libs/libevent-2.1.12-stable/mybuild/bin/test-ratelim" "-c" "1000" "-g" "35000" "-n" "30" "-t" "100" "-G" "500" "--check-grouplimit" "1000" "--check-connlimit" "50" "--check-stddev" "50")
set_tests_properties(test-ratelim__group_con_lim_drain PROPERTIES  _BACKTRACE_TRIPLES "D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;1360;add_test;D:/work_mine/3rd_libs/libevent-2.1.12-stable/CMakeLists.txt;0;")
