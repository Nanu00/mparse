#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef enum Operator_ {
        SUB = '-',
        ADD = '+',
        MUL = '*',
        DIV = '/',
        POW = '^'
} Operator;

int precedence(Operator o) {
    switch (o) {
        case SUB:
            return 2;
        case ADD:
            return 2;
        case MUL:
            return 3;
        case DIV:
            return 3;
        case POW:
            return 4;
    }
}

float do_op(float a, float b, Operator o) {
    switch (o) {
        case SUB:
            return a-b;
        case ADD:
            return a+b;
        case MUL:
            return a*b;
        case DIV:
            return a/b;
        case POW:
            return powf(a, b);
    }
}

typedef union NodeData_ {
    float num;
    enum {
        PARANTHESES_LEFT = '(',
        PARANTHESES_RIGHT = ')',
        // SQUARE_LEFT = '[',
        // SQUARE_RIGHT = ']',
        // CURLY_LEFT = '{',
        // CURLY_RIGHT = '}'
    } bracket;
    Operator op;
    char var;
} NodeData;

typedef enum DataType_ {
    NUMBER,
    OPERATOR,
    BRACKET,
    VARIABLE,
    // ASSERTIONS // shit like = or >
} DataType;

typedef struct Node_ {
    DataType type;
    NodeData data;
} Node;

typedef struct Leaf_ Leaf;
struct Leaf_ {
    Node node;
    Leaf *n1, *n2;
};

void print_node(Node l) {
    if (l.type == NUMBER) {
        printf("NUMBER(%f)", l.data.num);
    } else if (l.type == OPERATOR) {
        printf("OPERATOR(%c)", l.data.op);
    } else if (l.type == BRACKET) {
        printf("BRACKET(%c)", l.data.bracket);
    } else if (l.type == VARIABLE) {
        printf("VARIABLE(%c)", l.data.var);
    }
}

void print_tree(Leaf* l, int level) {
    for (int i = 0; i < level; i++)
        printf("\t");
    print_node(l->node);
    printf("\n");
    if (l->n1) {
        print_tree(l->n1, level+1);
    }
    if (l->n2) {
        print_tree(l->n2, level+1);
    }
}

void print_node_char(Node l) {
    if (l.type == NUMBER) {
        printf("%f", l.data.num);
    } else if (l.type == OPERATOR) {
        printf("%c", l.data.op);
    } else if (l.type == BRACKET) {
        printf("%c", l.data.bracket);
    } else if (l.type == VARIABLE) {
        printf("%c", l.data.var);
    }
}

void print_node_array(Node* l, size_t size) {
    printf("{ ");
    for (int i = 0; i < size; i++) {
        print_node(l[i]);
        if (size - i > 1)
            printf(", ");
        else
            printf(" ");
    }
    printf("}\n");
}

void print_expr(Node* l, size_t size) {
    for (int i = 0; i < size; i++) {
        print_node_char(l[i]);
        if (size - i > 1)
            printf(" ");
    }
    printf("\n");
}

size_t tokenize(char* input, Node **output) {
    // DONT FORGET TO FREE THE OUTPUT
    size_t output_size = 0;

    char* buffer = malloc(strlen(input)+1);
    strcpy(buffer, input);
    int buf_offset = 0;

    char num[1024] = {0};
    char c = 0;

    Node token = { OPERATOR, 0 };

    int bytes_read;

    int neg = 0;
    while (buf_offset < strlen(buffer)) {
        bytes_read = 0;
        sscanf((buffer+buf_offset), "%[0-9]%n", num, &bytes_read);

        if (bytes_read != 0) {
            if (neg) {
                token = (Node) { NUMBER, 0 - atoi(num) };
                neg = 0;
            }
            else
                token = (Node) { NUMBER, atoi(num) };
        } else {
            if (sscanf((buffer+buf_offset), "%[a-zA-Z]", &c)) {
                token = (Node) { VARIABLE, c };
                token.data.op = c;
            // } else if (sscanf((buffer+buf_offset), "%[]{}()[]", &c)) {
            } else if (sscanf((buffer+buf_offset), "%[()]", &c)) {
                token = (Node) { BRACKET, c };
                token.data.op = c;
            } else if (sscanf((buffer+buf_offset), "%[/+*^]", &c)) {
                token = (Node) { OPERATOR, c };
                token.data.op = c;
            } else if (sscanf((buffer+buf_offset), "%[-]", &c)) {
                if (output_size == 0 || ((*output)[output_size - 1].type != NUMBER && (*output)[output_size - 1].type != VARIABLE)) {
                    neg = 1;
                    buf_offset++;
                    continue;
                } else {
                    token = (Node) { OPERATOR, c };
                    token.data.op = c;
                }
            }
            bytes_read = 1;
        }

        buf_offset+=bytes_read;

        output_size++;

        *output = realloc(*output, output_size*sizeof(**output));
        (*output)[output_size - 1] = token;
    }
    return output_size;
}

