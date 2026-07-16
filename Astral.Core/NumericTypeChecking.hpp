#pragma once
#include "string.hpp"

enum NumericType
{
    NumericType_None,
    NumericType_Integer,
    NumericType_UInteger,
    NumericType_Float
};

inline bool CharIsPossibleNumericMember(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || c == 'x' || c == 'X' || c == '.' || c == '+' || c == '-';
}
inline NumericType CheckStringNumericType(CharSlice input)
{
    NumericType result = NumericType_None;
    bool expectingExponent = false;
    bool exponentExpectSignNotation = false;
    bool ifExponentExpectDigitFirst = false;
    bool startedWithZero = false;
    bool expectingHex = false;
    bool expectingBin = false;
    bool encounteredPeriod = false;
    for (u32 i = 0; i < input.length; i++)
    {
        if (expectingHex)
        {
            if (input.buffer[i] < '0' || input.buffer[i] > '9' || ((input.buffer[i] < 'a' || input.buffer[i] > 'f') && (input.buffer[i] < 'A' || input.buffer[i] > 'F')))
            {
                //not a valid hexadecimal literal
                return NumericType_None;
            }
        }
        else if (expectingBin)
        {
            if (input.buffer[i] != '1' && input.buffer[i] != '0')
            {
                //not a valid binary literal
                return NumericType_None;
            }
        }
        else if (expectingExponent)
        {
            bool foundNotation = false;
            if (exponentExpectSignNotation)
            {
                if (input.buffer[i] == '+')
                {
                    exponentExpectSignNotation = false;
                    foundNotation = true;
                }
                //0e-2 refers to 0.01, which must be a float
                else if (input.buffer[i] == '-')
                {
                    exponentExpectSignNotation = false;
                    foundNotation = true;
                    result = NumericType_Float;
                }
                else
                {
                    exponentExpectSignNotation = false;
                }
            }
            if (!exponentExpectSignNotation && !foundNotation)
            {
                //1e+01 is valid, so no need to check prefix zeroes, only that everything
                //from thereon is alphanumeric
                if (input.buffer[i] < '0' || input.buffer[i] > '9')
                {
                    return NumericType_None;
                }
            }
        }
        else
        {
            if (startedWithZero)
            {
                if (input.buffer[i] == 'b' || input.buffer[i] == 'B')
                {
                    //string starts with 0b..., is a binary literal
                    expectingBin = true;
                    startedWithZero = false;
                    result = NumericType_UInteger;
                    continue;
                }
                else if (input.buffer[i] == 'x' || input.buffer[i] == 'X')
                {
                    //string starts with 0x... is a hex literal
                    expectingHex = true;
                    startedWithZero = false;
                    result = NumericType_UInteger;
                    continue;
                }
                else if (input.buffer[i] == '.')
                {
                    startedWithZero = false;
                }
                else
                {
                    //expected float, hex or binary (ie: string starting with 0) but got some other character
                    //instead
                    return NumericType_None;
                }
            }
            
            if (input.buffer[i] == '-')
            {
                //- is only valid at the start
                if (result == NumericType_None)
                {
                    result = NumericType_Integer;
                }
                else
                {
                    return NumericType_None;
                }
            }
            else if (input.buffer[i] >= '0' && input.buffer[i] <= '9')
            {
                ifExponentExpectDigitFirst = false;

                if (input.buffer[i] == '0')
                {
                    //00 is invalid
                    if (startedWithZero)
                    {
                        return NumericType_None;
                    }
                    //-0 is always a float
                    else if (i == 1 && result == NumericType_Integer)
                    {
                        result = NumericType_Float;
                    }
                    else if (i == 0)
                    {
                        //cannot start integers with 0 unless is binary or hex literal
                        startedWithZero = true;
                    }
                }
                if (result == NumericType_None)
                {
                    result = NumericType_UInteger;
                }
            }
            else if (input.buffer[i] == 'e' || input.buffer[i] == 'E')
            {
                if (result != NumericType_None)
                {
                    if (ifExponentExpectDigitFirst)
                    {
                        return NumericType_None;
                    }
                    exponentExpectSignNotation = true;
                    expectingExponent = true;
                }
                else
                {
                    //e+... is invalid
                    return NumericType_None;
                }
            }
            else if (input.buffer[i] == '.')
            {
                //-0.0 or 1.0 is valid
                result = NumericType_Float;
                //0.e is not valid, but 0. will just equate to 0
                if (i < input.length - 1)
                {
                    ifExponentExpectDigitFirst = true;
                }
                else
                {
                    ifExponentExpectDigitFirst = false;
                }
                if (encounteredPeriod)
                {
                    //0..0 is invalid
                    return NumericType_None;
                }
                encounteredPeriod = true;
            }
            else return NumericType_None;
        }
    }

    if ((startedWithZero && input.length > 1) || ifExponentExpectDigitFirst)
    {
        //was not resolved
        return NumericType_None;
    }
    return result;
}

/*Test

text isInt = "-100";
text isUInt = "123";
text isFloat1 = "-0";
text isFloat2 = "-4.371139E-08";
text isFloat3 = "0.0";
text isUInt2 = "1e+10";
text isInt2 = "-123e+2";
text isFloat4 = "1e-10";

text isUInt3 = "0b11111111";
text err1 = "Hello World!";
text err2 = "0e0";
text err3 = "00xFFF";
text err4 = "-0xfFf1";

printf("%s\n", CheckStringNumericType(isInt) == NumericType_Integer ? "true": "false");
printf("%s\n", CheckStringNumericType(isUInt) == NumericType_UInteger ? "true": "false");
printf("%s\n", CheckStringNumericType(isFloat1) == NumericType_Float ? "true": "false");
printf("%s\n", CheckStringNumericType(isFloat2) == NumericType_Float ? "true": "false");
printf("%s\n", CheckStringNumericType(isFloat3) == NumericType_Float ? "true": "false");

printf("%s\n", CheckStringNumericType(isUInt2) == NumericType_UInteger ? "true": "false");
printf("%s\n", CheckStringNumericType(isUInt3) == NumericType_UInteger ? "true": "false");

printf("%s\n", CheckStringNumericType(isInt2) == NumericType_Integer ? "true": "false");
printf("%s\n", CheckStringNumericType(isFloat4) == NumericType_Float ? "true": "false");

printf("%s\n", CheckStringNumericType(err1) == NumericType_None ? "true": "false");
printf("%s\n", CheckStringNumericType(err2) == NumericType_None ? "true": "false");
printf("%s\n", CheckStringNumericType(err3) == NumericType_None ? "true": "false");
printf("%s\n", CheckStringNumericType(err4) == NumericType_None ? "true": "false");

*/