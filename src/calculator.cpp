#include "calculator.hpp"

#include <math.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

bool is_digit(char sym)
{
    return ZERO <= sym && sym <= NINE;
}

int char_to_digit(char c)
{
    return int(c) - 48;
}

bool is_operation(char sym)
{
    return sym == PLUS ||
           sym == MINUS ||
           sym == MOD ||
           sym == MULT ||
           sym == DIVIDE ||
           sym == POW;
}

bool is_left_bracket(char c)
{
    return c == BRACKET_L;
}

bool is_right_bracket(char c)
{
    return c == BRACKET_R;
}

bool is_bracket(char sym)
{
    return is_left_bracket(sym) or
           is_right_bracket(sym);
}

bool is_delim(char c)
{
    return c == ' ';
}

bool is_equal_sign(char sym)
{
    return sym == EQUALS;
}

bool is_end_of_string(char sym)
{
    return sym == '\n';
}

bool is_backspace(char sym)
{
    return sym == BACKSCAPE;
}

bool is_arrow(char curr_sym, char next_sym)
{
    return int(curr_sym) == 27 and next_sym == 91;
}

ARROW which_arrow(char third_sym)
{
    switch (third_sym)
    {
    case ARROW_UP:
        return UP;
    case ARROW_DOWN:
        return DOWN;
    case ARROW_LEFT:
        return LEFT;
    case ARROW_RIGHT:
        return RIGHT;
    }

    return MY_UNKNOWN;
}

int operation_priority(char op)
{

    if (op == PLUS or op == MINUS)
    {
        return 1;
    }
    if (op == MULT or op == DIVIDE or op == MOD)
    {
        return 2;
    }
    if (op == POW)
    {
        return 3;
    }
    return -1;
}

void process_operation(vector<double> &num_stack, char operation)
{
    if (num_stack.size() == 1)
    {
        double l = num_stack.back();
        num_stack.pop_back();
        switch (operation)
        {

        case PLUS:
            num_stack.push_back(l);
            break;

        case MINUS:
            num_stack.push_back(-l);
            break;

        default:
            num_stack.push_back(l);
        }
    }
    else if (num_stack.size() >= 2)
    {

        double rhs = num_stack.back();
        num_stack.pop_back();
        double lhs = num_stack.back();
        num_stack.pop_back();

        switch (operation)
        {

        case PLUS:
            num_stack.push_back(lhs + rhs);
            break;

        case MINUS:
            num_stack.push_back(lhs - rhs);
            break;

        case MULT:
            num_stack.push_back(lhs * rhs);
            break;

        case DIVIDE:
            num_stack.push_back(lhs / rhs);
            break;

        case MOD:
            num_stack.push_back(int(lhs) % int(rhs));
            break;

        case POW:
            num_stack.push_back(pow(lhs, rhs));
            break;

        default:
            // throw range_error("Unknown operation!");
            num_stack.push_back(lhs);
            num_stack.push_back(rhs);
        }
    }
}

float calculate(const char *equation, int start, int end)
{
    int i = start;
    char curr = equation[i];
    char curr_op;
    bool may_unary = true;

    vector<double> num_stack;
    vector<char> op_stack;

    while (i < end)
    {
        if (!is_delim(curr))
        {
            if (is_left_bracket(curr))
            {
                op_stack.push_back(curr);
                may_unary = true;
            }
            else if (is_right_bracket(curr))
            {
                while (!is_left_bracket(curr_op))
                {

                    curr_op = op_stack.back();
                    op_stack.pop_back();

                    if (is_left_bracket(curr_op))
                    {
                        break;
                    }

                    process_operation(num_stack, curr_op);
                }
            }
            else if (is_operation(curr))
            {
                curr_op = curr;
                int curr_op_priority = operation_priority(curr_op);

                while (!op_stack.empty() and
                       operation_priority(op_stack.back()) >= curr_op_priority)
                {
                    process_operation(num_stack, op_stack.back());
                    op_stack.pop_back();
                }
                op_stack.push_back(curr_op);
            }
            else if (is_digit(curr))
            {
                string operand;
                bool dot_was = false;

                while (i < end)
                {
                    if (is_digit(curr))
                    {
                        operand += curr;
                    }
                    else if (curr == '.')
                    {
                        if (!dot_was)
                        {
                            operand += curr;
                        }
                        dot_was = true;
                    }
                    else
                    {
                        break;
                    }

                    curr = equation[++i];
                }
                --i;

                num_stack.push_back(atof(operand.c_str()));
            }
            else
            {
                break;
            }
        }

        curr = equation[++i];
    }

    while (!op_stack.empty())
    {
        process_operation(num_stack, op_stack.back());
        op_stack.pop_back();
    }

    return num_stack.back();
}