Leaf* tokbtree(Node* tokens, size_t len) {
    Node* op_stack = malloc(sizeof(Node)*len);
    int op_stack_size = 0;
    memset(op_stack, 0, len);

    Leaf** out_stack = malloc(sizeof(Node*)*len);
    int out_stack_size = 0;
    memset(out_stack, 0, len);

    for (int i = 0; i < len; i++) {
        if (tokens[i].type == NUMBER || tokens[i].type == VARIABLE) {
            out_stack[out_stack_size] = malloc(sizeof(Leaf));
            *(out_stack[out_stack_size]) = (Leaf) {tokens[i], NULL, NULL};
            out_stack_size++;
        } else if (tokens[i].type == OPERATOR) {
            op_stack_size--;
            while (op_stack_size>=0) {
                if (op_stack[op_stack_size].type == BRACKET) {
                    break;
                } else if (tokens[i].data.op == POW && precedence(tokens[i].data.op) <= precedence(op_stack[op_stack_size].data.op)) {
                    break;
                } else if (precedence(tokens[i].data.op) != precedence(op_stack[op_stack_size].data.op)) {
                    break;
                }

                Leaf* l = malloc(sizeof(Leaf));
                *l = (Leaf) {op_stack[op_stack_size], out_stack[out_stack_size-2], out_stack[out_stack_size-1]};
                out_stack_size-=2;
                out_stack[out_stack_size] = l;
                out_stack_size++;
                op_stack_size--;
            }
            op_stack_size++;
            op_stack[op_stack_size] = tokens[i];
            op_stack_size++;
        } else if (tokens[i].type == BRACKET) {
            if (tokens[i].data.bracket == PARANTHESES_LEFT) {
                op_stack[op_stack_size] = tokens[i];
                op_stack_size++;
            } else {
                op_stack_size--;
                while (op_stack_size>=0) {
                    if (op_stack[op_stack_size].type == BRACKET && op_stack[op_stack_size].data.bracket == PARANTHESES_LEFT) {
                        op_stack_size--;
                        break;
                    } else {
                        Leaf* l = malloc(sizeof(Leaf));
                        *l = (Leaf) {op_stack[op_stack_size], out_stack[out_stack_size-2], out_stack[out_stack_size-1]};
                        out_stack_size-=2;
                        out_stack[out_stack_size] = l;
                        out_stack_size++;
                        op_stack_size--;
                    }
                }
                op_stack_size++;
            }
        }
    }

    op_stack_size--;
    while (op_stack_size>=0) {
        Leaf* l = malloc(sizeof(Leaf));
        *l = (Leaf) {op_stack[op_stack_size], out_stack[out_stack_size-2], out_stack[out_stack_size-1]};
        out_stack_size-=2;
        out_stack[out_stack_size] = l;
        out_stack_size++;
        op_stack_size--;
    }

    free(op_stack);

    Leaf* top = out_stack[0];
    free(out_stack);
    return top;
}

Leaf* eval(Leaf* top) {
    if (top->node.type == NUMBER || top->n1 == NULL || top->n2 == NULL) {
        return top;
    } else {
        if (top->n1->node.type != NUMBER) {
            top->n1 = eval(top->n1);
        }
        if (top->n2->node.type != NUMBER) {
            top->n2 = eval(top->n2);
        }

        if (top->n1->node.type == NUMBER && top->n2->node.type == NUMBER) {
            Leaf *top_new = malloc(sizeof(Leaf));
            *top_new = (Leaf) { (Node) { NUMBER, do_op(top->n1->node.data.num, top->n2->node.data.num, top->node.data.op) } };
            free(top);
            return top_new;
        }
        return top;
    }
}

void print_expr_rpn(Leaf *top) {
    if (top->n1)
        print_expr_rpn(top->n1);
    if (top->n2)
        print_expr_rpn(top->n2);
    print_node_char(top->node);
    printf(" ");
}

int main() {
    size_t output_size;
    Node* output = malloc(0);

    char* test_string = malloc(1024);
    fgets(test_string, 1024, stdin);
    test_string[strcspn(test_string, "\r\n")] = 0;

    output_size = tokenize(test_string, &output);

    print_node_array(output, output_size);
    printf("\n");

    Leaf *tree = tokbtree(output, output_size);
    printf("Tree:\n");
    print_tree(tree, 0);

    printf("\n");
    print_expr_rpn(tree);
    printf("\n");

    printf("\nEval:\n");
    tree = eval(tree);
    print_tree(tree, 0);

    printf("\n");
    print_expr_rpn(tree);
    printf("\n");

    free(output);
    free(test_string);
}
