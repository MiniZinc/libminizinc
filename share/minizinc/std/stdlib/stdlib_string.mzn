/***
  @groupdef stdlib.builtins.string String operations

  These functions implement operations on strings.
*/

/** @group stdlib.builtins.string Convert \a x into a string */
function string: show(any $T: x);
/** @group stdlib.builtins.string Convert \a x into a string */
function string: show(array[$U] of any $T: x);

function string: show_indexed(array[int] of string: idx, array[int] of string: x) =
	"[" ++ join(", ", [idx[i]++": "++x[i] | i in index_set(x)]) ++ "]";

function string: showDzn(any $T: x) ::mzn_internal_representation;
function string: showDzn(array[$U] of any $T: x) ::mzn_internal_representation;
function string: showDznId(string: x) ::mzn_internal_representation;

function string: showCheckerOutput();

/** @group stdlib.builtins.string Formatted to-string conversion for integers

Converts the integer \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: show_int(int: w, var int: x);
/** @group stdlib.builtins.string Formatted to-string conversion for floats.

Converts the float \a x into a string right justified
by the number of characters given by \a w, or left justified if
\a w is negative. The number of digits to appear after the decimal
point is given by \a p. It is a run-time error for \a p to be negative.
*/
function string: show_float(int: w, int: p, var float: x);

/** @group stdlib.builtins.string Convert two-dimensional array \a vs into a string
    with row and column headers \a row_hdr and \a col_hdr */
function string: show2d_indexed(array[int] of string: row_hdr,
                                array[int] of string: col_hdr,
                                array[int,int] of string: vs) =
  let {
      int: row_hdr_max = max([0]++[string_length(r)+1 | r in row_hdr]);
      string: row_hdr_offset = if row_hdr_max = 0 then " " else concat([" " | _ in 1..row_hdr_max+2]) endif;
      array[int] of int: col_hdr_max =
        [ j : max([if length(col_hdr)=0 then 0 else string_length(col_hdr[j]) endif]++
              [string_length(vs[i,j]) | i in index_set_1of2(vs)]) | j in index_set_2of2(vs)];
  } in if length(vs) = 0 then "[| |]" else
  if length(col_hdr)=0 then "["
  else 
  "[|"++row_hdr_offset++concat(i in index_set(col_hdr))(format_justify_string(col_hdr_max[i],col_hdr[i])++": ")++"\n "
  endif
  ++concat([
      "| "++
      if length(row_hdr) > 0 then format_justify_string(row_hdr_max-1, row_hdr[i])++": " endif++
      join(", ", [format_justify_string(col_hdr_max[j], vs[i,j]) | j in index_set_2of2(vs)])++"\n "
  | i in index_set_1of2(vs)
  ])++"|]" endif;

/** @group stdlib.builtins.string Convert two-dimensional array \a x into a string */
function string: show2d(array[$$E,$$F] of any $T: x) = let {
    bool: idx1_is_enum = if length(x) != 0 then min(enum_of(min(index_set_1of2(x)))) != -infinity else false endif;
	bool: idx1_is_needed = if length(x) != 0 then idx1_is_enum \/ min(index_set_1of2(x)) != 1 else false endif;
    bool: idx2_is_enum = if length(x) != 0 then min(enum_of(min(index_set_2of2(x)))) != -infinity else false endif;
	bool: idx2_is_needed = if length(x) != 0 then idx2_is_enum \/ min(index_set_2of2(x)) != 1 else false endif;
    array[int] of string: col_hdr = if idx2_is_needed then [i : show(i) | i in index_set_2of2(x)] else [] endif;
    array[int] of string: row_hdr = if idx1_is_needed then [i : show(i) | i in index_set_1of2(x)] else [] endif;
} in show2d_indexed(row_hdr, col_hdr, [ (i,j) : show(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x) ]);

