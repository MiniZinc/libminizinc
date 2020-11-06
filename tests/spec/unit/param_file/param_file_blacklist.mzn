/***
---
!Test
solvers: [gecode]
extra_files:
- param_file_blacklist_1.mpc
expected: !Error
  regex: .*not allowed in configuration file.*
---
!Test
solvers: [gecode]
extra_files:
- param_file_blacklist_2.mpc
expected: !Error
  regex: .*not allowed in configuration file.*
***/
