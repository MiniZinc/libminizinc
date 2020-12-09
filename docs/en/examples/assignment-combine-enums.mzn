include "alldifferent_except.mzn";

enum Flatmates = { Anne, Bert, Ceci, Dave };
enum Robots = { R2D2, C3PO, Marvin };

% Higher number = higher preference
array[Flatmates,Chores] of int: preference =
  [| 1, 2, 3, 4, 5 
   | 2, 1, 3, 4, 5 
   | 3, 5, 4, 1, 2 
   | 1, 5, 4, 2, 3 |];

array[Robots,Chores] of int: benefit =
  [| 20, 100, 20, 100, 30 
   | 10, 120, 40, 40,  60  
   | 50, 500, 30, 10,  70 |];

enum Chores = { Cooking, Vacuuming, Bathroom, Kitchen, Rubbish };

enum Workers = F(Flatmates) ++ R(Robots);

enum ChoreOrNothing = C(Chores) ++ { Nothing };

% Create preference and benefit arrays with added values for Nothing

array[Flatmates,ChoreOrNothing] of int: prefWithNothing =
  array2d(Flatmates,ChoreOrNothing,
          [ if c=Nothing then 0 else preference[f,C^-1(c)] endif
          | f in Flatmates, c in ChoreOrNothing]);

array[Robots,ChoreOrNothing] of int: benefitWithNothing =
  array2d(Robots,ChoreOrNothing,
          [ if c=Nothing then 0 else benefit[r,C^-1(c)] endif
          | r in Robots, c in ChoreOrNothing]);

array[Workers] of var ChoreOrNothing: assignment;

constraint alldifferent_except(assignment, {Nothing});

var int: overall_preferences =
  sum(f in Flatmates)(prefWithNothing[f, assignment[F(f)]]);

var int: overall_benefit =
  sum(r in Robots)(benefitWithNothing[r, assignment[R(r)]]);

solve maximize overall_benefit + overall_preferences;

output [ if w in F(Flatmates) then show(F^-1(w)) else show(R^-1(w)) endif++":\t"++
         if fix(assignment[w])=Nothing then "Nothing\n"
         else show(C^-1(assignment[w]))++"\n" endif
       | w in Workers];
