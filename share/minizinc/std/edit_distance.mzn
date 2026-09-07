include "fzn_edit_distance.mzn";
include "fzn_edit_distance_exact.mzn";

/** @group globals
  Constrains \a ED to be greater than or equal to the minimum edit distance between two strings \a S1 and \a S2.
  Any character insertion or deletion has a cost of 1, whereas any character substitution has a cost of 2.
  All characters need to be numbered from 1 to \a max_char.
  The integer arrays \a S1 and \a S2 need to have the same length, but may contain zeros at the end to indicate an "empty" character.
  Example: Let \a max_char = 2, \a S1 = [1,1,0,0,0], \a S2 = [1,2,1,0,0], then \a ED = 1, as the minimum edit distance is 1 between the strings "11" and "121" (a single '2' character needs to be inserted).
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param S1: Integer array that represents the first string
@param S2: Integer array that represents the second string
@param ED: Variable that is constrained to be >= than the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    assert(length(S1) == length(S2),
        "edit_distance: string arrays S1 and S2 need to have the same length",
    assert(max_char >= 1,
        "edit_distance: max_char must be >= 1",
    fzn_edit_distance(max_char, S1, S2, ED)
    ));

/** @group globals
  Constrains \a ED to be greater than or equal to the minimum edit distance between two strings \a S1 and \a S2.
  This constraint functions in the same way as the constraint \a edit_distance(max_char, S1, S2, ED), however it additionally specifies custom character insertion, deletion and substitution costs.
  All costs should be >= 1 and the following inequality must hold: C_sub <= C_ins + C_del
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param C_ins: Costs for a single character insertion
@param C_del: Costs for a single character deletion
@param C_sub: Costs for a single character substitution
@param S1: Integer array that represents the first string
@param S2: Integer array that represents the second string
@param ED: Variable that is constrained to be >= than the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance(int: max_char,
                        int: C_ins,
                        int: C_del,
                        int: C_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    assert(length(S1) == length(S2),
        "edit_distance: string arrays S1 and S2 need to have the same length",
    assert(max_char >= 1,
        "edit_distance: max_char must be >= 1",
    assert(C_sub <= C_ins + C_del,
        "edit_distance: The following inequality must hold C_sub <= C_ins + C_del",
    assert(C_sub >= 1 /\ C_ins >= 1 /\ C_del >= 1,
        "edit_distance: C_sub, C_ins, C_del all must be >= 1",
    fzn_edit_distance(max_char, C_ins, C_del, C_sub, S1, S2, ED)
    ))));
    
/** @group globals
  Constrains \a ED to be greater than or equal to the minimum edit distance between two strings \a S1 and \a S2.
  This constraint functions in the same way as the constraint \a edit_distance(max_char, S1, S2, ED), however it additionally specifies custom character insertion, deletion and substitution costs for each individual combination of characters.
  All costs should be >= 1, except the substitution costs of a character with itself which should be 0 (W_sub[c,c] = 0).
  Furthermore, the following inequality must hold for all pairs of characters c1 and c2: \a W_sub[c1,c2] <= \a W_del[c1] + \a W_ins[c2]
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param W_ins: Integer array containing the costs for single character insertions
@param W_del: Integer array containing the costs for single character deletions
@param W_sub: Integer array containing the costs for single character substitutions
@param S1: Integer array that represents the first string
@param S2: Integer array that represents the second string
@param ED: Variable that is constrained to be >= than the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance(int: max_char,
                        array[int] of int: W_ins,
                        array[int] of int: W_del,
                        array[int, int] of int: W_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    assert(length(S1) == length(S2),
        "edit_distance: string arrays S1 and S2 need to have the same length",
    assert(index_set(W_ins) == 1..max_char /\ index_set(W_del) == 1..max_char,
        "edit_distance: index sets of W_ins and W_del must be 1..max_char",
    assert(index_set_1of2(W_sub) == 1..max_char /\ index_set_2of2(W_sub) == 1..max_char,
        "edit_distance: both index sets of W_sub must be 1..max_char",
    assert(max_char >= 1,
        "edit_distance: max_char must be >= 1",
    assert(forall(c1 in 1..max_char, c2 in 1..max_char)(W_sub[c1,c2] <= W_ins[c2] + W_del[c1]),
        "edit_distance: The following inequality must hold for all c1, c2 in 1..max_char: W_sub[c1,c2] <= W_ins[c2] + W_del[c1]",
    assert(forall(c in 1..max_char)(W_ins[c] >= 1 /\ W_del[c] >= 1),
        "edit_distance: for all c in 1..max_char: W_ins[c] and W_del[c] must be >= 1",
    assert(forall(c1 in 1..max_char, c2 in 1..max_char where c1 != c2)(W_sub[c1,c2] >= 1),
        "edit_distance: for all c1, c2 in 1..max_char where c1 != c2: W_sub[c1,c2] must be >= 1",
    assert(forall(c in 1..max_char)(W_sub[c,c] == 0),
        "edit_distance: for all c in 1..max_char: W_sub[c,c] must be == 0",
    fzn_edit_distance(max_char, W_ins, W_del, W_sub, S1, S2, ED)
    ))))))));
    
/** @group globals
  Constrains \a ED to be equal to the minimum edit distance between two strings \a S1 and \a S2 (Note that this constraint is likely to be less efficient than its "edit_distance" version when you are aiming to minimize the edit distance).
  Any character insertion or deletion has a cost of 1, whereas any character substitution has a cost of 2.
  All characters need to be numbered from 1 to \a max_char.
  The integer arrays \a S1 and \a S2 need to have the same length, but may contain zeros at the end to indicate an "empty" character.
  Example: Let \a max_char = 2, \a S1 = [1,1,0,0,0], \a S2 = [1,2,1,0,0], then \a ED = 1, as the minimum edit distance is 1 between the strings "11" and "121" (a single '2' character needs to be inserted).
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param S1: Integer array that represents the first string
@param S2: Integer array that represents the second string
@param ED: Variable that is constrained to be equal to the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance_exact(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    edit_distance_exact(max_char, 1, 1, 2, S1, S2, ED);

/** @group globals
  Constrains \a ED to be equal to the minimum edit distance between two strings \a S1 and \a S2 (Note that this constraint is likely to be less efficient than its "edit_distance" version when you are aiming to minimize the edit distance).
  This constraint functions in the same way as the constraint \a edit_distance_exact(max_char, S1, S2, ED), however it additionally specifies custom character insertion, deletion and substitution costs.
  All costs should be >= 1 and the following inequality must hold: C_sub <= C_ins + C_del
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param C_ins: Costs for a single character insertion
@param C_del: Costs for a single character deletion
@param C_sub: Costs for a single character substitution
@param S1: Integer array that represents the first string
@param S2: Integer array that represents the second string
@param ED: Variable that is constrained to be equal to the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance_exact(int: max_char,
                        int: C_ins,
                        int: C_del,
                        int: C_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    assert(C_sub <= C_ins + C_del,
        "edit_distance: The following inequality must hold C_sub <= C_ins + C_del",
    assert(C_sub >= 1 /\ C_ins >= 1 /\ C_del >= 1,
        "edit_distance: C_sub, C_ins, C_del all must be >= 1",
    edit_distance_exact(max_char, [C_ins | i in 1..max_char], [C_del | i in 1..max_char], array2d(1..max_char, 1..max_char, [if i == j then 0 else C_sub endif | i in 1..max_char, j in 1..max_char]), S1, S2, ED)
    ));
    
/** @group globals
  Constrains \a ED to be equal to the minimum edit distance between two strings \a S1 and \a S2 (Note that this constraint is likely to be less efficient than its "edit_distance" version when you are aiming to minimize the edit distance).
  This constraint functions in the same way as the constraint \a edit_distance_exact(max_char, S1, S2, ED), however it additionally specifies custom character insertion, deletion and substitution costs for each individual combination of characters.
  All costs should be >= 1, except the substitution costs of a character with itself which should be 0 (W_sub[c,c] = 0).
  Furthermore, the following inequality must hold for all pairs of characters c1 and c2: \a W_sub[c1,c2] <= \a W_del[c1] + \a W_ins[c2]
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param W_ins: Integer array containing the costs for single character insertions
@param W_del: Integer array containing the costs for single character deletions
@param W_sub: Integer array containing the costs for single character substitutions
@param S1: Integer array that represents the first string
@param S2: Integer array that represents the second string
@param ED: Variable that is constrained to be equal to the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance_exact(int: max_char,
                        array[int] of int: W_ins,
                        array[int] of int: W_del,
                        array[int, int] of int: W_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    assert(length(S1) == length(S2),
        "edit_distance: string arrays S1 and S2 need to have the same length",
    assert(index_set(W_ins) == 1..max_char /\ index_set(W_del) == 1..max_char,
        "edit_distance: index sets of W_ins and W_del must be 1..max_char",
    assert(index_set_1of2(W_sub) == 1..max_char /\ index_set_2of2(W_sub) == 1..max_char,
        "edit_distance: both index sets of W_sub must be 1..max_char",
    assert(max_char >= 1,
        "edit_distance: max_char must be >= 1",
    assert(forall(c1 in 1..max_char, c2 in 1..max_char)(W_sub[c1,c2] <= W_ins[c2] + W_del[c1]),
        "edit_distance: The following inequality must hold for all c1, c2 in 1..max_char: W_sub[c1,c2] <= W_ins[c2] + W_del[c1]",
    assert(forall(c in 1..max_char)(W_ins[c] >= 1 /\ W_del[c] >= 1),
        "edit_distance: for all c in 1..max_char: W_ins[c] and W_del[c] must be >= 1",
    assert(forall(c1 in 1..max_char, c2 in 1..max_char where c1 != c2)(W_sub[c1,c2] >= 1),
        "edit_distance: for all c1, c2 in 1..max_char where c1 != c2: W_sub[c1,c2] must be >= 1",
    assert(forall(c in 1..max_char)(W_sub[c,c] == 0),
        "edit_distance: for all c in 1..max_char: W_sub[c,c] must be == 0",
    fzn_edit_distance_exact(max_char, W_ins, W_del, W_sub, S1, S2, ED)
    ))))))));