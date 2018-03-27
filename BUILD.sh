# This shell script is to run from the build subdirectory
# Usage: optional arg 1 for build type
if [[ -z "$1" ]]; then
  BLD_TYPE=Release
else
  BLD_TYPE=$1
fi
echo Setting build type:  $BLD_TYPE
cmake -DCMAKE_BUILD_TYPE=$BLD_TYPE -DCMAKE_VERBOSE_MAKEFILE=ON \
           -DCPLEX_STUDIO_DIR=$CPLEX_STUDIO_DIR \
           -DGUROBI_HOME=$GUROBI_HOME -DBUILD_GUROBI_PLUGIN=ON \
           -DOSICBC_HOME=$OSICBC_HOME -DOSICBC_LINKEXTRAS="" '# bz2 lapack blas  necessary on ArchLinux' \
           -DSCIP_DIR=$SCIP_DIR -DSOPLEX_DIR=$SOPLEX_DIR -DZIMPL_DIR=$ZIMPL_DIR \
           -DGECODE_HOME=$GECODE_HOME \
           ..
#cmake -DCMAKE_BUILD_TYPE=debug  -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 ..
cmake --build . -- -j3
#sudo cmake --build . --target install

