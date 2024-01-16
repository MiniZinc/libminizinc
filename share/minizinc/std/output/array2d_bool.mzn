function int: mzn_num_digits(int: x) =
	if x == 0 then 
		1
	else
		let {
			int: xx = abs(x)
		} in floor(log10(xx)) + 1 + (x < 0)
	endif;


/** @group stdlib.output Create string to visualise a 2D boolean array \a x as a checkered board. */
function string: show_array2d_bool(array[int,int] of var bool: x) ::output_only;
function string: show_array2d_bool(array[int,int] of bool: x) =
	if length(x) != 0 then
		let {
			any: xs = index_set_1of2(x);
			any: ys = index_set_2of2(x);
			int: one_digits = max(mzn_num_digits(max(xs)), mzn_num_digits(min(xs)));
			int: two_digits = max(mzn_num_digits(max(ys)), mzn_num_digits(min(ys)));
		} in concat(i in 1..two_digits) (
			concat(q in 1..one_digits)(" ") ++ " "
			++ concat(j in ys)(
				let {
					int: digit = (abs(j) div pow(10, two_digits - i)) mod 10;
				} in 
				if digit != 0 \/ i = two_digits \/ j > pow(10, two_digits - i) then
					show(digit)
				elseif j < 0 /\ abs(j) >= pow(10, two_digits - i-1) then
					"-"
				else 
					" "
				endif
			)
			++ "\n"
		) ++
		concat(i in xs)(
			concat(q in 1..(one_digits - (mzn_num_digits(i))))(" ")
			++ "\(i) "
			++ concat(j in ys)(if x[i,j] then "█" else " " endif)
			++ "\n"
		)
	endif;

