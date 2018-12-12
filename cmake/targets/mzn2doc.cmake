#### Binary target for MiniZinc documentation generator
add_executable(mzn2doc mzn2doc.cpp)
target_link_libraries(mzn2doc minizinc_solver)

install(
  TARGETS mzn2doc
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
