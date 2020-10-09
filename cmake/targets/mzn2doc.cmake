#### Binary target for MiniZinc documentation generator
add_executable(mzn2doc mzn2doc.cpp)
target_link_libraries(mzn2doc mzn)

install(
  TARGETS mzn2doc
  EXPORT libminizincTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
