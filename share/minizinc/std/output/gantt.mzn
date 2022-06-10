/** @group stdlib.output Create string Gantt chart from task \a start times, duration \a dur, and their names \a name. */
function string: show_gantt(array[$$T] of var int: start, array[$$T] of var int: dur, array[$$T] of string: name) ::output_only;

function string: show_gantt(array[$$T] of int: start, array[$$T] of int: dur, array[$$T] of string: name) =
	let {
		string: dot = "•";
		int: n_dots = 80;
		string: block = "█";
		
		int: final = max([start[i] + dur[i] | i in index_set(start)]);
		set of int: span = min(start)..final;
		array[0..n_dots] of int: thresholds = array1d(0..n_dots, [ceil(min(start) + i * length(span) / n_dots) | i in 0..n_dots-1] ++ [final]);
		
		int: name_len = max([string_length(name[i]) | i in index_set(start) ]);
		array[int] of index_set(start): sort = arg_sort(start);
		
		string: dot_line = concat(i in 1..name_len + 1)(" ") ++ concat(i in 1..n_dots) (dot);
		string: legend = concat(i in 1..name_len + 1)(" ") ++ concat(iv in [thresholds[i*10] | i in 0..n_dots div 10 -1]) (show_int(-10, iv)) ++ "\(final)";
	} in
		legend ++ "\n" ++
		dot_line ++ "\n" ++
		concat(i in index_set(sort)) (
			format_justify_string(name_len, name[sort[i]]) ++ " "
			++ concat(d in 1..n_dots where thresholds[d] <= start[sort[i]] + dur[sort[i]]) (
				if card(thresholds[d-1]..thresholds[d] intersect start[sort[i]]..start[sort[i]] + dur[sort[i]]) > 0 then
					block
				else
					" "
				endif
			)
			++ " \(start[sort[i]]) - \(start[sort[i]] + dur[sort[i]])\n" 
		) ++
		dot_line ++ "\n" ++
		legend;

/** @group stdlib.output Create string Gantt chart from task \a start times and duration \a dur, using the index set as name for the tasks. */
function string: show_gantt(array[$$T] of var int: start, array[$$T] of var int: dur) ::output_only;
function string: show_gantt(array[$$T] of int: start, array[$$T] of int: dur) =
	show_gantt(start, dur, array1d(index_set(start), [show(i) | i in index_set(start)]));

