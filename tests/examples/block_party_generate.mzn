/* The Block Party generation model
 * Author: Jean-Noël Monette
 * Original game by Three Pixel Heart LLC
 */

include "globals.mzn";

set of int: cubes=1..8;
set of int: pos=1..24;
set of int: symbols=0..4*4*4-1;
set of int: places=1..8*24;
set of int: faces= 1..6;
array[faces] of set of pos: face = [{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16},{17,18,19,20},{21,22,23,24}];
set of int: chars = 0..3;

array[cubes,pos] of var symbols: data;

constraint global_cardinality_closed(array1d(data),symbols,[3|i in symbols])::domain;

constraint forall(c in 7..8)(all_different([data[c,i]|i in pos])::domain);

%redundant constraint for the previous ones
array[cubes,symbols] of var 0..1: cnt;
constraint forall(c in 1..8)(global_cardinality_closed([data[c,i]|i in pos],symbols,[cnt[c,i]|i in symbols])::domain);
constraint forall(s in symbols)(sum(c in cubes)(cnt[c,s])=3);

constraint forall(c in cubes, f in faces)(
	   global_cardinality_low_up_closed([color(data[c,p])|p in face[f]],chars,[0|i in chars],[3|i in chars])::domain
	   /\
	   global_cardinality_low_up_closed([fill(data[c,p])|p in face[f]],chars,[0|i in chars],[3|i in chars])::domain
	   /\
	   global_cardinality_low_up_closed([shape(data[c,p])|p in face[f]],chars,[0|i in chars],[3|i in chars])::domain
);

%I believe this set of constraints is redundant with the previous one
constraint forall(c in cubes, f in faces)(
	   nvalue([color(data[c,p])|p in face[f]]) >= 2
	   /\
	   nvalue([fill(data[c,p])|p in face[f]]) >= 2
	   /\
	   nvalue([shape(data[c,p])|p in face[f]]) >= 2
);

solve
::seq_search([
  int_search(cube_at,input_order,indomain_random,complete),
  int_search(symbol_at,input_order,indomain_random,complete),
  int_search(rotation,input_order,indomain_random,complete),
  int_search(array1d(data),input_order,indomain_random,complete),
])
satisfy;

output ["data =",show2d(data),";\n","% cubes: ",show(cube_at),"\n% symbols: ",show(symbol_at),"\n% rotations: ",show(rotation)];

%How to implement linking functions with the advantages of CSE.
function array[1..3] of var 0..3: fcs(var 0..63:symbol) :: promise_total =
         let { var 0..3: color;  var 0..3:shape;var 0..3: fill;
         constraint  symbol = 4*4*fill+4*color+shape; }
         in [fill,color,shape];
function var 0..3: color_help(array[1..3] of var 0..3: fcs) :: promise_total = fcs[2];
function var 0..3: color(var int:symbol) ::promise_total = color_help(fcs(symbol));
function var 0..3: fill_help(array[1..3] of var 0..3: fcs) :: promise_total = fcs[1];
function var 0..3: fill(var int:symbol) ::promise_total = fill_help(fcs(symbol));
function var 0..3: shape_help(array[1..3] of var 0..3: fcs) :: promise_total = fcs[3];
function var 0..3: shape(var int:symbol) ::promise_total = shape_help(fcs(symbol));

int: ud=0;
int: lr=8;
int: fb=16;

array[cubes] of var cubes: cube_at;
array[pos] of var symbols: symbol_at;
array[cubes] of var 1..24: rotation;

%Each cube is placed once.
constraint alldifferent(cube_at);

%Party constraints on the 6 faces
constraint party([symbol_at[i] | i in 1..4]);
constraint party([symbol_at[i + 4] | i in 1..4]);
constraint party([symbol_at[(i - 1) * 2 + 1 + lr] | i in 1..4]);
constraint party([symbol_at[i * 2 + lr] | i in 1..4]);
constraint party([symbol_at[if i < 3 then i else i+2 endif + fb] | i in 1..4]);
constraint party([symbol_at[if i < 3 then i else i+2 endif + 2 + fb] | i in 1..4]);

%Linking cubes and symbols
constraint forall(i in {1,4,6,7})(
	   link_cube_and_symbols(cube_at[i],[symbol_at[i+ud],symbol_at[i+lr],symbol_at[i+fb]],rotation[i],data));
constraint forall(i in {2,3,5,8})(
	   link_cube_and_symbols(cube_at[i],[symbol_at[i+ud],symbol_at[i+fb],symbol_at[i+lr]],rotation[i],data));

%Note: this presolve does not change much
predicate diff_or_equal(array[1..4] of var 0..3: x) =
	  forall(i in 1..4,j in i+1..4)(x[i]!=x[j])
	  \/
	  forall(i in 1..4,j in i+1..4)(x[i]=x[j]);

% A party is alldiff or allequal for each of the three characteristics.
% Note: I never tried to (manually) presolve this one but it might be worth trying it as well...
predicate party(array[1..4] of var 0..63: symbols) = (
	  diff_or_equal([color(symbols[i]) | i in 1..4])
	  /\ diff_or_equal([shape(symbols[i]) | i in 1..4])
	  /\ diff_or_equal([fill(symbols[i]) | i in 1..4]) );


%Linking cube IDs with the three symbols appearing in relevant positions
predicate link_cube_and_symbols(var int: cube, array[1..3] of var int: symbols,var 1..24: pos,array[cubes,pos] of var symbols:data) =
	  forall(i in 1..3)(data[cube,pp[pos,i]]=symbols[i]);

%Which symbol positions on a cube are next to each other on the same corner.
%Made by hand, I do not think there is any hope for finding a simple formula that could be used in precomputation :-)
%Actually it might be divided by three and have a disjunction in link_cube_and_symbols
array[1..24,1..3] of int: pp = [|21,12,7
|23,17,11
|24,4,18
|22,8,3
|1,6,16
|2,14,20
|4,18,24
|3,22,8
|7,21,12
|5,10,15
|6,16,1
|8,3,22
|16,1,6
|15,5,10
|13,9,19
|14,20,2
|18,24,4
|20,2,14
|19,13,9
|17,11,23
|12,7,21
|11,23,17
|9,19,13
|10,15,5
|];
