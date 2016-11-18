%
% jp-encoding.mzn
%

%
% There are several popular character encodings in Japan.
% (EUC-JP, SJIS, UTF-8)
%
% If they are mixed into one text file (by accident or something),
% text information will be lost.
% So we can define a problem "recovering original encodings" for byte stream.
%
% In this problem, byte stream (as variable 'stream') is given as input
% and encodings are assigned as output (as variable 'encoding').
%

include "table.mzn";

% *_score tables have -log(probability of appearance) * 10 for each encoding.

array[1..256] of int: sjis_score = [
135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 57, 135, 135, 135, 135, 135,
135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135,
74, 135, 128, 119, 135, 100, 135, 135, 96, 96, 112, 104, 106, 92, 106, 107,
97, 92, 99, 95, 98, 106, 107, 103, 97, 101, 105, 135, 135, 111, 135, 119,
65, 46, 45, 64, 75, 74, 79, 78, 71, 74, 79, 76, 71, 79, 59, 64,
81, 78, 70, 67, 80, 81, 70, 82, 69, 81, 75, 72, 62, 60, 73, 70,
76, 79, 75, 66, 77, 75, 78, 73, 77, 43, 43, 79, 54, 68, 68, 61,
73, 67, 80, 66, 69, 53, 51, 72, 73, 68, 74, 71, 77, 80, 77, 135,
77, 28, 11, 51, 75, 73, 69, 69, 51, 47, 49, 49, 44, 49, 41, 47,
44, 48, 48, 47, 50, 46, 45, 48, 63, 76, 75, 79, 74, 68, 63, 73,
50, 67, 40, 80, 45, 79, 56, 71, 59, 42, 45, 55, 67, 49, 68, 58,
69, 53, 65, 54, 69, 45, 57, 50, 64, 60, 65, 54, 67, 44, 49, 58,
66, 47, 58, 74, 43, 47, 43, 59, 43, 45, 62, 62, 40, 45, 60, 72,
71, 69, 72, 68, 61, 80, 62, 68, 72, 68, 70, 75, 51, 66, 67, 61,
47, 56, 59, 61, 62, 53, 53, 47, 51, 43, 49, 60, 74, 60, 83, 74,
47, 47, 76, 71, 78, 82, 77, 83, 80, 81, 70, 67, 67, 135, 135, 135
];
array[1..256] of int: eucjp_score = [
135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 57, 135, 135, 135, 135, 135,
135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135,
74, 135, 128, 119, 135, 100, 135, 135, 96, 96, 112, 104, 106, 92, 106, 107,
97, 92, 99, 95, 98, 106, 107, 103, 97, 101, 105, 135, 135, 111, 135, 119,
135, 112, 114, 100, 108, 102, 135, 113, 121, 110, 124, 135, 111, 106, 111, 111,
100, 135, 114, 109, 112, 124, 135, 135, 124, 119, 128, 128, 121, 128, 135, 135,
135, 94, 128, 102, 104, 95, 113, 97, 108, 90, 124, 124, 99, 101, 95, 90,
105, 135, 93, 99, 94, 108, 128, 113, 135, 124, 117, 135, 135, 135, 135, 135,
135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135,
135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135,
135, 28, 41, 44, 11, 50, 45, 72, 55, 64, 57, 42, 45, 54, 65, 46,
49, 49, 51, 48, 50, 48, 54, 41, 45, 45, 52, 44, 48, 45, 49, 39,
43, 50, 51, 43, 49, 53, 40, 43, 41, 50, 34, 36, 48, 45, 38, 43,
53, 66, 62, 66, 61, 62, 49, 50, 59, 62, 62, 63, 63, 68, 50, 63,
63, 58, 46, 55, 57, 59, 58, 52, 50, 46, 49, 43, 48, 58, 63, 58,
73, 67, 46, 46, 70, 68, 58, 66, 71, 73, 72, 74, 66, 61, 58, 135
];
array[1..256] of int: utf8_score = [
139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 61, 139, 139, 139, 139, 139,
139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139,
78, 139, 132, 123, 139, 104, 139, 139, 100, 100, 116, 108, 110, 96, 110, 111,
101, 96, 103, 99, 102, 110, 111, 107, 101, 105, 109, 139, 139, 115, 139, 139,
139, 116, 118, 104, 112, 107, 139, 117, 125, 114, 128, 139, 115, 110, 115, 115,
104, 139, 118, 113, 116, 128, 139, 139, 128, 123, 132, 132, 125, 132, 139, 139,
139, 98, 132, 107, 108, 99, 117, 101, 112, 95, 128, 128, 103, 105, 99, 94,
109, 139, 97, 103, 98, 112, 132, 117, 139, 128, 121, 139, 139, 139, 139, 139,
36, 17, 27, 51, 42, 55, 47, 54, 41, 40, 49, 38, 42, 47, 64, 48,
53, 52, 49, 45, 56, 51, 56, 46, 58, 50, 58, 52, 57, 51, 65, 46,
51, 53, 63, 48, 54, 54, 44, 48, 45, 54, 45, 46, 61, 53, 42, 47,
53, 61, 63, 57, 61, 63, 62, 62, 48, 59, 45, 53, 39, 57, 49, 57,
139, 139, 139, 132, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 118, 139,
139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139,
139, 139, 68, 14, 42, 36, 40, 44, 44, 47, 139, 139, 139, 139, 139, 40,
139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139
];


