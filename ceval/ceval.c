#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_SIZE 100


typedef struct Token{
	char type; //n for number and the respective sign if it's an operation or a paranthesis 
	int value; //only used when the type is n
} Token;

typedef struct expNode{
	char type;
	int value;
	struct expNode* left;
	struct expNode* right; 
} expNode;

//Functions declarations
Token* tokenize(char* expression);
void printTokens(Token* tokens);

int main()
{
	char* expression = "10 + 5 * 9";
	Token* tokens = tokenize(expression);
	printTokens(tokens);
}

Token* tokenize(char* expression)
{
	Token* tokens = malloc(MAX_SIZE * sizeof(Token));
	int len = strlen(expression);
	int tokens_count = 0;

	for(int i = 0; i < len; i++)
	{	
		if(isspace(expression[i]))
		{
			continue;
		} else if (isdigit(expression[i]))
		{
			tokens[tokens_count].type = 'n';
			tokens[tokens_count].value = 0;
			while(i < len && isdigit(expression[i]))
			{
				tokens[tokens_count].value = tokens[tokens_count].value * 10 + (expression[i] - '0');
				i++;
			}
			i--;
			tokens_count++;
			continue;
		} else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/' || expression[i] == '(' || expression[i] == ')')
		{
			tokens[tokens_count].type = expression[i];
			tokens[tokens_count].value = 0;
			tokens_count++;
			continue;
		} else {
			fprintf(stderr, "Erorr: Invalid caracter '%c'.\n", expression[i]);
		}
	}

	tokens[tokens_count].type = '\0';
	return tokens;
}

void printTokens(Token* tokens)
{
	for(int i = 0; tokens[i].type != '\0'; i++)
	{
		if(tokens[i].type == 'n')
		{
			printf("Number: %d\n", tokens[i].value);
		} else {
			printf("Paranthesis/Operator: %c\n", tokens[i].type);
		}
	}
}
		
