% variables
var float: R;        % quarterly repayment
var float: P;        % principal initially borrowed
var 0.0 .. 10.0: I;  % interest rate (per quarter)

% intermediate variables
var float: B1; % balance after one quarter
var float: B2; % balance after two quarters
var float: B3; % balance after three quarters
var float: B4; % balance owing at end

constraint B1 = P * (1.0 + I) - R;
constraint B2 = B1 * (1.0 + I) - R;
constraint B3 = B2 * (1.0 + I) - R; 
constraint B4 = B3 * (1.0 + I) - R;

solve satisfy;

output [
 "Borrowing ", show_float(0, 2, P), " at ", show(I*100.0), 
 "% interest, and repaying ", show_float(0, 2, R), 
  "\nper quarter for 1 year leaves ", show_float(0, 2, B4), " owing\n"
];

% inputs to allow us to enter values from playground/IDE
opt float: R_IN;     % quarterly repayment
opt float: P_IN;     % principal initially borrowed
opt float: I_IN;     % interest rate (per quarter)
opt float: B4_IN;    % balance owing at end

constraint R ~= R_IN;
constraint P ~= P_IN;
constraint I ~= I_IN;
constraint B4 ~= B4_IN;
