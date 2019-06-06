#### MiniZinc Executable Target

add_executable(minizinc_exe minizinc.cpp)
target_link_libraries(minizinc_exe minizinc)

if(NOT WIN32) # TODO: Output name being the same as a library name gives a linker error with MVSC
  set_target_properties(minizinc_exe PROPERTIES OUTPUT_NAME "minizinc")
endif()

install(
  TARGETS minizinc_exe
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
