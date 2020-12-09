% Output
<output> ::= <no-solutions> [ <warnings> ] <free-text>
           | ( <solution> )* [ <complete> ] [ <warnings> ] <free-text>

% Solutions
<solution> ::= <solution-text> [ \n ] "----------" \n

% Unsatisfiable
<no-solutions> ::= "=====UNSATISFIABLE=====" \n

% Complete
<complete> ::= "==========" \n

% Messages
<warnings> ::= ( <message> )+

<message>  ::= ( <line> )+
<line>     ::= "%" [^\n]* \n
