// a skeleton implementation of a tokeniser

#include "tokeniser.h"
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    // the current input character, initiliased to '\n'
    // the eof marker can be confused with a legal character but not one accepted by our tokeniser
    static char ch = '\n' ;

    // the number of characters return by nextch() so far
    static int ch_count = 0 ;

    // the current line number and column, initialised to line 0 column 0
    // first call of nextch() will increment the line number to 1
    static int line_num = 0 ;
    static int column_num = 0 ;

    // the line number, column number and ch_count for the first character in the current token
    static int start_line = 0 ;
    static int start_column = 0 ;
    static int start_ch_count = 0 ;

    //control the number of tab
    static int tabNumber = 0;
    
    static char temp = ' ';
    
    static int digitNumber=0;
    
    /*================================================================================================================*/
    /*                                                 TOKEN-CONTEXT                                                  */
    /*================================================================================================================*/
    //record the ch
    static string strtemp;
    //store the strtemp
    static vector<string> myvector;
    //a method to print the line number according to the output
    static string line(int currentline);
    
    
    
    // generate a context string for the given token
    // all lines are shown after the preprocssing of special characters
    // it shows the line before the token, with a '$' marking the end of line,
    // the line containing the token up to the end of the token, and
    // a line of spaces ending with a ^ under the token's first character to mark its position
    // the lines are prefixed by line numbers, right justified in four spaces, followed by ": ", eg "  45: "
    // NOTE: if a token includes newline character(s), only display the token before the first newline followed by a '$'
    // NOTE: this function is not required for the milestone submission
    
    //a method to print the line number according to the output
    static string line(int currentline)
    {
        
        string currentstr = to_string(currentline);
        
        if(currentline<10)
        {
            currentstr.insert(currentstr.begin(),3,' ');
        }
        else if(currentline>=10 && currentline < 100)
        {
            currentstr.insert(currentstr.begin(),2,' ');
        }
        else if(currentline>=100 && currentline < 1000)
        {
            currentstr.insert(currentstr.begin(),1,' ');
        }
        else if(currentline>=1000)
        {
            currentstr.insert(currentstr.begin(),0,' ');
        }
        currentstr.push_back(':');
        currentstr.push_back(' ');
        
        return currentstr;
    }
    
    string token_context(Token token)
    {
        string spelling;
        //to remember the line number of the current token
        int currentline = token_line(token);
        //to remember the column number of the current token
        int currentcolumn = token_column(token);
        
        for(int i=currentline-1; i<=currentline; i++)
        {
            if(i == currentline-1)
            {
                // if the token in the first line
                if(currentline == 1)
                {
                    spelling += "   1: ";
                    for(int j=0; j<myvector[0].length(); j++)
                    {
                        if(j == currentcolumn-1)
                        {
                            break;
                        }
                        spelling += myvector[0][j];
                    }
                    if(token_is(token, tk_newline))
                    {
                        spelling+= "$";
                    }
                    else if(token_is(token, tk_eol_comment))
                    {
                        spelling+="//";
                        spelling+= token_spelling(token);
                        spelling+="$";
                    }
                    else if(token_is(token, tk_adhoc_comment))
                    {
                        spelling+="/*";
                        string str1;
                        string str2 = "\n";
                        int number = token_length(token)-4;
                        for(int k = 0; k < number; k++)
                        {
                            spelling+= token_spelling(token)[k];
                            if(k<=number-3)
                            {
                                str1 = token_spelling(token).substr(k+1, 1);
                            }
                            if(str1 == str2)
                            {
                                spelling+= "$";
                                break;
                            }
                        }
                        if(spelling[spelling.length()-1]!='$')
                        {
                            spelling+="*/";
                        }
                    }
                    else if(token_is(token, tk_scientific))
                    {
                        int number1 = currentcolumn-1;
                        int number2 = number1 + token_length(token);
                        for( ; number1 < number2; number1++)
                        {
                            spelling += myvector[0][number1];
                        }
                    }
                    else if(token_is(token, tk_float))
                    {
                        int number1 = currentcolumn-1;
                        int number2 = number1 + token_length(token);
                        for( ; number1 < number2; number1++)
                        {
                            spelling += myvector[0][number1];
                        }
                    }
                    else
                    {
                        spelling+= token_spelling(token);
                    }
                    spelling += "\n";
                    spelling += "      ";
                    for(int i=0; i<currentcolumn-1; i++)
                    {
                        spelling += " ";
                    }
                    spelling += "^";
                    spelling += "\n";
                    break;
                }
                //if the token in the other lines except the first line
                else
                {
                    spelling += line(currentline-1);
                    for (int j = 0; j < myvector[currentline - 2].length(); j++)
                    {
                        spelling += myvector[currentline - 2][j];
                    }
                }
            }
            //print the current line of the current token
            else
            {
                spelling += line(currentline);
                for (int i = 0; i < currentcolumn-1; i++)
                {
                    spelling+= myvector[currentline-1][i];
                }
                if(token_is(token, tk_newline))
                {
                    spelling+= "$";
                }
                else if(token_is(token, tk_eol_comment))
                {
                    spelling+="//";
                    spelling+= token_spelling(token);
                    spelling+="$";
                }
                else if(token_is(token, tk_adhoc_comment))
                {
                    spelling+="/*";
                    string str1;
                    string str2 = "\n";
                    int number = token_length(token)-4;
                    for(int i = 0; i < number; i++){
                        spelling+= token_spelling(token)[i];
                        if(i <= number-3)
                        {
                            str1 = token_spelling(token).substr(i+1,1);
                        }
                        if(str1 == str2)
                        {
                            spelling+= "$";
                            break;
                        }
                    }
                    if(spelling[spelling.length()-1]!='$')
                    {
                        spelling+="*/";
                    }
                }
                else if(token_is(token, tk_scientific))
                {
                    int number1 = currentcolumn-1;
                    int number2 = number1 + token_length(token);
                    for( ; number1 < number2; number1++)
                    {
                        spelling += myvector[0][number1];
                    }
                }
                else if(token_is(token, tk_float))
                {
                    int number1 = currentcolumn-1;
                    int number2 = number1 + token_length(token);
                    for( ; number1 < number2; number1++)
                    {
                        spelling += myvector[0][number1];
                    }
                }
                else
                {
                    spelling+= token_spelling(token);
                }
                spelling += "\n";
                spelling += "      ";
                for(int i=0; i<currentcolumn-1; i++)
                {
                    spelling += " ";
                }
                spelling += "^";
                spelling += "\n";
                break;
            }
        }
        return spelling;
    }
    
    // read next character if not at the end of input and update the line and column numbers
    static void nextch()
    {
        if ( ch == EOF ) return ;

        if ( ch == '\n' )                   // if last ch was newline ...
        {
            line_num++ ;                    // increment line number
            column_num = 0 ;                // reset column number
        }

        ch = read_char() ;                  // read the next character, probably from stdin but this could change during testing
        column_num++ ;                      // increment the column number

                                            // additional code will be required here to handle preprocessing of '\t' and '\r'
    
        //if input is '\t',
        if (ch == '\t')
        {
            if (tabNumber < 3)
            {
                //add 3 more ' '
                repeat_char();
                tabNumber++;
            }
            else
            {
                tabNumber = 0;
            } 
        }
        //if the input is '\r', convert it to '\n'
        if (ch == '\r')
        {
            temp = read_char();
            if (temp == '\n')
            {
                ch = temp;
            }
            else
            {
                ch = '\n';
            }
        }
        // use strtemp to record the input for use by token_context()
        strtemp+=ch;
        if(ch == '\t'){
            strtemp.erase(strtemp.length()-1);
            strtemp+=' ';
        }
        if(ch == '\n'){
            strtemp.erase(strtemp.length()-1);
            strtemp+='$';
            strtemp+='\n';
            myvector.push_back(strtemp);
            strtemp.clear();
        }
        ch_count++ ;
    }

    // initialise the tokeniser
    static void initialise()
    {
                                            // add any other initialisation code you need here
                                            // ...

        nextch() ;                          // make first call to nextch to initialise ch
    }
    
    /*================================================================================================================*/
    /*                                               PARSE TOKENISER                                                  */
    /*================================================================================================================*/

    ////////////////////////////////////////////////////////////////////////

    // NOTE: when creating a new token the length can be calculated as ch_count - start_ch_count
    //       eg new_token(tk_identifier,spelling,start_line,start_column,ch_count - start_ch_count) ;
    //       the eoi token must be given length 0

    // called when we find end of input or an error
    static Token parse_eoi()
    {
        // simulate end of input in case this is handling a bad token rather than a real end of input
        ch = EOF ;
        myvector.push_back(strtemp);
        // return an eoi token
        return new_token(tk_eoi,"",start_line,start_column,0) ;
    }

    static Token parse_wspace(TokenKind kind,string spelling,int line_num, int column_num,int ch_count)
    {
        // always read the next character - we have read past the end of the token
        start_ch_count = 1;
        nextch() ;

        // return a new Token object
        return new_token(kind,spelling,start_line,start_column,start_ch_count) ;
    }

    static Token parse_identifier()
    {

        string spelling(1,ch);
        if (spelling == "$")
        {
            return parse_eoi() ;
        }
        else{
            start_ch_count = 1;
            nextch();

            while (isalnum(ch) || ch == '$' || ch == '.' || ch == '_' )
            {
                spelling += ch ;
                start_ch_count++;
                nextch() ;
            }
            return new_token(keyword_kind(spelling,tk_identifier),spelling,start_line,start_column,start_ch_count) ;
        }   
    }
    static Token parse_float(string spelling, int start_ch_count, int digitNumber)
    {
        //third char
        nextch();
        if(!isdigit(ch)||ch=='e')
        {
            return parse_eoi() ;
        }
        if(ch=='0')
        {
            while(ch=='0')
            {
                spelling+= ch;
                start_ch_count++;
                nextch();
            }
            if(ch=='E')
            {
                spelling+= ch;
                start_ch_count++;
                nextch();
                spelling+= ch;//E-
                start_ch_count++;
                nextch();//E-0
                spelling+= ch;
                start_ch_count++;
                nextch();
                return new_token(tk_scientific,"0.0e+0",start_line,start_column,start_ch_count);
                
            }
            return new_token(tk_float,"0.0",start_line,start_column,start_ch_count) ; 
        }
        else
        {
            while(isdigit(ch))
            {
                spelling += ch;
                start_ch_count++;
                nextch();
                stringstream geek(spelling);
                double x = 0.0;
                geek >> x;
                double num = x/(pow(10,(digitNumber-1)));//(digitNumber-1)*10
                if(ch=='e')
                {
                    spelling+=ch;//3.6e
                    start_ch_count++;
                    nextch();
                    if(ch=='-')
                    {
                        while(isdigit(ch)||ch=='-')
                        {
                            spelling+=ch;
                            start_ch_count++;
                            nextch();
                        }
                        return new_token(tk_scientific,spelling,start_line,start_column,start_ch_count);
                    }
                    else if(isdigit(ch))
                    {
                        //convert char to int (char)9->(int)9
                        int chNum = ch -'0';
                        int lastDigit = chNum + (digitNumber-1);
                        
                        string str = to_string(num);
                        
                        str.push_back('e');
                        str+="+";
                        string str2 = to_string(lastDigit);

                        str+=str2;
                        
                        spelling=str;
                        
                        while(isdigit(ch))
                        {
                            start_ch_count++;
                            nextch();
                        }
                        return new_token(tk_scientific,spelling,start_line,start_column,start_ch_count);
                    }
                    
                }
            }
            return new_token(tk_float,spelling,start_line,start_column,start_ch_count) ; 
        }
    }
    
    static Token parse_number()
    {
        string spelling = "";
        start_ch_count = 0;
        //first char
        spelling+=ch;
        start_ch_count++;
        
        if (ch=='0')
        {
            //second char
            nextch();
            if (ch=='.')
            {
                spelling+=ch;//0.
                start_ch_count++;//2

                return parse_float(spelling, start_ch_count,0);
            }
            else{
                return new_token(tk_integer,"0",start_line,start_column,start_ch_count) ;
            }
        }
        // 1...9
        else
        {
            //second char
            nextch();

            while(isdigit(ch))
            {
                spelling+=ch;
                start_ch_count++;
                digitNumber=start_ch_count;
                nextch();
            }
            if (ch=='.')
            {
                spelling+=ch;
                start_ch_count++;
                return parse_float(spelling, start_ch_count,digitNumber);
            }
            //deal with the scientific(888E)
            if(ch=='e'||ch=='E')
            {
                digitNumber=start_ch_count;
                start_ch_count++;
                //convert string to int
                stringstream geek(spelling);//888
                int x = 0;
                geek >> x;
                float num = x/(pow(10,(digitNumber-1)));//(digitNumber-1)*10
                nextch();
                int chNum = ch -'0';//(char)0->(int)0
                int lastDigit = chNum + (digitNumber-1);
                //convert int to string
                string str = to_string(num);//(int)num -> (string)num
                //delete the trailing zero
                for (int i = str.size() -1; i >= 0; i--)
                {
                    int number = str[i]-'0';
                    if(number==0)
                    {
                        str.pop_back();
                    }
                }
                str.push_back('e');
                str+="+";
                string str2 = to_string(lastDigit);
                str+=str2;
                spelling=str;
                while(isdigit(ch))
                {
                    start_ch_count++;
                    nextch();
                }
                return new_token(tk_scientific,spelling,start_line,start_column,start_ch_count);
            }
            return new_token(tk_integer,spelling,start_line,start_column,start_ch_count) ;
        }
    }

    static Token parse_symbol(TokenKind kind,string spelling,int line_num, int column_num,int ch_count)
    {
        // always read the next character - we have read past the end of the token
        start_ch_count = 1;
        //the second "/"
        nextch() ;
        if(ch=='/' )
        {
            spelling="";
            start_ch_count++;
            nextch() ;
            start_ch_count++;
            while (isalnum(ch) || ch == '"' || ch == '/' || ch == ',' || ch == ':' || ch == ' ' || ch == '.'
            || ch == '=' || ch == '+' || ch == '\n'|| ch=='\''|| ch=='['||ch==']'||ch=='('||ch==')'||ch==';'||ch=='-')
            {
                spelling += ch ;
                start_ch_count++;
                nextch() ;
                if(ch=='\n')
                {
                    nextch();
                    return new_token(tk_eol_comment,spelling,start_line,start_column,start_ch_count) ;
                }
            }
        }
        else if (ch == '*')     // /*
        {
            spelling = "";
            nextch();
            start_ch_count++;
            if(ch=='*')         // /**
            {
                spelling+=ch;   //// *
                nextch();
                start_ch_count++;
                // /*(\n)
                //  *.......
                if(ch=='\n')    //// *\n
                {
                    spelling+=ch;
                    nextch();
                    start_ch_count++;
                }
                // /** ............... */
                else if(ch==' ')
                {
                    while (isalnum(ch) || ch == '"' || ch == '/' || ch == ',' || ch == ':' || ch == ' '
                    || ch == '.' || ch == '=' || ch == '+'||ch=='*' ||ch=='\n'|| ch=='\''|| ch == '&')
                    {
                        spelling += ch ;        //// *\n[]
                        start_ch_count++;
                        nextch() ;
                        temp=ch;
                        if(ch=='*')
                        {
                            start_ch_count++;
                            nextch();           //...... */
                            if(ch=='/')
                            {
                                start_ch_count++;
                                nextch();           //...... */\n
                                return new_token(tk_adhoc_comment,spelling,start_line,start_column,start_ch_count) ;
                            }
                            // /** ..........
                            //  *  ........ */
                            else if(ch==' ')
                            {
                                spelling+=temp;
                                while (isalnum(ch) || ch == '"' || ch == '/' || ch == ',' || ch == ':' || ch == ' '
                                || ch == '.' || ch == '=' || ch == '+'||ch=='*' ||ch=='\n'|| ch=='\''|| ch == '&')
                                {
                                    spelling += ch ;        //// *\n[]
                                    start_ch_count++;
                                    nextch() ;
                                    if(ch=='*')
                                    {
                                        start_ch_count++;
                                        nextch();           //...... */
                                        start_ch_count++;
                                        nextch();           //...... */\n
                                        return new_token(tk_adhoc_comment,spelling,start_line,start_column,start_ch_count) ;
                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
            while (isalnum(ch) || ch == '"' || ch == '/' || ch == ',' || ch == ':' || ch == ' ' || ch == '.'
            || ch == '=' || ch == '+'||ch=='*' ||ch=='\n'|| ch=='\''|| ch == '&'||ch=='('||ch==')'||ch=='-')
            {
                spelling += ch ;        //// *\n[]
                start_ch_count++;
                nextch() ;
                if(ch == '\n')
                {
                    spelling += ch ;    //// ....... \n
                    start_ch_count++;
                    
                    nextch();//white space      //// ...... \n[]
                    spelling += ch ;
                    start_ch_count++;
                    
                    nextch(); // '*'        //// ...... \n *
                    temp = ch;        //spelling+=ch;
                    start_ch_count++;
                    nextch(); // '/'
                    if(ch=='/')
                    {
                        start_ch_count++;
                        nextch();
                        return new_token(tk_adhoc_comment,spelling,start_line,start_column,start_ch_count) ;
                    }
                    spelling+=temp;
                }
            }
            return new_token(tk_adhoc_comment,spelling,start_line,start_column,start_ch_count) ;
        }
        return new_token(kind,spelling,start_line,start_column,start_ch_count) ;
    }
    
    static Token parse_string(TokenKind kind,string spelling,int line_num, int column_num,int ch_count)
    {
        start_ch_count=1;//"\""
        spelling="";
        nextch();           //character
        
        while (isalnum(ch) || ch == ':' || ch == ' '||ch=='.' ||ch==',')
        {
            spelling += ch ;
            start_ch_count++;
            nextch() ;
            if(ch=='"')
            {
                start_ch_count++;
                nextch() ;
                if(ch==')')
                return new_token(tk_string,spelling,start_line,start_column,start_ch_count) ;
            }
        }
        return new_token(tk_string,spelling,start_line,start_column,start_ch_count) ;
    }
    
    /*================================================================================================================*/
    /*                                                     NEXT_TOKEN                                                 */
    /*================================================================================================================*/

    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned
    Token next_token()
    {
        // if this is the first call of next_token() call initialise()
        if ( line_num == 0 ) initialise() ;

        // remember start of next token
        start_line = line_num ;
        start_column = column_num ;
        start_ch_count = ch_count ;

        switch(ch)      // ch is always the next char to read
        {
                        // add additional case labels here for characters that can start tokens
                        // call a parse_*() function to complete and return each kind of token
                        // this should follow the style used in the workshops
                        // but remember that the token grammar is different in this assignment
        case ' ':       return parse_wspace(tk_space," ",start_line,start_column,start_ch_count) ;
        case '\n':      return parse_wspace(tk_newline,"\n",start_line,start_column,start_ch_count) ;
        case '\t':      return parse_wspace(tk_space," ",start_line,start_column,start_ch_count);

        case 'a'...'z': return parse_identifier() ;
        case 'A'...'Z': return parse_identifier() ;
        case '_':       return parse_identifier() ;    
        case '$':       return parse_identifier() ;

        case'0'...'9':  return parse_number();
        
        case '@':       return parse_symbol(tk_at, "@",start_line,start_column,start_ch_count) ;
        case ';':       return parse_symbol(tk_semi, ";",start_line,start_column,start_ch_count) ;
        case ':':       return parse_symbol(tk_colon, ":",start_line,start_column,start_ch_count) ;
        case '!':       return parse_symbol(tk_not,  "!",start_line,start_column,start_ch_count) ;
        case ',':       return parse_symbol(tk_comma, ",",start_line,start_column,start_ch_count) ;
        case '.':       return parse_symbol(tk_stop,".",start_line,start_column,start_ch_count) ;
        case '{':       return parse_symbol(tk_lcb,"{",start_line,start_column,start_ch_count) ;
        case '}':       return parse_symbol(tk_rcb,"}",start_line,start_column,start_ch_count) ;
        case '(':       return parse_symbol(tk_lrb, "(",start_line,start_column,start_ch_count) ;
        case ')':       return parse_symbol(tk_rrb, ")",start_line,start_column,start_ch_count) ;
        case '[':       return parse_symbol(tk_lsb, "[",start_line,start_column,start_ch_count);
        case ']':       return parse_symbol(tk_rsb, "]",start_line,start_column,start_ch_count);
        case '/':       return parse_symbol(tk_div, "/",start_line,start_column,start_ch_count);
        
        case '"':       return parse_string(tk_string, "\"",start_line,start_column,start_ch_count);
        
                        // if EOF or anything unexpected - return eoi
        default:        return parse_eoi() ;
        }
    }
}
