# This shell script is to run from the build subdirectory
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON \
           -DCCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1263 \
           -DCGUROBI_HOME=$GUROBI_HOME \
           -DCOSICBC_HOME=$OSICBC_HOME -DOSICBC_LINKEXTRAS="" '# bz2 lapack blas  necessary on ArchLinux' \
           -DCSCIP_DIR=$SCIP_DIR -DSOPLEX_DIR=$SOPLEX_DIR -DZIMPL_DIR=$ZIMPL_DIR \
           -DCGECODE_HOME=/home/bg/Documents/prj/gecode-4.4.0 \
           ..
#cmake -DCMAKE_BUILD_TYPE=debug  -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 ..
cmake --build . -- -j3
#sudo cmake --build . --target install

