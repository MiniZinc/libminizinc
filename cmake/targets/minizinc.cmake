#### MiniZinc Executable Target

add_executable(minizinc minizinc.cpp)
target_link_libraries(minizinc mzn)

install(
  TARGETS minizinc
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
