// CPSC 332 Compilers
//  Created by Samuel Flinkfelt and Will Zhang

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

// creates a struct for the lexeme and token combo.
struct lexeme{
    lexeme() : lexeme("",""){}
    lexeme(std::string lexeme, std::string token) : lexeme_(lexeme), token_(token){}
    //creates a friend fucntion to ostream so everytime the << is used it outputs the combo of lexeme and token. Also the setw sets width for printout
    friend std::ostream& operator << (std::ostream& os,const lexeme& l){
        return os << std::setw(10)<< l.token_ << "    =   " << l.lexeme_;}
    
    std::string lexeme_;
    std::string token_;
};

class compiler{
public:
    compiler() = default;
    //using switch casing for changing the state of the machine
    enum FSM { START, IN_ID, IN_INT,ST_NUM, IN_NUM, REAL, ST_COMM, IN_COMM, END_COMM, OPER, SEP, END_INT, END_ID, END_OPER, END_SEP, END_REAL};
    //initial state of machine is start
    enum FSM currState = START;
    
    void readin(std::string input,std::string output){
        lexeme phrase;//creates struct of list
        
        //START to read in the file
        char c;
        std::ifstream ifs;
        ifs.open(input);
        //if files open already, call error
        if(!ifs.is_open()){
            std::cerr << "Couldn't open code.txt";
            exit(1);
        }//read in file and read each character of the file
        while (ifs.get(c)){
            inputfile_.push_back(c);
        }
        //exectues the lexer function
        lexer(inputfile_, phrase);
        //END of reading in the file
        
        //START to print out list to file and also console
        std::ofstream ofs;
        ofs.open(output);
        ofs << std::setw(10) << "TOKENS" << "   |   " << "LEXEMES\n";
        ofs << "    --------------------- \n";
        for(lexeme& word : cont_){
            ofs << word << "\n";
            std::cout << word <<"\n";}
        ofs.close();
        ifs.close();
        //
    }
    
