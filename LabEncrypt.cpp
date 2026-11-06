#include "LabPasscode.h"
#include "LabEEPROM.h"

// The passcode class is an array of characters. Keypresses are stored in a
// passcode object as ascii text.
// We take that ascii text, obfuscate it through a one-way algorithim, put
// that result into a new Passcode class that's stored on the EEPROM.

// The keypresses are not passed to this function until the entire key
// sequnce has been entered by the user, "#" has been pressed to submit
// the code

// After each character is calculated, the offset changes. The same
// digit will encode to something different at different positions in 
// the passcode

// The algorithim encodes the passcode, based on values in
// the passcode itself. The passcode in encoded using the passcode
// as the encoded key.


static int Offset(const Passcode& keypadInput) { //Find a starting offset
    if (pass.size() > 2) {
        //make sure the passcode is long enough, this is enforced elsewhere and double-checked here
        int mod = pass[2] - '0'; //take the second digit of the character array, convert from ascii to int
        if (mod > 0 && mod <= 9) { //if the second digit is 0, just return 0
            return (pass.size() + (pass[1] - '0')) % 36;
            // calculate a starting offset
            // take the length of the passcode, add the 2nd digit the user entered
            // return that value % 36
            // "psuedorandom" value between 0-35
        }
    }

    return 0;
}

static char EncodeChar(int value) { //take a value from 0-36, return 0-9 or A-Z
    if (value < 10) { // 0 - 9 encodes to 0 - 9, this is NOT the same digit the user input
        return '0' + value;
    }

    return 'A' + (value - 10); //10 - 35 encode to A - Z
}

// encryptPass is the main function, takes a passcode string of 0-9 ascii
// encode to a base 36 string of the same length.

// Cannot be reversed without knowing the algorithim, and each character
// output depends on the digits and output characters of the character
// before it. You need to know part of the passcode already, to start
// reversing the passcode.

// To reverse the first character, you need to know the original value
// of second character

void encryptPass(const Passcode& pass, Passcode& encrypted) {
    int offset = Offset(pass); // generate initial offset

    for (int i = 0; i < pass.size(); i++) {
        int digit = pass[i] - '0';
        //convert current digit from ascii to int
        int enc = (digit + offset) % 36;
        //take the value of the current digit and add the offset
        // % 36
        encrypted += EncodeChar(enc);
        // take some value from 1-36 and encode to a character
        offset = (offset + enc + i + 1) % 36;
        //generate a new offset, based on:
        // the previous offset
        // where we are in the loop
        // the semi-random digit we made up
        // add 1 to prevent divide by 0
    }
}