%
% labels for encoding
%

int: e_ascii = 0;
int: e_euc_jp = 1;
int: e_sjis = 2;
int: e_utf8 = 3;
int: e_unknown = 4;

%
% labels for byte_status
%

int: b_ascii = 0;

int: b_euc1 = 1;
int: b_euc2 = 2;

int: b_sjis1_1 = 3;
int: b_sjis2_1 = 4;
int: b_sjis2_2 = 5;

int: b_utf8_2_1 = 6;
int: b_utf8_2_2 = 7;
int: b_utf8_3_1 = 8;
int: b_utf8_3_2 = 9;
int: b_utf8_3_3 = 10;
int: b_utf8_4_1 = 11;
int: b_utf8_4_2 = 12;
int: b_utf8_4_3 = 13;
int: b_utf8_4_4 = 14;

int: b_unknown = 15;

%
% test data
%
% int: len = 12;
% array[1..len] of int: stream = [ 227, 129, 138, 227, 129, 175, 227, 130, 136, 227, 129, 134 ];
int: len;
set of int: rng = 1..len;

array[rng] of int: stream;

% int: len = 3	;
% array[rng] of int: stream = [ 65, 66, 67 ];

%------------------------------------------------------------------------------%
% Variables

array[rng] of var 0..b_unknown: byte_status;
array[rng] of var 0..e_unknown: encoding;
array[rng] of var 0..1: char_start;


int: maxObj = len * (max(i in 1..256)(eucjp_score[i] + sjis_score[i] + utf8_score[i]) + 1000);
var 0..maxObj: objective;

constraint objective = sum (i in rng) (
      score_of(encoding[i],stream[i])
    );

predicate score_computation(var 0..e_unknown:encoding_i, var 0..255:stream_i, var int: score_i) ::presolve(global) = 
  score_i = (bool2int(encoding_i == e_euc_jp) * eucjp_score[stream_i+1]
      + bool2int(encoding_i == e_sjis) * sjis_score[stream_i+1]
      + bool2int(encoding_i == e_utf8) * utf8_score[stream_i+1]
      + bool2int(encoding_i == e_unknown) * 1000);

function var int: score_of(var int: encoding_i, int: stream_i) = 
  let{
      var int: score_i;
      constraint score_computation(encoding_i,stream_i, score_i);
  } in score_i;

%------------------------------------------------------------------------------%
% Constraints

constraint forall (i in rng) (
  if i >= 2 then link_statuses(byte_status[i],byte_status[i-1]) else true endif
/\
  link_vars(encoding[i],byte_status[i],char_start[i])
/\
  init(byte_status[i],i)
/\
  stream_to_status(i,stream,byte_status[i])
);

predicate link_vars(var int: encoding_i, var int: byte_status_i, var int: char_start_i) ::presolve =
(forall([
%
% byte_status -> encoding
%
encoding_i = e_ascii <-> byte_status_i = b_ascii
,
    encoding_i = e_euc_jp ->
        byte_status_i = b_euc1 \/  byte_status_i = b_euc2
,
    (encoding_i = e_sjis ->
        byte_status_i = b_sjis1_1
        \/ byte_status_i = b_sjis2_1
        \/ byte_status_i = b_sjis2_2)
,
    encoding_i = e_utf8 ->
        byte_status_i = b_utf8_2_1
        \/ byte_status_i = b_utf8_2_2
        \/ byte_status_i = b_utf8_3_1
        \/ byte_status_i = b_utf8_3_2
        \/ byte_status_i = b_utf8_3_3
        \/ byte_status_i = b_utf8_4_1
        \/ byte_status_i = b_utf8_4_2
        \/ byte_status_i = b_utf8_4_3
        \/ byte_status_i = b_utf8_4_4
,
    byte_status_i = b_unknown <-> encoding_i = e_unknown
,
%
% byte_status -> char_start
%
    byte_status_i = b_ascii -> char_start_i = 1
,
    byte_status_i = b_utf8_2_1 -> char_start_i = 1
,
    byte_status_i = b_utf8_2_2 -> char_start_i = 0
,
    byte_status_i = b_utf8_3_1 -> char_start_i = 1
,
    byte_status_i in {b_utf8_3_2, b_utf8_3_3} -> char_start_i = 0
,
    byte_status_i = b_utf8_4_1 -> char_start_i = 1
,
    byte_status_i in {b_utf8_4_2, b_utf8_4_3, b_utf8_4_4} -> char_start_i = 0
,
    byte_status_i = b_euc1 -> char_start_i = 1
,
    byte_status_i = b_euc2 -> char_start_i = 0
,
    byte_status_i = b_sjis1_1 -> char_start_i = 1
,
    byte_status_i = b_sjis2_1 -> char_start_i = 1
,
    byte_status_i = b_sjis2_2 -> char_start_i = 0
,
%
% unknown
%
    byte_status_i = b_unknown -> char_start_i = 1
]));


