/***
!Test
extra_files:
- coerce_enum_str_err.json
solvers: [gecode]
expected: !Error
  regex: .*has invalid type-inst.*
***/

enum Day = {Mon, Tue, Wed, Thu, Fri, Sat, Sun};
enum Month = {Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Oct, Nov, Dec};
Day: d ::add_to_output;
