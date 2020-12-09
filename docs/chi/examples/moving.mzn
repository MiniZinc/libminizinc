include "cumulative.mzn";

enum OBJECTS;
array[OBJECTS] of int: duration; % 移动的持续时间
array[OBJECTS] of int: handlers; % 需要的搬运工的数量
array[OBJECTS] of int: trolleys; % 需要的手推车的数量

int: available_handlers;
int: available_trolleys;
int: available_time;

array[OBJECTS] of var 0..available_time: start;
var 0..available_time: end;

constraint cumulative(start, duration, handlers, available_handlers);
constraint cumulative(start, duration, trolleys, available_trolleys);
 
constraint forall(o in OBJECTS)(start[o] +duration[o] <= end);

solve minimize end;

output [ "start = \(start)\nend = \(end)\n"];
