#### MiniZinc Executable Target

add_executable(minizinc minizinc.cpp)
target_link_libraries(minizinc mzn)

install(
  TARGETS minizinc
  EXPORT libminizincTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
