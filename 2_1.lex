% {
#include <stdio.h>
	  % } delim[|\t |\n] ws{delim} +
	letter[A - Za - z] digit[0 - 9] id{letter}({letter} | {digit}) * num{digit} + (\.{digit} +)
	? (E[+| -] ? {digit} +)
	? % %
{
	ws
}
{
	printf("Delimiter|whitespace \n");
}
if
	| else | then | int | float | char { printf("%s is an keyword \n", yytext); }
{
	id
}
{
	printf("%s is an identifier \n", yytext);
}
{
	num
}
{
	printf("it is an number \n");
}
"<" { printf("it is a relational operator less than \n"); }
"<=" { printf("it is a relational operator less than or equal \n"); }
">" { printf("it is a relational operator greater than \n"); }
">=" { printf("it is a relational operator greater than or equal \n"); }
"==" { printf("it is a relational operator equals \n"); }
"<>" { printf("it is a relational operator not equal \n"); }
"+" | "-" | "*" | "/" | "%" { printf("Operator: %s\n", yytext); }
"=" { printf("it is a assignment operator equal|single equal \n"); }
.|\n{/* Ignore other characters */} % %
		int main()
{
	yylex();
	return 0;
}
