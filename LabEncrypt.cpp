#include "LabEncrypt.h"


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


static int Offset(const Passcode& pass) { //Find a starting offset
    if (pass.size() > 2) {
        //make sure the passcode is long enough, this is enforced elsewhere and double-checked here
        int dig1 = pass[0] - '0'; //take the first digit of the character array, convert from ascii to int
        int dig2 = pass[1] - '0';
        int dig3 = pass[2] - '0'; 
        return (pass.size() + dig1 + dig2 + dig3) % 36;
        // calculate a starting offset
        // add the first 3 digits together + size
        // return that value % 36
        // "psuedorandom" value between 0-35
        
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
// of the first 3 characters + password length

void encryptPass(const Passcode& pass, Passcode& encrypted) {
    const int outputLength = 32;
    int passLength = pass.size();
    int offset = Offset(pass); // generate initial offset
    int prevEnc = 0;
    encrypted.clear();

    for (int i = 0; i < (outputLength + 1); i++) {
        int digit = 0;
        int enc = 0;

        if (i < passLength) { //use the real digit while they are still available
            digit = pass[i] - '0';
        } else {
            //start padding out using psuedo-random digits from the passcode itself
            int index = prevEnc % passLength;
            digit = pass[index] - '0'; 

            if (i % 2 == 1) {
                digit = (digit + 1) * i;
            } //breaks up output pattern in codes like 0000, 1111, etc

        }
        


        enc = (digit + offset) % 36;
        //take the value of the current digit and add the offset
        // % 36

        if (i == outputLength) {
            //encrypted[0] = EncodeChar(enc);
            // loop back to the beginning, obscuring the first character
        } else {
            encrypted += EncodeChar(enc);
            // this is what runs for most of the loop
            // take some value from 0-35 and encode to a character
        }




        offset = (offset + enc + i + passLength) % 36;
        //generate a new offset, based on:
        // the previous offset
        // where we are in the loop
        // the semi-random digit we made up
        // the pass length

        prevEnc = enc;

    }
}