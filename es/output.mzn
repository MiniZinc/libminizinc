% Salida
<output> ::= <no-solutions> [ <warnings> ] <free-text>
           | ( <solution> )* [ <complete> ] [ <warnings> ] <free-text>

% Soluciones
<solution> ::= <solution-text> [ \n ] "----------" \n

% Insatisfactorio
<no-solutions> ::= "=====UNSATISFIABLE=====" \n

% Completo
<complete> ::= "==========" \n

% Mensajes
<warnings> ::= ( <message> )+

<message>  ::= ( <line> )+
<line>     ::= "%" [^\n]* \n
