#%% cd build/
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1262 -DGUROBI_HOME=/opt/gurobi604/linux64 ..
#cmake -DCMAKE_BUILD_TYPE=debug  -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 ..
    ### /opt/ibm/ILOG/CPLEX_Enterprise_Server1261/CPLEX_Studio ..
cmake --build . -- -j3
#sudo cmake --build . --target install