predicate stream_to_status(int:i, array[int] of int: stream, var int: byte_status_i) = 
(  forall([
%
% ASCII
%
  if (stream[i] < 128) then true else
    byte_status_i != b_ascii endif
,
%
% UTF-8
%

% C2-DF, 80-BF
    if (i >= len) then
      byte_status_i != b_utf8_2_1
    else
    if    ((194 <= stream[i] /\ stream[i] <= 223)
          /\ (128 <= stream[i+1] /\ stream[i+1] <= 191))
    then true else
      byte_status_i != b_utf8_2_1
    endif
    endif
,

% E0-EF, 80-BF, 80-BF
    if (i >= len - 1) then
      byte_status_i != b_utf8_3_1
    else
      if  ((224 <= stream[i] /\ stream[i] <= 239)
         /\ (128 <= stream[i+1] /\ stream[i+1] <= 191)
         /\ (128 <= stream[i+2] /\ stream[i+2] <= 191))
      then true else
              byte_status_i != b_utf8_3_1
      endif
    endif
,

% F0-F7, 80-BF, 80-BF, 80-BF
    if (i >= len - 2) then
      byte_status_i != b_utf8_4_1
    else
      if (240 <= stream[i] /\ stream[i] <= 247
         /\ 128 <= stream[i+1] /\ stream[i+1] <= 191
         /\ 128 <= stream[i+2] /\ stream[i+2] <= 191
         /\ 128 <= stream[i+3] /\ stream[i+3] <= 191)
      then true else
        byte_status_i != b_utf8_4_1
      endif
    endif
,

%
% EUC-JP (CP51932)
% (A1-A8, AD, B0-F4, F9-FC), (A1-FE)
%
    if (i >= len) then
      byte_status_i !=b_euc1
    else
      if  ((161 <= stream[i] /\ stream[i] <= 168)
         \/ (173 = stream[i])
         \/ (176 <= stream[i] /\ stream[i] <= 244)
         \/ (249 <= stream[i] /\ stream[i] <= 252))
        /\ (161 <= stream[i+1] /\ stream[i+1] <= 254)
      then true else
        byte_status_i != b_euc1
      endif
    endif
,

%
% SJIS
%

% (A1-DF)
   if (161 <= stream[i] /\ stream[i] <= 223)
    then true else
    byte_status_i != b_sjis1_1
    endif
,

% (81-9F, E0-FC), (40-7E, 80-FC)
    if (i >= len) then
      byte_status_i != b_sjis2_1
    else
      if ((129 <= stream[i] /\ stream[i] <= 159)
          \/ (224 <= stream[i] /\ stream[i] <= 252))
       /\ ((64 <= stream[i+1] /\ stream[i+1] <= 126)
           \/ (128 <= stream[i+1] /\ stream[i+1] <= 252))
      then true else
        byte_status_i != b_sjis2_1
      endif
    endif
])
);

