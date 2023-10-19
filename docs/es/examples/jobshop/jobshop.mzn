enum JOB;
enum TASK;
TASK: last = max(TASK);

% Duración de tareas.
array [JOB,TASK] of int: d;

% Duración total.
int: total = sum(i in JOB, j in TASK)(d[i,j]);

% Dígitos para salida.
int: digs = ceil(log(10.0,int2float(total)));

% Horas de inicio.
array [JOB,TASK] of var 0..total: s;

% Tiempo de finalización total.
var 0..total: end;

% Asegurar de que las tareas se realicen en secuencia.
constraint
    forall(i in JOB) (
        forall(j in TASK where j < last)
            (s[i,j] + d[i,j] <= s[i,enum_next(TASK,j)]) /\
        s[i,last] + d[i,last] <= end
    );

% Garantizar que no se superponen las tareas.
constraint
    forall(j in TASK) (
        forall(i,k in JOB where i < k) (
            s[i,j] + d[i,j] <= s[k,j] \/
            s[k,j] + d[k,j] <= s[i,j]
        )
    );

solve minimize end;

output ["Fin = \(end)\n"] ++
       [ show_int(digs,s[i,j]) ++ " " ++
         if j == last then "\n" else "" endif |
         i in JOB, j in TASK ];
