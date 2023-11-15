include "ide/vis.mzn";

int: n = 4;
var 1..3: x;
var 1..3: y;
constraint x + y > n;

function ann: vis_foo(int: n, var int: x, var int: y) = let {
    constraint output_to_json_section(
        mzn_vis_server("foo.html", (n: n)),
        (x: x, y: y)
    );
} in output_only;

output vis_foo(n, x, y);