predicate stream_to_status_old(int:i, array[int] of int: stream, var int: byte_status_i) = 
(  forall([
%
% ASCII
%
  if (stream[i] < 128) then true else
    byte_status_i != b_ascii endif
,
%
% UTF-8
%

% C2-DF, 80-BF
    if (i >= len) then
      byte_status_i != b_utf8_2_1
    else
      byte_status_i = b_utf8_2_1 ->
        ((194 <= stream[i] /\ stream[i] <= 223)
          /\ (128 <= stream[i+1] /\ stream[i+1] <= 191))
    endif
,

% E0-EF, 80-BF, 80-BF
    if (i >= len - 1) then
      byte_status_i != b_utf8_3_1
    else
      byte_status_i = b_utf8_3_1 ->
        ((224 <= stream[i] /\ stream[i] <= 239)
         /\ (128 <= stream[i+1] /\ stream[i+1] <= 191)
         /\ (128 <= stream[i+2] /\ stream[i+2] <= 191))
    endif
,

% F0-F7, 80-BF, 80-BF, 80-BF
    if (i >= len - 2) then
      byte_status_i != b_utf8_4_1
    else
      byte_status_i = b_utf8_4_1 ->
        (240 <= stream[i] /\ stream[i] <= 247
         /\ 128 <= stream[i+1] /\ stream[i+1] <= 191
         /\ 128 <= stream[i+2] /\ stream[i+2] <= 191
         /\ 128 <= stream[i+3] /\ stream[i+3] <= 191)
    endif
,

%
% EUC-JP (CP51932)
% (A1-A8, AD, B0-F4, F9-FC), (A1-FE)
%
    if (i >= len) then
      byte_status_i !=b_euc1
    else
      byte_status_i = b_euc1 ->
        ((161 <= stream[i] /\ stream[i] <= 168)
         \/ (173 = stream[i])
         \/ (176 <= stream[i] /\ stream[i] <= 244)
         \/ (249 <= stream[i] /\ stream[i] <= 252))
        /\ (161 <= stream[i+1] /\ stream[i+1] <= 254)
    endif
,

%
% SJIS
%

% (A1-DF)
   byte_status_i = b_sjis1_1 ->
     (161 <= stream[i] /\ stream[i] <= 223)
,

% (81-9F, E0-FC), (40-7E, 80-FC)
    if (i >= len) then
      byte_status_i != b_sjis2_1
    else
      byte_status_i = b_sjis2_1 ->
        ((129 <= stream[i] /\ stream[i] <= 159)
          \/ (224 <= stream[i] /\ stream[i] <= 252))
       /\ ((64 <= stream[i+1] /\ stream[i+1] <= 126)
           \/ (128 <= stream[i+1] /\ stream[i+1] <= 252))
    endif
])
);


predicate link_statuses(var int: byte_status_i, var int: byte_status_im1) ::presolve = 
(  forall([
%
% UTF-8
%

% C2-DF, 80-BF
      byte_status_i == b_utf8_2_2 <-> byte_status_im1 == b_utf8_2_1
,

% E0-EF, 80-BF, 80-BF
      byte_status_i == b_utf8_3_2 <-> byte_status_im1 == b_utf8_3_1
,
      byte_status_i == b_utf8_3_3 <-> byte_status_im1 == b_utf8_3_2
,

% F0-F7, 80-BF, 80-BF, 80-BF
      byte_status_i == b_utf8_4_2 <-> byte_status_im1 == b_utf8_4_1
,
      byte_status_i == b_utf8_4_3 <-> byte_status_im1 == b_utf8_4_2
,
      byte_status_i == b_utf8_4_4 <-> byte_status_im1 == b_utf8_4_3
,

%
% EUC-JP (CP51932)
% (A1-A8, AD, B0-F4, F9-FC), (A1-FE)
%
      byte_status_i == b_euc2 <-> byte_status_im1 = b_euc1
,

%
% SJIS
%

% (A1-DF)

% (81-9F, E0-FC), (40-7E, 80-FC)
      byte_status_i == b_sjis2_2 <-> byte_status_im1 == b_sjis2_1
])
);


predicate init(var int: byte_status_i, int: i) = 
(  forall([
%
% UTF-8
%

% C2-DF, 80-BF
    if (i < 2) then
      byte_status_i != b_utf8_2_2
    else
      true
    endif
,

% E0-EF, 80-BF, 80-BF
    if (i < 2) then
      byte_status_i != b_utf8_3_2
    else
      true
    endif
,
    if (i < 3) then
      byte_status_i != b_utf8_3_3
    else
      true
    endif
,

% F0-F7, 80-BF, 80-BF, 80-BF
    if (i < 2) then
      byte_status_i != b_utf8_4_2
    else
      true
    endif
,
    if (i < 3) then
      byte_status_i != b_utf8_4_3
    else
      true
    endif
,
    if (i < 4) then
      byte_status_i != b_utf8_4_4
    else
      true
    endif
,

%
% EUC-JP (CP51932)
% (A1-A8, AD, B0-F4, F9-FC), (A1-FE)
%
    if (i < 2) then
      byte_status_i != b_euc2
    else
      true
    endif
,

%
% SJIS
%

% (A1-DF)

% (81-9F, E0-FC), (40-7E, 80-FC)
    if (i < 2) then
      byte_status_i != b_sjis2_2
    else
      true
    endif
])
);


%------------------------------------------------------------------------------%
% Search and Solve

solve
    :: seq_search([
        int_search(byte_status, first_fail, indomain_split, complete),
        int_search(char_start, input_order, indomain_max, complete),
        int_search(encoding, first_fail, indomain_split, complete)
    ])
    minimize objective;

%------------------------------------------------------------------------------%
% Output

output [
    "encoding = ", show(encoding), ";\n",
    "byte_status = ", show(byte_status), ";\n",
    "char_start = ", show(char_start), ";\n",
    "constraint objective = ", show(objective), ";\n"
]
