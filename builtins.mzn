% Comparisons
function     bool: '<'(    $T,    $T);
function var bool: '<'(any $T,any $T);
function     bool: '>'(    $T,    $T);
function var bool: '>'(any $T,any $T);
function     bool: '<='(    $T,    $T);
function var bool: '<='(any $T,any $T);
function     bool: '>='(    $T,    $T);
function var bool: '>='(any $T,any $T);
function     bool: '='(    $T,    $T);
function var bool: '='(any $T,any $T);
function     bool: '!='(    $T,    $T);
function var bool: '!='(any $T,any $T);

function     bool: '<'(array[$U] of     $T,array[$U] of     $T);
function var bool: '<'(array[$U] of any $T,array[$U] of any $T);
function     bool: '>'(array[$U] of     $T,array[$U] of     $T);
function var bool: '>'(array[$U] of any $T,array[$U] of any $T);
function     bool: '<='(array[$U] of     $T,array[$U] of     $T);
function var bool: '<='(array[$U] of any $T,array[$U] of any $T);
function     bool: '>='(array[$U] of     $T,array[$U] of     $T);
function var bool: '>='(array[$U] of any $T,array[$U] of any $T);
function     bool: '='(array[$U] of     $T,array[$U] of     $T);
function var bool: '='(array[$U] of any $T,array[$U] of any $T);
function     bool: '!='(array[$U] of     $T,array[$U] of     $T);
function var bool: '!='(array[$U] of any $T,array[$U] of any $T);

% Arithmetic operations
function     int:   '+'(    int,     int);
function var int:   '+'(var int,  var int);
function     float: '+'(    float,    float);
function var float: '+'(var float,var float);
function     int:   '-'(    int,     int);
function var int:   '-'(var int,  var int);
function     float: '-'(    float,    float);
function var float: '-'(var float,var float);
function     int:   '*'(    int,     int);
function var int:   '*'(var int,  var int);
function     float: '*'(    float,    float);
function var float: '*'(var float,var float);
function     int:   '-'(    int);
function var int:   '-'(var int);
function     float: '-'(    float);
function var float: '-'(var float);

function     int: 'div'(int,int);
function var int: 'div'(var int,var int);
function     int: 'mod'(int,int);
function var int: 'mod'(var int,var int);
function     float: '/'(    float,    float);
function var float: '/'(var float,var float);

function     int:   sum(array[$T] of     int);
function var int:   sum(array[$T] of var int);
function     float: sum(array[$T] of     float);
function var float: sum(array[$T] of var float);

function any $T: min(any $T, any $T);
function any $T: min(array[$U] of any $T);
function any $T: max(any $T, any $T);
function any $T: max(array[$U] of any $T);
function int: min(set of int);

function int: abs(int);
function var int: abs(var int);
function float: abs(float);
function var float: abs(var float);

function float: sqrt(float);
function var float: sqrt(var float);

function int: pow(int, int);
function float: pow(float, float);

function float: exp(float);
function var float: exp(var float);
function float: ln(float);
function var float: ln(var float);
function float: log10(float);
function var float: log10(var float);
function float: log2(float);
function var float: log2(var float);

function float: log(float, float);

function float: sin(float);
function var float: sin(var float);
function float: cos(float);
function var float: cos(var float);
function float: tan(float);
function var float: tan(var float);
function float: asin(float);
function var float: asin(var float);
function float: acos(float);
function var float: acos(var float);
function float: atan(float);
function var float: atan(var float);
function float: sinh(float);
function var float: sinh(var float);
function float: cosh(float);
function var float: cosh(var float);
function float: tanh(float);
function var float: tanh(var float);
function float: asinh(float);
function var float: asinh(var float);
function float: acosh(float);
function var float: acosh(var float);
function float: atanh(float);
function var float: atanh(var float);

% Logical operations
function     bool: '/\'(    bool,     bool);
function var bool: '/\'(var bool, var bool);
function     bool: '\/'(    bool,     bool);
function var bool: '\/'(var bool, var bool);
function     bool: '->'(    bool,     bool);
function var bool: '->'(var bool, var bool);
function     bool: '<-'(    bool,     bool);
function var bool: '<-'(var bool, var bool);
function     bool: '<->'(    bool,     bool);
function var bool: '<->'(var bool, var bool);
function     bool: 'xor'(    bool,     bool);
function var bool: 'xor'(var bool, var bool);
function     bool: 'not'(    bool);
function var bool: 'not'(var bool);
function     bool: forall(array[$T] of     bool);
function var bool: forall(array[$T] of var bool);
function     bool: exists(array[$T] of     bool);
function var bool: exists(array[$T] of var bool);
function     bool: xorall(array[$T] of     bool);
function var bool: xorall(array[$T] of var bool);
function     bool: iffall(array[$T] of     bool);
function var bool: iffall(array[$T] of var bool);

