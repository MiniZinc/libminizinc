% 变量
var float: R;        % 季度还款
var float: P;        % 初始借贷本金
var 0.0 .. 10.0: I;  % 利率

% 中间变量
var float: B1; % 一个季度后的欠款
var float: B2; % 两个季度后的欠款
var float: B3; % 三个季度后的欠款
var float: B4; % 最后欠款

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
