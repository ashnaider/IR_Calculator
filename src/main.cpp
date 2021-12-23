#include <Arduino.h>
#include <ArduinoSTL.h>
#include <IRremote.h>

#include "calculator.hpp"


#define IR_RECEIVE_PIN 2
#define MAX_TIME 150
long lastPressTime = 0;

#define BUFF 60

enum ExprSide
{
    Lhs,
    Rhs
};

ExprSide currExprSide = Lhs;

// int in_val;
char chr, next_chr;
double lhs, rhs, res;
bool expr_accumulated = false;
bool result_pinted = false;
short i = 0, expr_len = 0;
short equal_sign_pos = -1;
char expression[BUFF];

bool in_functional_mode = false;

#define EPS .005

bool within_eps(float a, float b)
{
    return a - EPS < b && b < a + EPS;
}

void setup()
{
    Serial.begin(9600);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// 1 = 0xBA45FF00
// 2 = 0xB946FF00
// 3 = 0xB847FF00
// 4 = 0xBB44FF00
// 5 = 0xBF40FF00
// 6 = 0xBC43FF00
// 7 = 0xF807FF00
// 8 = 0xEA15FF00
// 9 = 0xF609FF00
// * = 0xE916FF00
// 0 = 0xE619FF00
// # = 0xF20DFF00
// UP = 0xE718FF00
// LEFT = 0xF708FF00
// OK = 0xE31CFF00
// RIGHT = 0xA55AFF00
// DOWN = 0xAD52FF00

#define LEFT 0xF708FF00
#define RIGHT 0xA55AFF00
#define FN 0xE916FF00

char decode_char(uint32_t packet)
{
    if (packet == LEFT || packet == RIGHT)
    {
        if (in_functional_mode)
        {
            if (packet == LEFT) {
                return '(';
            } 
            else if (packet == RIGHT)
            {
                return ')';
            }
        }
        else
        {
            if (packet == LEFT) {
                return '*';
            } 
            else if (packet == RIGHT)
            {
                return '/';
            }
        }
    }

    switch (packet)
    {
    case 0xBA45FF00:
        return '1';

    case 0xB946FF00:
        return '2';

    case 0xB847FF00:
        return '3';

    case 0xBB44FF00:
        return '4';

    case 0xBF40FF00:
        return '5';

    case 0xBC43FF00:
        return '6';

    case 0xF807FF00:
        return '7';

    case 0xEA15FF00:
        return '8';

    case 0xF609FF00:
        return '9';

    case 0xE619FF00:
        return '0';

    case 0xF20DFF00: // # == BACKSCAPE
        return 8;

    case 0xE31CFF00: // OK == Enter
        return '\n';

    case 0xE916FF00: // * == FUNCTIONAL
        in_functional_mode = !in_functional_mode;
        return 'f';

    case 0xE718FF00: // UP == +
        return '+';

    case 0xAD52FF00: // DOWN == -
        return '-';
    
    default:
        return '?';
    }

    return '?';
}

bool accumulate_expression()
{
    if (IrReceiver.decode())
    {
        if (millis() - lastPressTime > MAX_TIME)
        {

            IRData data = IrReceiver.decodedIRData;
            uint32_t raw_data = data.decodedRawData;

            chr = decode_char(raw_data);

            lastPressTime = millis();
        }
        else
        {
            return false;
        }

        IrReceiver.resume(); // Continue receiving

        if (is_digit(chr) || is_operation(chr) ||
            is_bracket(chr))
        {
            expression[i++] = chr;
            Serial.print(chr);
        }
        else if (is_equal_sign(chr))
        {
            if (currExprSide == Lhs)
            {
                currExprSide = Rhs;
                equal_sign_pos = i;
                Serial.print(chr);
                expression[i++] = chr;
            }
        }
        else if (is_end_of_string(chr))
        {
            expression[i++] = chr;
            expr_len = i + 1;
            i = 0;

            return true;
        }
        else if (is_backspace(chr))
        {

            const char *carr_ret = "\r";
            Serial.print(carr_ret);

            for (short j = 0; j < i; ++j)
            {
                Serial.print(" ");
            }

            Serial.print(carr_ret);

            for (short j = 0; j < i - 1; ++j)
            {
                Serial.print(expression[j]);
            }

            --i;
        }
    }

    return false;
}

void reset_flags()
{
    currExprSide = Lhs;
    expr_accumulated = false;
    equal_sign_pos = -1;
    expr_len = 0;
}

void loop()
{

    if (!expr_accumulated)
    {
        expr_accumulated = accumulate_expression();
    }
    else
    {
        if (equal_sign_pos != -1)
        {
            Serial.println();

            lhs = calculate(expression, 0, equal_sign_pos);

            Serial.print("Lhs: ");
            Serial.println(lhs, 3);

            rhs = calculate(expression, equal_sign_pos + 1, expr_len);

            Serial.print("Rhs: ");
            Serial.println(rhs, 3);

            if (within_eps(lhs, rhs))
            {
                Serial.print("Expressions are equal with eps=");
                Serial.println(EPS, 3);
            }
            else
            {
                Serial.print("Expressions are NOT equal with eps=");
                Serial.println(EPS, 3);
            }
        }
        else
        {
            res = calculate(expression, 0, expr_len);

            Serial.print("\nResult: ");
            Serial.println(res, 3);
        }

        reset_flags();
    }
}
