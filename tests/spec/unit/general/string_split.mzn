/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

constraint assert(string_split("abc", "") = ["a", "b", "c"], "Incorrect");
constraint assert(string_split("abc", "d") = ["abc"], "Incorrect");
constraint assert(string_split("abc", "b") = ["a", "c"], "Incorrect");
constraint assert(string_split("abc", "a") = ["", "bc"], "Incorrect");
constraint assert(string_split("abc", "c") = ["ab", ""], "Incorrect");
constraint assert(string_split("aaa", "a") = ["", "", "", ""], "Incorrect");
