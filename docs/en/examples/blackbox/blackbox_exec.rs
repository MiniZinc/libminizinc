//! Template: MiniZinc black-box propagator, executable (subprocess) mode.
//!
//! Build:  rustc -O blackbox_exec.rs -o blackbox   (or build a Cargo binary)
//!
//! Use from MiniZinc:
//!   predicate my_prop(array[int] of var int: xs)
//!       ::minizinc_value_propagator ::blackbox_exec("./blackbox");
//!
//! The solver starts this program once and communicates over stdin/stdout,
//! one line per propagation:
//!
//!   request : <int values, comma-separated>;<float values, comma-separated>\n
//!   response: <int outputs, comma-separated>;<float outputs, comma-separated>\n
//!
//! A segment may be empty. For a BOUNDS propagator each segment holds two
//! entries per variable, a lower then an upper bound: "lb0,ub0,lb1,ub1;\n".

use std::io::{self, BufRead, Write};

fn parse<T: std::str::FromStr>(segment: &str) -> Vec<T> {
    segment
        .split(',')
        .filter(|s| !s.trim().is_empty())
        .map(|s| s.trim().parse().unwrap_or_else(|_| panic!("bad value: {s}")))
        .collect()
}

fn main() {
    // The extra arguments from the ::blackbox_exec annotation are passed on the
    // command line, so they are available as `std::env::args().skip(1)`.
    let _args: Vec<String> = std::env::args().skip(1).collect();

    let stdin = io::stdin();
    let mut stdout = io::stdout();
    for line in stdin.lock().lines() {
        let line = line.expect("read error");
        let (int_seg, float_seg) = line.split_once(';').unwrap_or((line.as_str(), ""));
        let ints: Vec<i64> = parse(int_seg);
        let _floats: Vec<f64> = parse(float_seg);

        // Example (relational value propagator): accept (1) iff the integer
        // inputs sum to an even number, reject (0) otherwise.
        let accept = (ints.iter().sum::<i64>() % 2 == 0) as i64;
        writeln!(stdout, "{accept};").expect("write error");
        stdout.flush().expect("flush error"); // MUST flush after each response
    }
}