/** @group stdlib.builtins.string Convert three-dimensional array \a x into a string */
function string: show3d(array[int,int,int] of any $T: x) =
	let {
		int: len1=card(index_set_1of3(x));
		int: len2=card(index_set_2of3(x));
		int: len3=card(index_set_3of3(x));
		array[int] of string: s = [show(x[i,j,k]) | i in index_set_1of3(x), j in index_set_2of3(x), k in index_set_3of3(x)];
		int: max_length = if length(s) > 0 then max([string_length(s[i]) | i in index_set(s)]) else 0 endif;
	} in
		"[| | "++
		concat([
			format_justify_string(max_length,s[(i - 1) * len2 * len3 + (j - 1) * len3 + k]) ++
			if k < len3 then
				", "
			elseif j < len2 then
				" |\n     "
			elseif i < len1 then
				" |,\n\n   | "
			else
				" | |]\n"
			endif | i in 1..len1, j in 1..len2, k in 1..len3
		]) ++ if len1 = 0 then "| |]" else "" endif;

/** @group stdlib.builtins.string Convert \a x into JSON string */
function string: showJSON(any $T: x);
/** @group stdlib.builtins.string Convert \a x into JSON string */
function string: showJSON(array[$U] of any $T: x);

/** @group stdlib.builtins.string Create a JSON object from an array of key-value pairs \a obj.

    \a obj[\p i, \p 1] is the key name for the \p i-th entry  
    \a obj[\p i, \p 2] is the JSON value for the \o i-th entry (usually generated using showJSON)  
 */
function string: json_object(array [int, 1..2] of string: obj) =
    concat([
        "{",
        join(",", [
            join(":", [showJSON(obj[i, 1]), obj[i, 2]])
            | i in index_set_1of2(obj)
        ]),
        "}"
    ]);

/** @group stdlib.builtins.string Create a JSON array from an array of JSON strings \a arr.
    
    The elements are assumed to be strings already in JSON format (usually generated using showJSON)
 */
function string: json_array(array [int] of string: arr) = concat(["[", join(",", arr), "]"]);

/** @group stdlib.builtins.string Return length of \a s 

    This function interprets the string as UTF-8 encoded.
    The length is the number of Unicode code points in the string.
*/
function int: string_length(string: s);

/** @group stdlib.builtins.string Split \a s by separator \a sep

    Returns an array of strings containing the substrings of \a s
    that are separated by \a sep. If \a sep is empty, the result is
    an array of characters of \a s.

    This function interprets the string as UTF-8 encoded, so when
    splitting with an empty separator, the result is an array of
    Unicode code points.
  */
function array[int] of string: string_split(string: s, string: sep);

/** @group stdlib.builtins.string Return concatenation of \a s1 and \a s2 */
function string: '++'(string: s1, string: s2) ::mzn_internal_representation;
/** @group stdlib.builtins.string Return concatenation of strings in array \a s */
function string: concat(array[$T] of string: s);
/** @group stdlib.builtins.string Join string in array \a s using delimiter \a d */
function string: join(string: d, array[$T] of string: s);

/** @group stdlib.builtins.string Convert \a x into a string */
function string: format(any $T: x) = show(x);
/** @group stdlib.builtins.string Convert \a x into a string */
function string: format(array[$U] of any $T: x) = show(x);

/** @group stdlib.builtins.string Return array for output of all variables in JSON format */
function array[int] of string: outputJSON();
/** @group stdlib.builtins.string Return array for output of all variables in JSON format,
    including objective if \a b is true
*/
function array[int] of string: outputJSON(bool: b);
/** @group stdlib.builtins.string Return array for output of all parameters in JSON format */
function array[int] of string: outputJSONParameters();

/** @group stdlib.builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p, or the maximum number of digits after the decimal point for floating
point numbers. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, any $T: x);
/** @group stdlib.builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, array[$U] of any $T: x);

/** @group stdlib.builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, any $T: x);
/** @group stdlib.builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, array[$U] of any $T: x);
/** @group stdlib.builtins.string String justification

Returns the string \a x right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format_justify_string(int: w, string: x);
/** @group stdlib.builtins.string String justification

Returns the string \a x right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of of \a x to be formatted is given by
\a p. It is a run-time error for \a p to be negative.
*/
function string: format_justify_string(int: w, int: p, string: x);

/** @group stdlib.builtins.string Return path of file where this function is called */
function string: file_path();

/** @group stdlib.builtins.string Add \a o to the output section \a s */
test output_to_section(string: s, string: o);

/** @group stdlib.builtins.string Output \a o to the JSON section \a s */
test output_to_json_section(string: s, any $T: o);
/** @group stdlib.builtins.string Output \a o to the JSON section \a s */
test output_to_json_section(string: s, array [$U] of any $T: o);
