constraint let {
  var set of int: x;
  constraint {1};
} in true;

int: x;

solve satisfy;
solve minimize x;
