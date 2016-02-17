% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
constraint
     let {
      var float: x = true
     }
     in
     x >= 0.0
;
solve satisfy;
