include "alldifferent_except.mzn";

enum Flatmates = { Anne, Bert, Ceci, Dave };
enum Chores = { Cooking, Vacuuming, Bathroom, Kitchen, Rubbish };

enum ChoreOrNothing = C(Chores) ++ { Nothing };

array[Flatmates] of var ChoreOrNothing: assignment;

constraint alldifferent_except(assignment, {Nothing});

output [ show(f)++":\t"++
         if fix(assignment[f])=Nothing then "Nothing\n"
         else show(C^-1(assignment[f]))++"\n" endif
       | f in Flatmates];
