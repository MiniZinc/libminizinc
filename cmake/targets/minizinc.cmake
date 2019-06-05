#### MiniZinc Executable Target

add_executable(minizinc_exe minizinc.cpp)
target_link_libraries(minizinc_exe minizinc)
set_target_properties(minizinc_exe PROPERTIES OUTPUT_NAME "minizinc")

install(
  TARGETS minizinc_exe
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
