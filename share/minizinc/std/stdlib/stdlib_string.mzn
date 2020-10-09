/***
  @groupdef stdlib.string String operations

  These functions implement operations on strings.
*/

/** @group stdlib.string Convert \a x into a string */
function string: show(var opt set of $T: x);
/** @group stdlib.string Convert \a x into a string */
function string: show(var opt $T: x);
/** @group stdlib.string Convert \a x into a string */
function string: show(array[$U] of var opt $T: x);

function string: showDzn(var opt set of $T: x);
function string: showDzn(var opt $T: x);
function string: showDzn(array[$U] of var opt $T: x);
function string: showDznId(string: x);

function string: showCheckerOutput();

/** @group stdlib.string Formatted to-string conversion for integers

Converts the integer \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: show_int(int: w, var int: x);
/** @group stdlib.string Formatted to-string conversion for floats.

Converts the float \a x into a string right justified
by the number of characters given by \a w, or left justified if
\a w is negative. The number of digits to appear after the decimal
point is given by \a p. It is a run-time error for \a p to be negative.
*/
function string: show_float(int: w, int: p, var float: x);

/** @group stdlib.string Convert two-dimensional array \a x into a string */
function string: show2d(array[int,int] of var opt $T: x) =
  let { int: rows=card(index_set_1of2(x));
        int: cols=card(index_set_2of2(x));
        array[int] of string: s = [show(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)];
        int: max_length = max([string_length(s[i]) | i in index_set(s)])
  } in
  "[| "++
  concat([format_justify_string(max_length,s[(i-1)*cols+j])++
   if j<cols then ", "
   elseif i<rows then " |\n   " else " |]\n"
   endif | i in 1..rows, j in 1..cols
  ])++if rows=0 then "|]" else "" endif;

/** @group stdlib.string Convert three-dimensional array \a x into a string */
function string: show3d(array[int,int,int] of var opt $T: x) =
  let { int: len1=card(index_set_1of3(x));
        int: len2=card(index_set_2of3(x));
        int: len3=card(index_set_3of3(x));
        array[int] of string: s = [show(x[i,j,k]) | i in index_set_1of3(x),
                                                    j in index_set_2of3(x),
                                                    k in index_set_3of3(x)];
        int: max_length = max([string_length(s[i]) | i in index_set(s)])
  } in
  "[| | "++
  concat([format_justify_string(max_length,s[(i-1)*len2*len3+(j-1)*len3+k])++
   if k<len3 then ", "
   elseif j<len2 then " |\n     "
   elseif i<len1 then " |,\n\n   | "
   else " | |]\n"
   endif | i in 1..len1, j in 1..len2, k in 1..len3
  ])++if len1=0 then "| |]" else "" endif;

/** @group stdlib.string Convert \a x into JSON string */
function string: showJSON(var opt $T: x);

/** @group stdlib.string Convert \a x into JSON string */
function string: showJSON(array[$U] of var opt $T: x);

/** @group stdlib.string Return length of \a s */
function int: string_length(string: s);

/** @group stdlib.string Return concatenation of \a s1 and \a s2 */
function string: '++'(string: s1, string: s2);
/** @group stdlib.string Return concatenation of strings in array \a s */
function string: concat(array[$T] of string: s);
/** @group stdlib.string Join string in array \a s using delimiter \a d */
function string: join(string: d, array[$T] of string: s);

/** @group stdlib.string Convert \a x into a string */
function string: format(var opt $T: x) = show(x);
/** @group stdlib.string Convert \a x into a string */
function string: format(var opt set of $T: x) = show(x);
/** @group stdlib.string Convert \a x into a string */
function string: format(array[$U] of var opt $T: x) = show(x);

/** @group stdlib.string Return array for output of all variables in JSON format */
function array[int] of string: outputJSON();
/** @group stdlib.string Return array for output of all variables in JSON format,
    including objective if \a b is true
*/
function array[int] of string: outputJSON(bool: b);
/** @group stdlib.string Return array for output of all parameters in JSON format */
function array[int] of string: outputJSONParameters();

/** @group stdlib.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p, or the maximum number of digits after the decimal point for floating
point numbers. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, var opt $T: x);
/** @group stdlib.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, var opt set of $T: x);
/** @group stdlib.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, array[$U] of var opt $T: x);

/** @group stdlib.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, var opt $T: x);
/** @group stdlib.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, var opt set of $T: x);
/** @group stdlib.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, array[$U] of var opt $T: x);
/** @group stdlib.string String justification

Returns the string \a x right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format_justify_string(int: w, string: x);

/** @group stdlib.string Return path of file where this function is called */
function string: file_path();

