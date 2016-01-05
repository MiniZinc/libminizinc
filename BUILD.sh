# This shell script is to run from the build subdirectory
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON \
           -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1262 \
           -DGUROBI_HOME=/opt/gurobi605/linux64 \
           -DSCIP_DIR=$SCIP_DIR -DSOPLEX_DIR=$SOPLEX_DIR -DZIMPL_DIR=$ZIMPL_DIR \
           ..
#cmake -DCMAKE_BUILD_TYPE=debug  -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 ..
cmake --build . -- -j3
#sudo cmake --build . --target install

