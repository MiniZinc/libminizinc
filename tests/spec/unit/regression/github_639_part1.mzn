var 1..2: x;
array[1..2] of var opt 1..1: arr;
constraint absent(arr[x - 1]) \/ absent(arr[x + 1]);
