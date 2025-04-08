%{
#include<stdio.h>
int i=0,id=0;
%}
%%
[#].*[<].*[>]\n {}
[ \t\n]+ {}
\/\/.*\n {}
\/\*(.*\n)*.*\*\/ {}
auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto
|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsig
ned|void|volatile|while {printf("token: %d < keyword, %s >\n",++i,yytext);}
[+\-\*\/%<>] {printf("token: %d < operator, %s >\n",++i,yytext);}
[();{}] {printf("token: %d < special char, %s >\n",++i,yytext);}
[0-9]+ {printf("token: %d < constant, %s >\n",++i,yytext);}
[a-zA-Z_][a-zA-Z0-9_]* {printf("token: %d < ID %d, %s >\n",++i,++id,yytext);}
^[^a-zA-Z_] {printf("ERROR INVALID TOKEN %s\n",yytext);}
%%
