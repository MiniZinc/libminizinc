/***
!Test
expected:
- !Result
  solution: !Solution {}
***/

constraint
     let {
      var float: x = true
     }
     in
     x >= 0.0
;
solve satisfy;
