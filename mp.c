#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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

typedef struct Number_ {
    float value;
    char var;
    float var_pow;
} Number;

typedef union NodeData_ {
    Number num;
    enum {
        PARANTHESES_LEFT = '(',
        PARANTHESES_RIGHT = ')',
        // SQUARE_LEFT = '[',
        // SQUARE_RIGHT = ']',
        // CURLY_LEFT = '{',
        // CURLY_RIGHT = '}'
    } bracket;
    Operator op;
} NodeData;

typedef enum DataType_ {
    NUMBER,
    OPERATOR,
    BRACKET,
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
        printf("NUMBER(%f%c)", l.data.num.value, l.data.num.var);
    } else if (l.type == OPERATOR) {
        printf("OPERATOR(%c)", l.data.op);
    } else if (l.type == BRACKET) {
        printf("BRACKET(%c)", l.data.bracket);
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
        if (!(l.data.num.var && l.data.num.value == 1))
            printf("%f", l.data.num.value);
        if (l.data.num.var)
            printf("%c", l.data.num.var);
        if (l.data.num.var && l.data.num.var_pow != 1)
            printf("^%f", l.data.num.var_pow);
    } else if (l.type == OPERATOR) {
        printf("%c", l.data.op);
    } else if (l.type == BRACKET) {
        printf("%c", l.data.bracket);
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

Leaf* do_op(Number n1, Number n2, Operator o) {
    Leaf* l = calloc(sizeof(Leaf), 1);
    if (n1.var == n2.var) {
        switch (o) {
            case ADD:
                if (n1.var && n1.var_pow != n2.var_pow) {
                    l->node.type = OPERATOR;
                    l->node.data.op = ADD;

                    l->n1 = calloc(sizeof(Leaf), 1);
                    l->n2 = calloc(sizeof(Leaf), 1);

                    l->n1->node.type = NUMBER;
                    l->n2->node.type = NUMBER;

                    l->n1->node.data = (NodeData) { n1 };
                    l->n2->node.data = (NodeData) { n2 };
                } else {
                    l->node.type = NUMBER;
                    l->node.data = (NodeData) { (Number) { n1.value + n2.value, n1.var, n1.var_pow } };
                }
                break;
            case SUB:
                if (n1.var && n1.var_pow != n2.var_pow) {
                    l->node.type = OPERATOR;
                    l->node.data.op = SUB;

                    l->n1 = calloc(sizeof(Leaf), 1);
                    l->n2 = calloc(sizeof(Leaf), 1);

                    l->n1->node.type = NUMBER;
                    l->n2->node.type = NUMBER;

                    l->n1->node.data = (NodeData) { n1 };
                    l->n2->node.data = (NodeData) { n2 };
                } else {
                    l->node.type = NUMBER;
                    l->node.data = (NodeData) { (Number) { n1.value - n2.value, n1.var, n1.var_pow } };
                }
                break;
            case MUL:
                l->node.type = NUMBER;
                l->node.data = (NodeData) { (Number) { n1.value * n2.value, n1.var, n1.var_pow + n2.var_pow } };
                break;
            case DIV:
                l->node.type = NUMBER;
                l->node.data = (NodeData) { (Number) { n1.value / n2.value, n1.var, n1.var_pow - n2.var_pow } };
                break;
            case POW:
                l->node.type = OPERATOR;
                l->node.data.op = POW;

                l->n1 = calloc(sizeof(Leaf), 1);
                l->n2 = calloc(sizeof(Leaf), 1);

                l->n1->node.type = NUMBER;
                l->n2->node.type = NUMBER;

                l->n1->node.data = (NodeData) { n1 };
                l->n2->node.data = (NodeData) { n2 };
                break;
        }
    } else {
        switch (o) {
            case ADD:
                l->node.type = OPERATOR;
                l->node.data.op  = ADD ;

                l->n1 = calloc(sizeof(Leaf), 1);
                l->n2 = calloc(sizeof(Leaf), 1);

                l->n1->node.type = NUMBER;
                l->n2->node.type = NUMBER;

                l->n1->node.data = (NodeData) { n1 };
                l->n2->node.data = (NodeData) { n2 };
                break;
            case SUB:
                l->node.type = OPERATOR;
                l->node.data.op = SUB ;

                l->n1 = calloc(sizeof(Leaf), 1);
                l->n2 = calloc(sizeof(Leaf), 1);

                l->n1->node.type = NUMBER;
                l->n2->node.type = NUMBER;

                l->n1->node.data = (NodeData) { n1 };
                l->n2->node.data = (NodeData) { n2 };
                break;
            case MUL:
                if (n1.var == 0) {
                    l->node.type = NUMBER;
                    l->node.data = (NodeData) { (Number) { n1.value * n2.value, n2.var, n2.var_pow } };
                } else if (n2.var == 0) {
                    l->node.type = NUMBER;
                    l->node.data = (NodeData) { (Number) { n1.value * n2.value, n1.var, n1.var_pow } };
                } else {
                    l->node.type = OPERATOR;
                    l->node.data.op = MUL;

                    l->n1 = calloc(sizeof(Leaf), 1);
                    l->n2 = calloc(sizeof(Leaf), 1);

                    l->n1->node.type = NUMBER;
                    l->n2->node.type = NUMBER;

                    l->n1->node.data = (NodeData) { n1 };
                    l->n2->node.data = (NodeData) { n2 };
                }
                break;
            case DIV:
                if (n1.var == 0) {
                    l->node.type = NUMBER;
                    l->node.data = (NodeData) { (Number) { n1.value / n2.value, n2.var, -n2.var_pow } };
                } else if (n2.var == 0) {
                    l->node.type = NUMBER;
                    l->node.data = (NodeData) { (Number) { n1.value / n2.value, n1.var, n1.var_pow } };
                } else {
                    l->node.type = OPERATOR;
                    l->node.data.op = DIV;

                    l->n1 = calloc(sizeof(Leaf), 1);
                    l->n2 = calloc(sizeof(Leaf), 1);

                    l->n1->node.type = NUMBER;
                    l->n2->node.type = NUMBER;

                    l->n1->node.data = (NodeData) { n1 };
                    l->n2->node.data = (NodeData) { n2 };
                }
                if (l->node.type == NUMBER && l->node.data.num.var && (l->node.data.num.var_pow == 0 || l->node.data.num.value == 0))
                        l->node.data.num.var = 0;
                break;
            case POW:
                if (n1.var && !n2.var) {
                    l->node.type = NUMBER;
                    l->node.data.num = n1;
                    l->node.data.num.var_pow*=n2.value;
                } else {
                    l->node.type = OPERATOR;
                    l->node.data.op = POW;

                    l->n1 = calloc(sizeof(Leaf), 1);
                    l->n2 = calloc(sizeof(Leaf), 1);

                    l->n1->node.type = NUMBER;
                    l->n2->node.type = NUMBER;

                    l->n1->node.data = (NodeData) { n1 };
                    l->n2->node.data = (NodeData) { n2 };
                }
                break;
        }
    }
    if (l->node.type == NUMBER && l->node.data.num.var && (l->node.data.num.var_pow == 0 || l->node.data.num.value == 0))
        l->node.data.num.var = 0;
    return l;
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

        if (top->node.type == OPERATOR && top->n1->node.type == NUMBER && top->n2->node.type == NUMBER) {
            Leaf *top_new = malloc(sizeof(Leaf));
            top_new = do_op(top->n1->node.data.num, top->n2->node.data.num, top->node.data.op);
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
    int func = 0;
    while (buf_offset < strlen(buffer)) {
        bytes_read = 0;
        sscanf((buffer+buf_offset), "%[0-9]%n", num, &bytes_read);

        if (bytes_read != 0) {
            if (neg) {
                token = (Node) { NUMBER, (Number) { -atoi(num), 0, 0.0 } };
                neg = 0;
            }
            else
                token = (Node) { NUMBER, (Number) { atoi(num), 0, 0.0 } };
        } else {
            if (sscanf((buffer+buf_offset), "%[a-zA-Z]", &c)) {
                token = (Node) { NUMBER, (Number) { 1.0, c, 1.0 } };
            // } else if (sscanf((buffer+buf_offset), "%[]{}()[]", &c)) {
            } else if (sscanf((buffer+buf_offset), "%[()]", &c)) {
                token = (Node) { BRACKET, c };
                token.data.op = c;
            // } else if (sscanf((buffer+buf_offset), "%[(]", &c)) {
            //     if ((*output)[output_size - 1].type == VARIABLE) {
            //         
            //     }
            } else if (sscanf((buffer+buf_offset), "%[/+*^]", &c)) {
                token = (Node) { OPERATOR, c };
                token.data.op = c;
            } else if (sscanf((buffer+buf_offset), "%[-]", &c)) {
                if (output_size == 0 || ((*output)[output_size - 1].type != NUMBER)) {
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
        if (tokens[i].type == NUMBER) {
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
