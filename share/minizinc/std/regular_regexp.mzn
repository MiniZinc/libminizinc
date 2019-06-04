include "fzn_regular_regexp.mzn";

/** @group globals.extensional
  The sequence of values in array \a x  is accepted by the regular
  expression \a r. This constraint generates it's DFA equivalent.

  Regular expressions can use the following syntax:

  - Selection:

     - Concatenation: "12 34", 12 followed by 34.
       (Characters are assumed to be the part of the same number unless split
       by syntax or whitespace.)
     - Union: "7|11", a 7 or 11.
     - Groups: "7(6|8)", a 7 followed by a 6 or an 8.
     - Wildcard: ".", any value within the domain.
     - Classes: "[3-6 7]", a 3,4,5,6, or 7.
     - Negated classes: "[^3 5]", any value within the domain except for a 3 or a 5.

  - Quantifiers:

     - Asterisk: "12*", 0 or more times a 12.
     - Question mark: "5?", 0 or 1 times a 5. (optional)
     - Plus sign: "42+", 1 or more time a 42.
     - Exact: "1{3}", exactly 3 times a 1.
     - At least: "9{5,}", 5 or more times a 9.
     - Between: "7{3,5}", at least 3 times, but at most 5 times a 7.

  Members of enumerated types can be used in place of any integer (e.g., "A B",
  A followed by B). Enumerated identifiers still use whitespace for concatenation.
*/
predicate regular(array[int] of var int: x, string: r) = 
          fzn_regular(array[int] of var int: x, string: r);
