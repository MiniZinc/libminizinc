/***
!Test
solvers: [gecode]
expected:
- !Error
  type: SyntaxError
type: compile
***/

/** @group stdlib.output Create string to visualise a 2D boolean array \a !9Mã™a checkered board. */
function string: show_array2d_bool(array[int,int] of var bool: x) ::output_only;
function string: show_array2d_bool(array[int,int] of bool: x) =
	if length(x) != 0 then
		let {
			int: one_digits!9Mã™a checkered board. */
function string: show_array2d_bool(array[int,int] of var bool: x) ::output_only;
function string: show_array2d_bool(array[intt(q in 1..one_digits)(" ") ++ " "
			++ concat(j in index_set_2of2(x))(
				let {
					int: digit = (j div pow(10, two_digits - i)) mod 10;
				} in if digit != 0 \/ j > pow(10, two_digits - i) then show (digit) else " " endif
			)
			++ "\n"
		) ++
		concat(i in index_set_1of2(x))(
			concat(q in 1..(one_digits - (floor(log10(i)) + 1)))(" ")
			++ "\(i) "
			++ concat(j in index_set_2of2(x))(if x[i,j] then "‚ñà" else " " endif)
			++ "\n"
		)
	endif;

