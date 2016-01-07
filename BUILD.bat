REM This Windows batch is to run from the build subdirectory
cmake .. -G"Visual Studio 12 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON ^
   -DCPLEX_STUDIO_DIR="C:\Program Files\IBM\ILOG\CPLEX_Studio1262" ^
   -DOSICBC_HOME="C:\Users\glebb\prj\coin-Cbc" ^
   -DGUROBI_HOME=%GUROBI_HOME% ^
   -DSCIP_DIR=C:\Users\glebb\Downloads\scip-3.2.0\scipoptsuite-3.2.0\scip-3.2.0 -DSOPLEX_DIR=C:\Users\glebb\Downloads\scip-3.2.0\scipoptsuite-3.2.0\soplex-2.2.0 -DZIMPL_DIR=C:\Users\glebb\Downloads\scip-3.2.0\scipoptsuite-3.2.0\zimpl-3.3.3
REM Release cannot be specified from config for MSVC, so doing it for build (can also switch on in IDE)
cmake --build . --config Release
