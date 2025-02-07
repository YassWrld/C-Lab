#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_SIZE 100
#define EPSILON 1e-9 // For floating-point comparisons

typedef struct Token
{
    char type;   // 'n' for number, operator, or parenthesis
    double value; 
} Token;

typedef struct expNode
{
    Token token;
    struct expNode* left;
    struct expNode* right;
} expNode;

// Function prototypes
Token* tokenize(char* expression);
void printTokens(Token* tokens);
expNode* createNode(Token token, expNode* left, expNode* right);
expNode* parse_expression(Token** tokens);
expNode* parse_term(Token** tokens);
expNode* parse_factor(Token** tokens);
double evaluate(expNode* root);
void freeTree(expNode* root);
void printTree(expNode* root, int depth);

int main()
{
    char expression[] = "-10.5 + (3.14 * -2) / 0.5";
    printf("Expression: %s\n", expression);
    Token* tokens = tokenize(expression);

    if (!tokens)
    {
        return 1;
    }

    Token* current = tokens;
    expNode* root = parse_expression(&current);

    printf("\nExpression Tree:\n");
    printTree(root, 0);

    printf("\nEvaluating...\n");
    double result = evaluate(root);
    printf("Result: %.2f\n", result);

    freeTree(root);
    free(tokens);
    return 0;
}

expNode* createNode(Token token, expNode* left, expNode* right)
{
    expNode* node = (expNode*)malloc(sizeof(expNode));
    node->token = token;
    node->left = left;
    node->right = right;
    return node;
}

Token* tokenize(char* expression)
{
    Token* tokens = malloc(MAX_SIZE * sizeof(Token));
    int tokens_count = 0;
    int len = strlen(expression);

    for (int i = 0; i < len && tokens_count < MAX_SIZE - 1; i++)
    {
        if (isspace(expression[i]))
        {
            continue;
        }

        if (isdigit(expression[i]) || expression[i] == '.' || 
            (expression[i] == '-' && (i == 0 || !isdigit(expression[i-1]))))
        {
            int is_negative = 0;
            if (expression[i] == '-')
            {
                is_negative = 1;
                i++;
            }

            double value = 0;
            double fraction = 1.0;
            int decimal_found = 0;

            while (i < len && (isdigit(expression[i]) || expression[i] == '.'))
            {
                if (expression[i] == '.')
                {
                    if (decimal_found)
                    {
                        fprintf(stderr, "Error: Multiple decimals in number\n");
                        free(tokens);
                        return NULL;
                    }
                    decimal_found = 1;
                    i++;
                    continue;
                }

                if (!decimal_found)
                {
                    value = value * 10 + (expression[i] - '0');
                }
                else
                {
                    fraction *= 0.1;
                    value += (expression[i] - '0') * fraction;
                }
                i++;
            }
            i--;

            tokens[tokens_count].type = 'n';
            tokens[tokens_count].value = is_negative ? -value : value;
            tokens_count++;
        }
        else if (strchr("+-*/()", expression[i]))
        {
            tokens[tokens_count].type = expression[i];
            tokens[tokens_count].value = 0;
            tokens_count++;
        }
        else
        {
            fprintf(stderr, "Error: Invalid character '%c'\n", expression[i]);
            free(tokens);
            return NULL;
        }
    }

    tokens[tokens_count].type = '\0';
    return tokens;
}

expNode* parse_factor(Token** tokens)
{
    if ((*tokens)->type == '(')
    {
        (*tokens)++;
        expNode* node = parse_expression(tokens);
        if ((*tokens)->type != ')')
        {
            fprintf(stderr, "Error: Missing closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
        (*tokens)++;
        return node;
    }
    
    if ((*tokens)->type == '-')
    {
        (*tokens)++;
        expNode* operand = parse_factor(tokens);
        return createNode((Token){'-', 0}, createNode((Token){'n', 0}, NULL, NULL), operand);
    }
    
    if ((*tokens)->type == 'n')
    {
        expNode* node = createNode(**tokens, NULL, NULL);
        (*tokens)++;
        return node;
    }

    fprintf(stderr, "Error: Unexpected token '%c'\n", (*tokens)->type);
    exit(EXIT_FAILURE);
}

expNode* parse_term(Token** tokens)
{
    expNode* node = parse_factor(tokens);
    while ((*tokens)->type == '*' || (*tokens)->type == '/')
    {
        Token op = **tokens;
        (*tokens)++;
        node = createNode(op, node, parse_factor(tokens));
    }
    return node;
}

expNode* parse_expression(Token** tokens)
{
    expNode* node = parse_term(tokens);
    while ((*tokens)->type == '+' || (*tokens)->type == '-')
    {
        Token op = **tokens;
        (*tokens)++;
        node = createNode(op, node, parse_term(tokens));
    }
    return node;
}

double evaluate(expNode* root)
{
    if (root->token.type == 'n')
    {
        return root->token.value;
    }
    
    double left = evaluate(root->left);
    double right = evaluate(root->right);
    
    switch (root->token.type)
    {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': 
            if (fabs(right) < EPSILON)
            {
                fprintf(stderr, "Error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
            return left / right;
        default:
            fprintf(stderr, "Error: Unknown operator '%c'\n", root->token.type);
            exit(EXIT_FAILURE);
    }
}

void printTokens(Token* tokens)
{
    for (int i = 0; tokens[i].type != '\0'; i++)
    {
        if (tokens[i].type == 'n')
        {
            printf("Number: %10.2f\n", tokens[i].value);
        }
        else
        {
            printf("Operator/Paren: %8c\n", tokens[i].type);
        }
    }
}

void printTree(expNode* root, int depth)
{
    if (!root)
    {
        return;
    }
    printTree(root->right, depth + 1);
    printf("%*s", depth * 4, "");
    if (root->token.type == 'n')
    {
        printf("%.2f\n", root->token.value);
    }
    else
    {
        printf("%c\n", root->token.type);
    }
    printTree(root->left, depth + 1);
}

void freeTree(expNode* root)
{
    if (!root)
    {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
