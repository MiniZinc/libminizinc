REM This Windows batch is to run from the build subdirectory
REM Release is not used, need to switch on manually in MSVC
cmake .. -G"Visual Studio 12 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCPLEX_STUDIO_DIR="C:\Program Files\IBM\ILOG\CPLEX_Studio1262"  
cmake --build .
