#include "mbed.h"
#include <string>
#include <map>

DigitalOut myled(LED1);

void fillMap();
void dot();
void dash();
void letterSpace();
void blinkMorse(string);
string stringToMorse(string); 

map <char, string> morseMap;

int main() {  
    string name = "radovan";
    fillMap();
    string morse = stringToMorse(name);
    blinkMorse(morse);
}

void blinkMorse(string code) {
    for(int i = 0; i < code.length(); i++)
    {
        switch(code[i]) {
            case '.':
                dot();
                break;
            case '-':
                dash();
                break;
            case '|':
                letterSpace();
                break;
        }
    }
}

void dot() {
    myled = 1;
    wait(0.3);
    myled = 0;
    wait(0.3);
}

void dash() {
    myled = 1;
    wait(0.9);
    myled = 0;
    wait(0.3);
}

void letterSpace() {
    wait(0.6);
}

string stringToMorse(string str) {
    string morseCode = "";
    for(int i = 0; i<str.length(); i++)
    {
        morseCode += morseMap[str[i]];
        morseCode += "|";
    }
    return morseCode;
}

void fillMap() {
    morseMap.insert(pair<char, string>('a', ".-"));
    morseMap.insert(pair<char, string>('a', "-.."));
    morseMap.insert(pair<char, string>('b', "-..."));
    morseMap.insert(pair<char, string>('c', "-.-."));
    morseMap.insert(pair<char, string>('d', "-.."));
    morseMap.insert(pair<char, string>('e', "."));
    morseMap.insert(pair<char, string>('f', "..-."));
    morseMap.insert(pair<char, string>('g', "--."));
    morseMap.insert(pair<char, string>('h', "...."));
    morseMap.insert(pair<char, string>('i', ".."));
    morseMap.insert(pair<char, string>('j', ".---"));
    morseMap.insert(pair<char, string>('k', "-.-"));
    morseMap.insert(pair<char, string>('l', ".-.."));
    morseMap.insert(pair<char, string>('m', "--"));
    morseMap.insert(pair<char, string>('n', "-."));
    morseMap.insert(pair<char, string>('o', "---"));
    morseMap.insert(pair<char, string>('p', ".--."));
    morseMap.insert(pair<char, string>('q', "--.-"));
    morseMap.insert(pair<char, string>('r', ".-."));
    morseMap.insert(pair<char, string>('s', "..."));
    morseMap.insert(pair<char, string>('t', "-"));
    morseMap.insert(pair<char, string>('u', "--."));
    morseMap.insert(pair<char, string>('v', "...-"));
    morseMap.insert(pair<char, string>('w', ".--"));
    morseMap.insert(pair<char, string>('x', "-..-"));
    morseMap.insert(pair<char, string>('y', "-.--"));
    morseMap.insert(pair<char, string>('z', "--.."));
}