% Set operations
function     bool: 'in'(     $T,     set of $T);
function var bool: 'in'(var int, var set of int);

function     bool: 'subset'(    set of $T,      set of $T);
function var bool: 'subset'(var set of int, var set of int);
function     bool: 'superset'(    set of $T,      set of $T);
function var bool: 'superset'(var set of int, var set of int);

function     set of $T:  'union'(    set of $T,      set of $T);
function var set of int: 'union'(var set of int, var set of int);
function     set of $T:  'intersect'(    set of $T,      set of $T);
function var set of int: 'intersect'(var set of int, var set of int);
function     set of $T:  'diff'(    set of $T,      set of $T);
function var set of int: 'diff'(var set of int, var set of int);
function     set of $T:  'symdiff'(    set of $T,      set of $T);
function var set of int: 'symdiff'(var set of int, var set of int);

function set of int: '..'(int,int);
function set of float: '..'(float,float);

function     int: card(    set of $T);
function var int: card(var set of int);

function     set of $U:  array_union(array[$T] of     set of $U);
function var set of int: array_union(array[$T] of var set of int);
function     set of $U:  array_intersect(array[$T] of     set of $U);
function var set of int: array_intersect(array[$T] of var set of int);

% Array operations
function int: length(array[$T] of any $U);
function array[int] of any $T: '++'(array[int] of any $T,
                                    array[int] of any $T);

function set of int: index_set(array[int] of any $U);
function set of int: index_set_1of2(array[int,int] of any $U);
function set of int: index_set_2of2(array[int,int] of any $U);
function set of int: index_set_1of3(array[int,int,int] of any $U);
function set of int: index_set_2of3(array[int,int,int] of any $U);
function set of int: index_set_3of3(array[int,int,int] of any $U);
function set of int: index_set_1of4(array[int,int,int,int] of any $U);
function set of int: index_set_2of4(array[int,int,int,int] of any $U);
function set of int: index_set_3of4(array[int,int,int,int] of any $U);
function set of int: index_set_4of4(array[int,int,int,int] of any $U);
function set of int: index_set_1of5(array[int,int,int,int,int] of any $U);
function set of int: index_set_2of5(array[int,int,int,int,int] of any $U);
function set of int: index_set_3of5(array[int,int,int,int,int] of any $U);
function set of int: index_set_4of5(array[int,int,int,int,int] of any $U);
function set of int: index_set_5of5(array[int,int,int,int,int] of any $U);
function set of int: index_set_1of6(array[int,int,int,int,int,int] of any $U);
function set of int: index_set_2of6(array[int,int,int,int,int,int] of any $U);
function set of int: index_set_3of6(array[int,int,int,int,int,int] of any $U);
function set of int: index_set_4of6(array[int,int,int,int,int,int] of any $U);
function set of int: index_set_5of6(array[int,int,int,int,int,int] of any $U);
function set of int: index_set_6of6(array[int,int,int,int,int,int] of any $U);

function array[int] of any $V: array1d(set of int,
                                       array[$U] of any $V);
function array[int,int] of any $V: array2d(set of int, set of int, 
                                           array[$U] of any $V);
function array[int,int,int] of any $V: array3d(set of int,
                                               set of int,
                                               set of int,
                                               array[$U] of any $V);
function array[int,int,int,int] of any $V: array4d(set of int,
                                                   set of int,
                                                   set of int,
                                                   set of int,
                                                   array[$U] of any $V);
function array[int,int,int,int,int] of any $V: array5d(set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       array[$U] of any $V);
function array[int,int,int,int,int,int] of any $V: array6d(set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           array[$U] of any $V);

% Coercion operations

function int: ceil(float);
function int: floor(float);
function int: round(float);

function int: bool2int(bool);
function var int: bool2int(var bool);
function float: int2float(int);
function var float: int2float(var int);
function array[int] of $T: set2array(set of $T);

% String operations

function string: show(any $T);
function string: show_int(int, var int);
function string: show_float(int, int, var float);
function string: '++'(string, string);
function string: concat(array[$T] of string);
function string: join(string, array[$T] of string);

% Bound and domain operations

function int: lb(var int);
function int: ub(var int);
function float: lb(var float);
function float: ub(var float);
function set of int: lb(var set of int);
function set of int: ub(var set of int);
function array[$U] of int: lb(array[$U] of var int);
function array[$U] of int: ub(array[$U] of var int);
function array[$U] of float: lb(array[$U] of var float);
function array[$U] of float: ub(array[$U] of var float);
function array[$U] of set of int: lb(array[$U] of var set of int);
function array[$U] of set of int: ub(array[$U] of var set of int);
function set of int: dom(var int);
function set of int: dom_array(array[$T] of var int);
function int: dom_size(var int);

% Other operations

function any $T: assert(bool, string, any $T);
function bool: assert(bool, string);
function any $T: trace(string, any $T);
function $T: fix(any $T);
function bool: is_fixed(any $T);
