/***
!Test
extra_files:
- coerce_enum_str.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  d: "Mon"
  darr: ["Fri", "Sat", "Sun"]
  dset: !!set {"Fri", "Sat", "Sun"}
***/

enum Day = {Mon, Tue, Wed, Thu, Fri, Sat, Sun};
Day: d ::add_to_output;
array[int] of Day: darr ::add_to_output;
set of Day: dset ::add_to_output;
