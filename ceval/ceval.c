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
	Token token;
	struct expNode* left;
	struct expNode* right; 
} expNode;

//Functions declarations
Token* tokenize(char* expression);
void printTokens(Token* tokens);

expNode* createNode(Token token, expNode* left, expNode* right);

expNode* parse_expression(Token** tokens);
expNode* parse_term(Token** tokens);
expNode* parse_factor(Token** tokens);

double evaluate(expNode* root);
void freeTree(expNode* root);

void printTree(expNode* root, int depth) {
    if (root == NULL) return;

    // Print right subtree (top)
    printTree(root->right, depth + 1);

    // Print current node
    for (int i = 0; i < depth; i++) {
        printf("   "); // Indent based on depth
    }
    if (root->token.type == 'n') {
        printf("%d\n", root->token.value); // Print number
    } else {
        printf("%c\n", root->token.type); // Print operator/parenthesis
    }

    // Print left subtree (bottom)
    printTree(root->left, depth + 1);
}

int main() {
    char* expression = "10 + 5 * 9";
    printf("Expression: %s\n", expression);
    Token* tokens = tokenize(expression);

    if (tokens == NULL) {
        return 1;
    }

    //printTokens(tokens);

    Token* current = tokens;
    expNode* root = parse_expression(&current);

    printf("\nExpression Tree:\n");
    printTree(root, 0); // Print the tree

    int result = evaluate(root);
    printf("\nResult: %d\n", result);

    freeTree(root);
    free(tokens);

    return 0;
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
				tokens[tokens_count].value = tokens[tokens_count].value * 10 + (expression[i] - '0'); // - '0' because expression[i] will get the ASCII value of the number. 
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
			break;
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
		
expNode* createNode(Token token, expNode* left, expNode* right)
{
	expNode* node = (expNode*)malloc(sizeof(expNode));
	node->token = token;
	node->left = left;
	node->right = right;
	return node;
}

expNode* parse_expression(Token** tokens)
{
	expNode* node = parse_term(tokens);

	while((*tokens)->type == '+' || (*tokens)->type == '-')
	{
		Token op = **tokens;
		(*tokens)++;
		expNode* right = parse_term(tokens);
		node = createNode(op, node, right);
	}

	return node;
}

expNode* parse_term(Token** tokens)
{
	expNode* node = parse_factor(tokens);

	while((*tokens)->type == '*' || (*tokens)->type == '/')
	{
		Token op = **tokens;
		(*tokens)++;
		expNode* right = parse_factor(tokens);
		node = createNode(op, node, right);
	}

	return node;
}

expNode* parse_factor(Token** tokens)
{
	if((*tokens)->type == 'n')
	{
		expNode* node = createNode(**tokens, NULL, NULL);
		(*tokens)++;
		return node;
	} else if((*tokens)->type == '(')
	{
		(*tokens)++;
		expNode* node = parse_expression(tokens);
		if((*tokens)->type != ')')
		{
			fprintf(stderr, "Error: Missing closing paranthesis ')'\n");
			exit(1);
		}
		(*tokens)++;
		return node;
	}
}

double evaluate(expNode* root)
{
	if(root->token.type == 'n')
	{
		return root->token.value;
	}

	int left = evaluate(root->left);
	int right = evaluate(root->right);

	switch(root->token.type) {
		case '+':
			return left + right;
		case '-': 
			return left - right;
		case '*':
			return left * right;
		case '/':
			return left / right;
		default:
			fprintf(stderr, "Error: Invalid operator.\n");
			exit(1);
	}	
}

void freeTree(expNode* root) {
    if (root == NULL) return;
    freeTree(root->left); // Free left subtree
    freeTree(root->right); // Free right subtree
    free(root); // Free current node
}
