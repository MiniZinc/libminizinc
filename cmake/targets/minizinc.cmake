#### Main MiniZinc Binary Target

add_executable(minizinc minizinc.cpp)
target_link_libraries(minizinc minizinc_solver)

# Deprecated binary targets
add_executable(mzn2fzn minizinc.cpp)
target_link_libraries(mzn2fzn minizinc_solver)
add_executable(solns2out minizinc.cpp)
target_link_libraries(solns2out minizinc_solver)

install(
  TARGETS minizinc mzn2fzn solns2out
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