    void lexer(const std::vector<char> &input_,lexeme& phrase){
        char cache[100];
        int i = 0;
        for(int j = 0; j < input_.size(); j++){
        char cr = input_[j];
           switch(currState){
               case START: //State 1 read the character, make sure it's not a  space, newline or tab, if it's a alpha go to ID, if digit go to REAL#, if negative go to INTEGER, if OPERATOR or SEPERATOR go there.
                    if(cr == '\n' || cr == ' ' || cr == '\t' || cr == '\r'){
                        break;
                    } else {
                        cache[i++] = cr;
                        if(cr == '!'){
                            currState = ST_COMM;
                        }
                        else if(isalpha(cr) == 1){
                            currState = IN_ID;
                        }
                        else if(isdigit(cr) == 1){
                            currState = REAL;
                        }
                        else if(cr == '-'){
                            currState = IN_INT;
                        }
                        else if(isOperator(cr) == 1){
                            currState = OPER;
                        }
                        else if(isSeperator(cr) == 1){
                            currState = END_SEP;
                        }
                    }
                    break;
                    
                case IN_ID: //State 2
                    if (isalpha(cr) == 1 || isdigit(cr) == 1 || cr == '$')
                    {
                        cache[i++] = cr;
                        currState = IN_ID;
                    } else if(isOperator(cr)){
                        j--;
                        currState = END_OPER;
                    }else if(isSeperator(cr)){
                        j--;
                        currState = END_ID;
                    }
                    else {
                        currState = END_ID;
                    }
                    break;
                   
               case REAL:
                   if (isdigit(cr) == 1){
                       cache[i++] = cr;
                       currState = REAL;
                   }else if (cr == '.'){
                       cache[i++] = cr;
                       currState = IN_INT;
                   }else if(isOperator(cr) == 1 || isSeperator(cr) == 1){
                       j--;
                       currState = END_REAL;
                       
                   } else {
                       currState = END_REAL;
                   }
                   break;

                case IN_INT:
                    if(isdigit(cr) == 1){
                        cache[i++] = cr;
                        currState = IN_INT;
                    }else{
                        currState = END_INT;
                    }
                    break;
                   
                case OPER:
                   if(cr == '+' || cr == '-'){
                       cache[i++] = cr;
                       currState = END_OPER;
                    }else{
                        currState = END_OPER;
                    }
                    break;

                case ST_COMM: // If a comment starts
                    cache[i++] = cr;
                    currState = IN_COMM;
                    break;
                   
                case IN_COMM://State 6
                    if(cr == '!'){
                        cache[i++] = cr;
                        currState = END_COMM;
                    }
                    else{
                        cache[i++] = cr;
                    }
                    break;
            }
            // during each character it checks to see if it's a end state.  If it is then prints out the string in the buffer memory and assigns it a TOKEN and LEXEME to the vector, which are tied to eachother.
            if (currState == END_ID || currState == END_COMM || currState == END_INT || currState == END_REAL || currState == END_OPER || currState == END_SEP)//End state, cleans up cache and resets machine state
            {
                cache[i] = '\0';
            //Keyword has 3 different if statements because a KEYWORD can also be an IDENTIFIER
                if(currState == END_ID){
                    if(isKeyword(cache) == true){
                        phrase.token_ = "KEYWORD";
                        phrase.lexeme_ = cache;
                        cont_.push_back(phrase);
                    }else{
                        phrase.token_ = "IDENTIFIER";
                        phrase.lexeme_ = cache;
                        cont_.push_back(phrase);
                    }
                }else if(currState == END_COMM){
                    phrase.token_ = "COMMENT";
                    phrase.lexeme_ = cache;
                    cont_.push_back(phrase);
                }else if(currState == END_SEP){
                    phrase.token_ = "SEPERATOR";
                    phrase.lexeme_ = cache;
                    cont_.push_back(phrase);
                }else if(currState == END_OPER){
                    phrase.token_ = "OPERATOR";
                    phrase.lexeme_ = cache;
                    cont_.push_back(phrase);
                }else if(currState == END_INT){
                    phrase.token_ = "INTEGER";
                    phrase.lexeme_ = cache;
                    cont_.push_back(phrase);
                }else if(currState == END_REAL){
                    phrase.token_ = "REAL";
                    phrase.lexeme_ = cache;
                    cont_.push_back(phrase);
                }
                currState = START;
                memset(cache, '\0', sizeof(cache));
                i = 0;
            }
        }
    }
    //pool of Keywords, checks to see if the character is in this pool, when the functions is called.
    bool isKeyword(char cache[]){
        const char* keyword[] = {"int", "bool", "float", "if", "then", "else", "endif", "while", "whileend", "do", "doend", "for","and", "or", "forend", "input", "output", "function", "true", "false"};
        size_t n = sizeof(keyword)/sizeof(keyword[0]);
        for(int i = 0; i < n; i++){
            if(strcmp(keyword[i], cache) == 0){
                return true;
            }
        }
        return false;
    }
    //pool of Operators, checks to see if the character is in this pool, when the functions is called.
    bool isOperator(char cr){
        const char* operators[] = { "=", ">", "<", "-", "+", "/", "%", "*", "==", "++"};
        size_t n = sizeof(operators)/sizeof(operators[0]);
        for(int i =0; i < n; i++){
            if(strchr(operators[i], cr)){
                return true;
            }
        }
        return false;
    }
    //pool of Seperators, checks to see if the character is in this pool, when the functions is called.
    bool isSeperator(char cr){
        const char* seperator[] = { "'", "(", ")", "{", "}", "[", "]", ",", ":", ";", "!","#", "."};
        size_t n = sizeof(seperator)/sizeof(seperator[0]);
        for(int i = 0; i < n; i++){
            if(strchr(seperator[i], cr)){
                return true;
            }
        }
        return false;
    }
    //This is the container of the lexemes and tokens
private:
    std::vector<char>inputfile_;
    std::vector<lexeme>cont_;
};

int main(int argc, const char * argv[]) {
    std::string input, output;
    std::cout<< "Please enter file name:";
    std::cin >> input;
    std::cout<< "Please enter a output file name:";
    std::cin >> output;
    compiler().readin(input, output);
    
    printf("\nDone\n");

    return 0;
}
