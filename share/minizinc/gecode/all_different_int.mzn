%
%  Main authors:
%     Guido Tack <tack@gecode.org>
%
%  Copyright:
%     Guido Tack, 2007
%
%  Last modified:
%     $Date: 2009-09-09 01:42:03 +1000 (Wed, 09 Sep 2009) $ by $Author: schulte $
%     $Revision: 9689 $
%
%  This file is part of Gecode, the generic constraint
%  development environment:
%     http://www.gecode.org
%
%  Permission is hereby granted, free of charge, to any person obtaining
%  a copy of this software and associated documentation files (the
%  "Software"), to deal in the Software without restriction, including
%  without limitation the rights to use, copy, modify, merge, publish,
%  distribute, sublicense, and/or sell copies of the Software, and to
%  permit persons to whom the Software is furnished to do so, subject to
%  the following conditions:
%
%  The above copyright notice and this permission notice shall be
%  included in all copies or substantial portions of the Software.
%
%  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
%  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
%  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
%  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
%  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
%  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
%  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
%
%

predicate gecode_all_different_int(array[int] of var int: x);
%predicate all_different_int(array[int] of var int: x) = gecode_all_different_int(x);
predicate all_different_int(array[int] of var int: xs) = 
  let {
    set of int: cs = {fix(x) | x in xs where is_fixed(x)};
    array[int] of var int: ys = [x | x in xs where not is_fixed(x)];
    constraint forall(y in ys, c in cs) (y != c);
  } in
    if length(ys) <= 1 then true
    elseif length(ys) == 2 then ys[1] != ys[2]
    else gecode_all_different_int(ys)
    endif;

predicate all_different_int_reif(array[int] of var int: x, var bool: r) =
  r = forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );
