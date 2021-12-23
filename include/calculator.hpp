#pragma once

#include <Arduino.h>

#include <math.h>
#include <vector>
#include <string>


#define ZERO '0'
#define NINE '9'

#define PLUS      '+'
#define MINUS     '-'
#define MOD       '%'
#define MULT      '*'
#define DIVIDE    '/'
#define POW       '^'
#define EQUALS    '='
#define BRACKET_L '('
#define BRACKET_R ')'

#define BACKSCAPE 8

#define ARROW_UP    'A'
#define ARROW_DOWN  'B'
#define ARROW_LEFT  'D'
#define ARROW_RIGHT 'C'

enum ARROW { UP, DOWN, LEFT, RIGHT, MY_UNKNOWN};

bool is_digit(char sym);

int char_to_digit(char c);

bool is_operation(char sym);

bool is_left_bracket(char c);

bool is_right_bracket(char c);

bool is_bracket(char sym);

bool is_delim(char c);

bool is_equal_sign(char sym);

bool is_end_of_string(char sym);

bool is_backspace(char sym);

bool is_arrow(char curr_sym, char next_sym);

ARROW which_arrow(char third_sym);

int operation_priority(char op);

void process_operation(std::vector<double>& num_stack, char operation);

float calculate(const char *equation, int start, int end);


