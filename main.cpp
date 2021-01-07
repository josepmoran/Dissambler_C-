/*
*  
*  
*  Programming Assignment 1
*  
*  Program takes two inputs, .obj file and .sym file (IN THAT ORDER),
*  and converts it into one output, an .lis file.
*  All input/output files are in .txt format for convience.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <vector>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>


using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;

struct opcodes {
    string opcode;
    string mneumonic;
    int format;
};

// all the opcodes!
struct opcodes op[] = {
  {"18", "ADD", 3}, {"58", "ADDF", 3}, {"90", "ADDR", 2}, {"40", "AND", 3}, {"B4", "CLEAR", 2}, {"28", "COMP", 3},
  {"88", "COMPF", 3}, {"A0", "COMPR", 2}, {"24", "DIV", 3}, {"64", "DIVF", 3}, {"9C", "DIVR", 2}, {"C4", "FIX", 1},
  {"C0", "FLOAT", 1}, {"F4", "HIO", 1}, {"3C", "J", 3}, {"30", "JEQ", 3}, {"34", "JGT", 3}, {"38", "JLT", 3},
  {"48", "JSUB", 3}, {"00", "LDA", 3}, {"68", "LDB", 3}, {"50", "LDCH", 3}, {"70", "LDF", 3}, {"08", "LDL", 3},
  {"6C", "LDS", 3}, {"74", "LDT", 3}, {"04", "LDX", 3}, {"D0", "LPS", 3}, {"20", "MUL", 3}, {"60", "MULF", 3},
  {"98", "MULR", 2}, {"C8", "NORM", 1}, {"44", "OR", 3}, {"D8", "RD", 3}, {"AC", "RMO", 2}, {"4C", "RSUB", 3},
  {"A4", "SHIFTL", 2}, {"A8", "SHIFTR", 2}, {"F0", "SIO", 1}, {"EC", "SSK", 3}, {"0C", "STA", 3}, {"78", "STB", 3},
  {"54", "STCH", 3}, {"80", "STF", 3}, {"D4", "STI", 3}, {"14", "STL", 3}, {"7C", "STS", 3}, {"B0", "STSW", 3},
  {"84", "STT", 3}, {"10", "STX", 3}, {"1C", "SUB", 3}, {"5C", "SUBF", 3}, {"94", "SUBR", 3}, {"B0", "SVC", 3},
  {"E0", "TD", 3}, {"F8", "TIO", 3}, {"2C", "TIX", 3}, {"B8", "TIXR", 3}, {"DC", "WD", 3}
};
////////////////////////
// HELPER FUNCTIONS
////////////////////////

// returns mneumonic of given opcode
string getMneumonic(string opcode) {
    for (int i = 0; i < 59; i++) {
        if (op[i].opcode == opcode) return op[i].mneumonic;
    }
}

// returns format of given opcode
int getFormat(string opcode) {
    for (int i = 0; i < 59; i++) {
        if (op[i].opcode == opcode) return op[i].format;
    }
}

// converts a string hexadecimal to an int decimal
int HtoD(string hexadecimal) {
    std::stringstream ss;
    ss << std::hex << hexadecimal;
    unsigned int decimal;
    ss >> decimal;
    return decimal;
}

// converts a signed string hexadecimal to an int decimal
int signedHtoD(string hexadecimal) {
    if (hexadecimal.substr(0,1) == "8" || hexadecimal.substr(0,1) == "9" || hexadecimal.substr(0,1) == "A" ||
        hexadecimal.substr(0,1) == "B" || hexadecimal.substr(0,1) == "C" || hexadecimal.substr(0,1) == "D" ||
        hexadecimal.substr(0,1) == "E" || hexadecimal.substr(0,1) == "F") {
        int inthex = HtoD(hexadecimal);
        int x = 4096;//in hex is 10000
        int decimal = inthex - x; // 10000 - your hex
    }
    else return HtoD(hexadecimal);
}

// takes the first byte of object code and converts to the corresponding opcode
// e.g. object code: 57C003
// takes "57" and returns the opcode "54" which corresponds to mneumonic "STCH"
// useful for format 3/4
string firstByteToOpcode(string hexadecimal) {
    string x = hexadecimal.substr(1, 1);
    if (x == "1" || x == "2" || x == "3") return (hexadecimal.substr(0, 1) + "0");
    else if (x == "5" || x == "6" || x == "7") return (hexadecimal.substr(0, 1) + "4");
    else if (x == "9" || x == "A" || x == "B") return (hexadecimal.substr(0, 1) + "8");
    else if (x == "D" || x == "E" || x == "F") return (hexadecimal.substr(0, 1) + "C");
    else return "Invalid hex";
}

// finds nixbpe flags from 2nd & 3rd hex value of object code
string getNixbpe(string hexadecimal) {
    int temp1 = HtoD(hexadecimal.substr(0, 1));  // convert to decimal
    int temp2 = HtoD(hexadecimal.substr(1, 1));
    string nixbpe = "";

    // first two flags "ni"
    temp1 = temp1 % 4;
    if (temp1 == 1) nixbpe += "01";
    else if (temp1 == 2) nixbpe += "10";
    else nixbpe += "11";

    // last four flags "xbpe"
    switch (temp2) {
        case 0:
            nixbpe += "0000";
            break;
        case 1:
            nixbpe += "0001";
            break;
        case 2:
            nixbpe += "0010";
            break;
        case 4:
            nixbpe += "0100";
            break;
        case 8:
            nixbpe += "1000";
            break;
        case 9:
            nixbpe += "1001";
            break;
        case 10:
            nixbpe += "1010";
            break;
        case 12:
            nixbpe += "1100";
            break;
    }
    return nixbpe;
}

// formats mneumonic for output files
string writeMneumonic(string hexadecimal) {
    string currentMneumonic = "";
    for (int i = 0; i < 59; i++) {
        if (op[i].opcode == hexadecimal) currentMneumonic = op[i].mneumonic;
    }

    switch (currentMneumonic.length()) {
    case 1:
        return (currentMneumonic + "      ");
        break;
    case 2:
        return (currentMneumonic + "     ");
        break;
    case 3:
        return (currentMneumonic + "    ");
        break;
    case 4:
        return (currentMneumonic + "   ");
        break;
    case 5:
        return (currentMneumonic + "  ");
        break;
    case 6:
        return (currentMneumonic + " ");
        break;
    }
}

///////////////////////////////////////////////////////////////



// converts a int decimal  to an String hexadecimal
string DtoH(int i) {

    std::stringstream ss;
    ss << std::hex << i;
    string variable =ss.str();
    std::string var = variable;
    for(i=0;i<=var.size();i++){
    var[i] = toupper(var[i]);
    }
  //  cout <<var;

    return var;
}
////////////////////
string FormatLoc(int loc)//Get the integer from Loc and call DtoH to obtain a string hexadecimal, and finally add the
//correct format to 0,    for example    Loc=23 and get Loc=0023, Loc=15 and get Loc=000F
{
    string variable=DtoH(loc);
    string variable2="";
    if(loc <=15)
    {
        variable2="000";
    }
    else if(loc >=16 && loc<=255)
    {
        variable2="00";
    }
    else if(loc >255 && loc<4095)
    {
        variable2="0";
    }
    else{
         variable2="";
    }
    variable2.append(variable);




return variable2;
}


/////////////////
//Search in Symbol if displacment is equal and return the displacement for format 4
//Input (objecSymbol, displacement)
//return output (displacement) or return "" if fail
string SearchInSymbol(vector<vector<string> > objSymCode, string displacement) {
    string symbol = "";
    for (int x = 0;x < objSymCode.size();x++) {
        if (objSymCode[x][1].substr(1, 5) == displacement) {
            symbol = objSymCode[x][0];
            //cout << "\nobject code: " <<objSymCode[x][1].substr(1,5);
            //cout << "\ndisplacemente: " << displacement;
        }
    }


    return symbol;
}


/////////////////
//Search in Literal if displacment is equal and return the displacement for format 4
//Input (objecSymbol, displacement)
//return output (displacement) or return "" if fail
string SearchInLiteral(vector<vector<string> > obj, string displacement) {
    string literal = "";
    for (int x = 0;x < obj.size();x++) {
        if (obj[x][3].substr(1, 5) == displacement) {
            literal = obj[x][1];
            //cout << "\nobject code: " <<objSymCode[x][1].substr(1,5);
            //cout << "\ndisplacemente: " << displacement;
        }
    }


    return literal;
}

/////////////////////////////

void writeLis(vector<vector<string> > obj, vector<vector<string> > sym, vector<vector<string> > lit) {
    ofstream lis;
    lis.open("sample.lis");
    lis << ".  ASSEMBLY LISTING CODE FOR ASSIGNMENT #1" << endl;
    lis << obj[1][0].substr(3, 4).c_str() << "    ";  // initial loc
    lis << obj[0][1] << "        " << "START" << endl;  // SUM

    int index = 9; // index of first hex value of current object code
    // int endIndex = 0; // index of next object code
    int textRecordLength = obj[1][0].length();
    int loc = HtoD(obj[1][0].substr(1, 6)); // convert initial loc to decimal
    int base = 0;
    string nixbpe = "";
    bool foundInSym = false;
    bool foundInLit = false;
    string foundSymBase = "";

    // stops when it hits the end of the record
    while (index < textRecordLength) {
        // resetting booleans
        foundInSym = false;
        foundInLit = false;

        // convert loc from int to hex
        string locString = FormatLoc(loc);

        // check literals and write if found
        for (int i = 0; i < lit.size(); i++) {
            if (locString == lit[i][3].substr(2, 4)) {
                lis << "          LTORG" << endl;
                lis << locString << "          *       " << lit[i][1] << "        " << lit[i][1].substr(3, 2) << endl;
                loc ++;
                index = index + 2;
            }
        }

        locString = FormatLoc(loc);
        lis << locString << "    ";

        // check if address is in symbol table
        for (int i = 0; i < sym.size(); i++) {
            if (locString == sym[i][1].substr(2, 4)) {
                lis << sym[i][0];
                foundInSym = true;
            }
        }
        if (!foundInSym) lis << "     ";

        // find the opcode (format 3/4), mneumonic, and format
        string opcode = firstByteToOpcode(obj[1][0].substr(index, 2));
        string mneumonic = getMneumonic(opcode);
        int format = getFormat(opcode);

        // format 1
        if (format == 1) {
            // not included in this disassembler
        }

        // format 2
        else if (format == 2) {
            string format2Opcode = obj[1][0].substr(index, 2);
            lis << getMneumonic(format2Opcode);

            int arg1 = HtoD(obj[1][0].substr(index + 2, 1));
            switch(arg1){
                case 0:
                    lis << "A";
                    break;
                case 1:
                    lis << "X";
                    break;
                case 2:
                    lis << "L";
                    break;
                case 3:
                    lis << "B";
                    break;
                case 4:
                    lis << "S";
                    break;
                case 5:
                    lis << "T";
                    break;
                case 6:
                    lis << "F";
                    break;
                case 8:
                    lis << "PC";
                    break;
                case 9:
                    lis << "SW";
                    break;
            }
            // if not opcode that uses one arg
            if(format2Opcode != "B0" && format2Opcode != "B4" && format2Opcode != "B8") {
                int arg2 = HtoD(obj[1][0].substr(index + 3, 1));
                switch(arg2){
                    case 0:
                        lis << ",A";
                        break;
                    case 1:
                        lis << ",X";
                        break;
                    case 2:
                        lis << ",L";
                        break;
                    case 3:
                        lis << ",B";
                        break;
                    case 4:
                        lis << ",S";
                        break;
                    case 5:
                        lis << ",T";
                        break;
                    case 6:
                        lis << ",F";
                        break;
                    case 8:
                        lis << ",PC";
                        break;
                    case 9:
                        lis << ",SW";
                        break;
                }
            }
            lis << "     " << obj[1][0].substr(index, 4) << endl;
        }

        // format 3, or possibly 4
        else {
            nixbpe = getNixbpe(obj[1][0].substr(index + 1, 2));
            if (mneumonic == "LDB") {
                int n = 3;
                int tmp = 0;
                if(nixbpe.substr(5,1)=="1") n = 5;
                else if(nixbpe.substr(4,1)=="1") tmp = loc;
                else if(nixbpe.substr(3,1)=="1") tmp = base;

                if(nixbpe.substr(1,1)=="1") base = tmp + HtoD(obj[1][0].substr(index + 3, n));
            }

            if (nixbpe.substr(5, 1) == "1") { //If true then is/////// Format 4 Begin ////////////////////////



                //Check the third column, to detect the e value




                /////////write the Tag or LAbel for example SUM
                lis << "+"; //  write + sign to lis file
                lis << getMneumonic(opcode);//write the opcode i.e LDB
                lis << "     ";

                //////////write the symbol or the literal///////
                //search the symbol///////////////
                string symbol = SearchInSymbol(sym, obj[1][0].substr(index + 3, 5));
                if (mneumonic == "LDB") {
                    foundSymBase = symbol;
                }
                //search for literal//////
                string literal = SearchInLiteral(lit, obj[1][0].substr(index + 3, 5));

                if (symbol != "")
                {
                    if (nixbpe.substr(0, 2) == "11")
                    {
                        // simple addressing
                        lis << " ";
                    }
                    else if (nixbpe.substr(0, 2) == "10")
                    {
                        // indirect addressing
                        lis << "@";
                    }
                    else {   //inmediate ni = 01
                        // immediate addressing
                        lis << "#";
                    }
                    lis << symbol;

                }
                else {


                    if (nixbpe.substr(0, 2) == "11")
                    {
                        // simple addressing, ni = 11
                        lis << " ";

                    }
                    else if (nixbpe.substr(0, 2) == "10")
                    {
                        // indirect addressing, ni = 10
                        lis << "@";
                    }
                    else {
                        // immediate addressing, ni = 01
                        lis << "#";
                        lis << obj[1][0].substr(index + 4, 4);
                    }
                }
                format = 4;
                //////Check the M record if exist/////
                string tempLocString="00";
                tempLocString.append(FormatLoc(loc+1));
                
                for(int x=2;x<obj.size()-1;x++) {
                    if(obj[x][0]=="M") {
                        if(obj[x][1]==tempLocString) {
                         //cout<<"Findd M record:"<<tempLocString<<endl;
                        }
                        break;
                    }
                }
                ///////////check the M record if exist/////

                /////End of format4//////////////
            }
            else {///////////Begin of format 3//////////////
                lis << " "; // add a space to lis file (no +)
                lis << writeMneumonic(opcode);

                int TA = 0;
                string disp = obj[1][0].substr(index + 3, 3);
                int PC = loc + 3;
                
                // addressing: simple, immediate, indirect
                if (nixbpe.substr(0, 2) == "11") lis << " ";
                else if (nixbpe.substr(0, 2) == "10") lis << "@";
                else lis << "#";

                // find if direct, PC rel, or base rel
                if (nixbpe.substr(4, 1) == "1") TA = PC + signedHtoD(disp); // PC relative
                else if (nixbpe.substr(3, 1) == "1") TA = base + HtoD(disp); // base relative
                else TA = HtoD(disp);
                string hexTA = FormatLoc(TA);

                if (mneumonic == "LDX") {
                    lis << obj[1][0].substr(index + 5, 1);
                }
                else {
                    // check symbols and literals
                    foundInSym = false;
                    foundInLit = false;
                    for (int i = 0; i < sym.size(); i++) {
                        if (hexTA == sym[i][1].substr(2, 4)) {
                            lis << sym[i][0];
                            if (mneumonic == "LDB") {
                               foundSymBase = sym[i][0];
                            }
                            foundInSym = true;
                        }
                    }
                    for (int i = 0; i < lit.size(); i++) {
                        if (hexTA == lit[i][3].substr(2, 4)) {
                            lis << lit[i][1];
                            foundInLit = true;
                        }
                    }
                    if (!foundInLit && !foundInSym) lis << "        ";
                }
            }

            // indexed
            if (nixbpe.substr(2, 1) == "1") {
                lis << ",X";
            }
            lis << "     ";

            // object code
            if (format == 3) lis << obj[1][0].substr(index, 6) << endl; //write format 3 opcode in lis
            else{
            lis << obj[1][0].substr(index, 8) << endl;  //write format 4 opcode in lis

            }
            if (mneumonic == "LDB") {
                lis << "          BASE    " << foundSymBase << endl;
            }
        }
        index += 2 * format;
        loc += format;
    }
    
    
    int i = 0;
    int symLoc = 0;
    for (i = 0; i < sym.size()-1; i++) {
        symLoc = HtoD(sym[i][1].substr(2, 4));
        if (loc <= symLoc) {
            lis << sym[i][1].substr(2, 4) << "    " << sym[i][0] << "  RESW    " << (HtoD(sym[i+1][1].substr(2, 4)) - symLoc) / 3 << endl;
        }
    }
    lis << sym[i][1].substr(2, 4) << "    " << sym[i][0] << "  RESW    " << "1" << endl;

    // end record
    lis << "        END     ";
    int objSize = obj.size();
    string endRecordAddr = obj[objSize-1][1];
    for (int i = 0; i < sym.size(); i++) {
        if (endRecordAddr == sym[i][1]) {
            lis << sym[i][0] << endl;
        }
    }


    lis.close();
}

vector<string> getHeader(string line) {
    vector<string> temp; //holding vector for each line
    temp.push_back(line.substr(0,1)); // Col. 1
    temp.push_back(line.substr(1,6)); // Col. 2-7
    temp.push_back(line.substr(7,6)); // Col. 8-13
    temp.push_back(line.substr(13,6)); // Col. 14-19
    return temp;
}

vector<string> getText(string line) {
    vector<string> temp; //holding vector for each line
    temp.push_back(line);
    return temp;
}

vector<string> getMod(string line) {
    vector<string> temp; //holding vector for each line
    temp.push_back(line.substr(0,1)); // Col. 1
    temp.push_back(line.substr(1,6)); // Col. 2-7
    temp.push_back(line.substr(7,2)); // Col. 8-9
    return temp;
}

vector<string> getEnd(string line) {
    vector<string> temp; //holding vector for each line
    temp.push_back(line.substr(0,1)); // Col. 1
    temp.push_back(line.substr(1,6)); // Col. 2-7
    return temp;
}

////////////////////////
// END OF HELPER FUNCTIONS
////////////////////////

int main(int argc, char *argv[])
{
    if (argc < 3) //if two inputs are not found
    {
        cout << "No bueno." << '\n';
        exit(1);
    }
    ifstream file1(argv[1]); // INPUT 1: .obj file
    ifstream file2(argv[2]); // INPUT 2: .sym file

    //Gets total input file lines
    string s;
    int sTotal = 0;
    ifstream in;
    in.open(argv[1]);
    while(getline(in, s)) {
        sTotal ++;
    }
    in.close();

    ////////// ACCESSING/READING INPUT FILE //
    vector<string> temp; //holding vector for each line
  vector<vector<string> > objectCode; //2D vector for holding object code
    vector<vector<string> > symCode; //2D vector for holding symbol code
    vector<vector<string> > literalCode; //2D vector for holding literals
  string line;

    // Gets Object File //
    while(getline(file1,line)) //getting name, literal, length, address
    {
        if(line.substr(0,1) == "H") {
            // cout << "H" << endl;
            objectCode.push_back(getHeader(line));
        }
        if(line.substr(0,1) == "T") {
            // cout << "T" << endl;
            objectCode.push_back(getText(line));
        }
        if(line.substr(0,1) == "M") {
            // cout << "M" << endl;
            objectCode.push_back(getMod(line));
        }
        if(line.substr(0,1) == "E") {
            // cout << "E" << endl;
            objectCode.push_back(getEnd(line));
        }
    }

    // Gets Sym File
    getline(file2, line);
    getline(file2, line);
    getline(file2, line);
    while(line.size() == 17) //getting symbol, value, flag
    {
        temp.clear();
        temp.push_back(line.substr(0,6)); // Symbol
        temp.push_back(line.substr(8,6)); // Value
        temp.push_back(line.substr(16,1)); // Flag
        symCode.push_back(temp);
        getline(file2,line);
    }

    // Gets Literals
    getline(file2,line);
    getline(file2,line);
    while(getline(file2,line)) //getting name, literal, length, address
    {
        temp.clear();
        temp.push_back(line.substr(0,6)); // name
        temp.push_back(line.substr(8,6)); // literal
        temp.push_back(line.substr(19,1)); // length
        temp.push_back(line.substr(24,6)); // address
        literalCode.push_back(temp);
    }


    //DEBUG//
    cout << "===ObjectCode 2D Vector===" << endl;
    for (int i = 0; i < objectCode.size(); i++){
        for (int j = 0; j < objectCode[i].size(); j++){
                cout << objectCode[i][j] << " ";
            }
            cout << endl;
    }
    cout << "===Symbol 2D Vector===" << endl;
    cout << "Symbol  Value   Flags:" << endl;
    cout << "-----------------------" << endl;
    for (int i = 0; i < symCode.size(); i++){
        for (int j = 0; j < symCode[i].size(); j++){
                cout << symCode[i][j] << " ";
            }
            cout << endl;
    }
    cout << "===Literal 2D Vector===" << endl;
    cout << "Name    Literal  Length Address:" << endl;
    cout << "------------------------------" << endl;
    for (int i = 0; i < literalCode.size(); i++){
        for (int j = 0; j < literalCode[i].size(); j++){
                cout << literalCode[i][j] << " ";
            }
            cout << endl;
    }
    //END OF DEBUG//

    
    file1.close();
    file2.close();
    writeLis(objectCode, symCode, literalCode);
    return 0;
}
