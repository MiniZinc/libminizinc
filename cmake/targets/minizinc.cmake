#### MiniZinc Executable Target

add_executable(minizinc minizinc.cpp)
target_link_libraries(minizinc mzn)
if(MINGW)
  # minizinc.cpp defines wmain on Windows, which MinGW only uses with -municode.
  target_link_libraries(minizinc -municode)
endif()

install(
  TARGETS minizinc
  EXPORT libminizincTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
