REM This Windows batch is to run from the build subdirectory
cmake .. -G"Visual Studio 12 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCPLEX_STUDIO_DIR="C:\Program Files\IBM\ILOG\CPLEX_Studio1262"  
REM Release cannot be specified from config for MSVC, so doing it for build (can also switch on in IDE)
cmake --build . --config Release
