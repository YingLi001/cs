//==========================
// Computer System
// Student: Ying Li
// id: your a1776903
// Semester:s1
// Year:2020
// Assignment 3
//==========================
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and generate the equivalent Hack Virtual Machine Code.
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate walk_* function to walk any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
void walk_class(ast t) ;
void walk_class_var_decs(ast t) ;
void walk_var_dec(ast t) ;
void walk_subr_decs(ast t) ;
void walk_subr(ast t) ;
void walk_constructor(ast t) ;
void walk_function(ast t) ;
void walk_method(ast t) ;
void walk_param_list(ast t) ;
void walk_subr_body(ast t) ;
void walk_var_decs(ast t) ;
void walk_statements(ast t) ;
void walk_statement(ast t) ;
void walk_let(ast t) ;
void walk_let_array(ast t) ;
void walk_if(ast t) ;
void walk_if_else(ast t) ;
void walk_while(ast t) ;
void walk_do(ast t) ;
void walk_return(ast t) ;
void walk_return_expr(ast t) ;
void walk_expr(ast t) ;
void walk_term(ast t) ;
void walk_int(ast t) ;
void walk_string(ast t) ;
void walk_bool(ast t) ;
void walk_null(ast t) ;
void walk_this(ast t) ;
void walk_unary_op(ast t) ;
void walk_var(ast t) ;
void walk_array_index(ast t) ;
void walk_call_as_function(ast t) ;
void walk_call_as_method(ast t) ;
void walk_subr_call(ast t) ;
void walk_expr_list(ast t) ;
void walk_infix_op(ast t) ;

// two methods to tranlate the infix operation and unary operation
static string translate_op(string op);
static string translate_uop(string uop);

// declare the global variables to record the class name, while loop number,
// if statement number and field variables number
string className ;
static int while_counter = 0 ;
static int if_counter = 0 ;
static int this_number = 0 ;

/**
 * walk class: record the class name and recursively call walk_class_var_decs and walk_subr_decs
 * @param t
 */
void walk_class(ast t)
{
    string myclassname = get_class_class_name(t) ;
    className = myclassname ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;

    walk_class_var_decs(var_decs) ;
    walk_subr_decs(subr_decs) ;
}

/**
 * using for loop to traverse the variable declarations
 * @param t
 */
void walk_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;

    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_class_var_decs(t,i)) ;
    }
}

// the variable declaration method, calculate the field variables here
void walk_var_dec(ast t)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    int offset = get_var_dec_offset(t) ;
    
    if(segment == "this")
    {
        this_number++ ;
    }
}

// traverse the subroutines in a class including constructor, method and function
void walk_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    
    for ( int i = 0 ; i < size ; i++ )
    {
        walk_subr(get_subr_decs(t,i)) ;
    }
}

// walk different kinds of subroutines
void walk_subr(ast t)
{
    ast subr = get_subr_subr(t) ;

    switch(ast_node_kind(subr))
    {
    case ast_constructor:
        walk_constructor(subr) ;
        break ;
    case ast_function:
        walk_function(subr) ;
        break ;
    case ast_method:
        walk_method(subr) ;
        break ;
    default:
        fatal_error(0,"Unexpected subroutine kind") ;
        break ;
    }
}

// subroutine case 1: constructor
void walk_constructor(ast t)
{
    // the global variables counting the while and if statements number should be become 0 in a new subroutine
    if_counter = 0 ;
    while_counter = 0 ;
    
    // get the correct value and print out the corresponding VM code
    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;
    ast var = get_subr_body_decs(subr_body) ;
    int var_number = size_of_var_decs(var) ;
    write_to_output("function "+className+"."+name+" "+to_string(var_number)+"\n") ;
    
    walk_param_list(param_list) ;
    
    // When creating an object by calling a constructor, the constructor's implementation
    // starts by allocating some memory with space for each field and making the address of the memory
    write_to_output("push constant "+to_string(this_number)+"\n") ;
    write_to_output("call Memory.alloc 1\n") ;
    write_to_output("pop pointer 0\n") ;
    
    walk_subr_body(subr_body) ;
}

// subroutine case 2: function
void walk_function(ast t)
{
    if_counter = 0 ;
    while_counter = 0 ;
    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;
    ast var = get_subr_body_decs(subr_body) ;
    int var_number = size_of_var_decs(var) ;
    write_to_output("function "+className+"."+name+" "+to_string(var_number)+"\n") ;
    
    walk_param_list(param_list) ;
    
    walk_subr_body(subr_body) ;
}

// subroutine case 3: method
void walk_method(ast t)
{
    if_counter = 0 ;
    while_counter = 0 ;
    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;
    ast var = get_subr_body_decs(subr_body) ;
    int var_number = size_of_var_decs(var) ;
    write_to_output("function "+className+"."+name+" "+to_string(var_number)+"\n") ;
    
    walk_param_list(param_list) ;
    
    // When a method is called, the first parameter is always a pointer to the object it is operating on.
    // So, the start address of the THIS segment is shown below
    write_to_output("push argument 0\n") ;
    write_to_output("pop pointer 0\n") ;
    
    walk_subr_body(subr_body) ;
}

// traverse the parameter list of the subroutines
void walk_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_param_list(t,i)) ;
    }
}

// traverse the variable declarations and statements in subroutines
void walk_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    walk_var_decs(decs) ;
    walk_statements(body) ;
}

// traverse the variable declarations in subroutines
void walk_var_decs(ast t)
{
    int ndecs = size_of_var_decs(t) ;

    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_var_decs(t,i)) ;
    }
}

// traverse the statements in subroutines
void walk_statements(ast t)
{
    int nstatements = size_of_statements(t) ;
    
    for ( int i = 0 ; i < nstatements ; i++ )
    {
        walk_statement(get_statements(t,i)) ;
    }
}

// different kinds of statements
void walk_statement(ast t)
{
    ast statement = get_statement_statement(t) ;

    switch(ast_node_kind(statement))
    {
    case ast_let:
        walk_let(statement) ;
        break ;
    case ast_let_array:
        walk_let_array(statement) ;
        break ;
    case ast_if:
        walk_if(statement) ;
        break ;
    case ast_if_else:
        walk_if_else(statement) ;
        break ;
    case ast_while:
        walk_while(statement) ;
        break ;
    case ast_do:
        walk_do(statement) ;
        break ;
    case ast_return:
        walk_return(statement) ;
        break ;
    case ast_return_expr:
        walk_return_expr(statement) ;
        break ;
    case ast_statements:
        walk_statements(statement) ;
        break ;
    default:
        fatal_error(0,"Unexpected statement kind") ;
        break ;
    }
}

// statement case 1: let
void walk_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;
    string var_segment = get_var_segment(var) ;
    int var_offset = get_var_offset(var) ;
    
    // comment lhs will make the life easier
//    walk_var(var) ;
    // rhs
    walk_expr(expr) ;
    
    write_to_output("pop "+var_segment+" " + to_string(var_offset)+"\n") ;
}

// statement case 2: let array
void walk_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;
    
    walk_expr(index) ;
    walk_var(var) ;
    
    write_to_output("add\n") ;
    
    walk_expr(expr) ;
    
    write_to_output("pop temp 0\n") ;
    write_to_output("pop pointer 1\n") ;
    write_to_output("push temp 0\n") ;
    write_to_output("pop that 0\n") ;
}

// statement case 3: if
void walk_if(ast t)
{
    string label = to_string(if_counter++) ;
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;
    
    walk_expr(condition) ;
    
    write_to_output("if-goto IF_TRUE"+label+"\n") ;
    write_to_output("goto IF_FALSE"+label+"\n") ;
    write_to_output("label IF_TRUE"+label+"\n") ;
    
    walk_statements(if_true) ;
    
    write_to_output("label IF_FALSE"+label+"\n") ;
}

// statement case 4: if else
void walk_if_else(ast t)
{
    string label = to_string(if_counter++) ;
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;

    walk_expr(condition) ;
    
    write_to_output("if-goto IF_TRUE"+label+"\n") ;
    write_to_output("goto IF_FALSE"+label+"\n") ;
    write_to_output("label IF_TRUE"+label+"\n") ;
    
    walk_statements(if_true) ;
    
    write_to_output("goto IF_END"+label+"\n") ;
    write_to_output("label IF_FALSE"+label+"\n") ;
    
    walk_statements(if_false) ;
    
    write_to_output("label IF_END"+label+"\n") ;
}

// statement case 5: while
void walk_while(ast t)
{
    string label = to_string(while_counter++) ;
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;
    
    write_to_output("label WHILE_EXP"+label+"\n") ;
    
    walk_expr(condition) ;
    
    write_to_output("not\n") ;
    write_to_output("if-goto WHILE_END"+label+"\n") ;
    
    walk_statements(body) ;
    
    write_to_output("goto WHILE_EXP"+label+"\n") ;
    write_to_output("label WHILE_END"+label+"\n") ;
}

// statement case 6: do including call as function and call as method
void walk_do(ast t)
{
    ast call = get_do_call(t) ;

    switch(ast_node_kind(call))
    {
    case ast_call_as_function:
        walk_call_as_function(call) ;
        break ;
    case ast_call_as_method:
        walk_call_as_method(call) ;
        break ;
    default:
        fatal_error(0,"Unexpected call kind") ;
        break ;
    }
    
    write_to_output("pop temp 0\n") ;
}

// statement case 7: return
void walk_return(ast t)
{
    // always finish with return so the VM code is a complete void function
    write_to_output("push constant 0\n") ;
    write_to_output("return\n") ;
}

// statement case 8: return experience
void walk_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;
    
    walk_expr(expr) ;
    
    write_to_output("return\n") ;
}

// traveser the experiences including term and infix operation
void walk_expr(ast t)
{
    // using a vector to store all operations and print out them from the last one to the first one
    vector<string> operation;
    int term_ops = size_of_expr(t) ;
    
    for ( int i = 0 ; i < term_ops ; i++ )
    {
        ast term_op = get_expr(t,i) ;

        if ( i % 2 == 0 )
        {
            walk_term(term_op) ;
        }
        else
        {
            walk_infix_op(term_op) ;
            
            string op = get_infix_op_op(term_op) ;
            string result = translate_op(op) ;
            operation.push_back(result) ;
        }
    }
    
    for ( int j = operation.size()-1 ; j >= 0 ; j-- )
    {
        write_to_output(operation[j]+"\n") ;
    }
}

// different kinds of term
void walk_term(ast t)
{
    ast term = get_term_term(t) ;

    switch(ast_node_kind(term))
    {
    case ast_int:
        walk_int(term) ;
        break ;
    case ast_string:
        walk_string(term) ;
        break ;
    case ast_bool:
        walk_bool(term) ;
        break ;
    case ast_null:
        walk_null(term) ;
        break ;
    case ast_this:
        walk_this(term) ;
        break ;
    case ast_expr:
        walk_expr(term) ;
        break ;
    case ast_unary_op:
        walk_unary_op(term) ;
        break ;
    case ast_var:
        walk_var(term) ;
        break ;
    case ast_array_index:
        walk_array_index(term) ;
        break ;
    case ast_call_as_function:
        walk_call_as_function(term) ;
        break ;
    case ast_call_as_method:
        walk_call_as_method(term) ;
        break ;
    default:
        fatal_error(0,"Unexpected term kind") ;
        break ;
    }
}

/**
 * term case 1: integer
 * According to the description on myUni:
 * Integer constant values should all be in the range 0 to +32,767. However, the ast_int node will
 * accept any value in the range -32,768 to +32,767. Where a negative value is found, the absolute
 * integer value must be pushed onto the stack and followed by a neg command.
 * In the special case of, -32,768, the integer constant 16,384 must be pushed onto the stack twice
 * followed by an add command.
 * @param t
 */
void walk_int(ast t)
{
    int _constant = get_int_constant(t) ;
    
    if(_constant < 0)
    {
        if(_constant == -32768)
        {
            write_to_output("push constant 16384\n") ;
            write_to_output("push constant 16384\n") ;
            write_to_output("neg\n") ;
        }
        
        write_to_output("push constant "+to_string(_constant)+"\n") ;
        write_to_output("neg\n") ;
    }
    else
    {
        write_to_output("push constant "+to_string(_constant)+"\n") ;
    }
}

/**
 * term case 2: string
 * String literals must be created using the String.new() constructor
 * and then adding each character using the String.appendChar() method one at a time.
 * @param t
 */
void walk_string(ast t)
{
    string _constant = get_string_constant(t) ;
    int size = _constant.size() ;
    
    write_to_output("push constant "+to_string(size)+"\n") ;
    write_to_output("call String.new 1\n") ;
    
    for ( int i = 0; i < _constant.size(); ++i )
    {
        write_to_output("push constant "+to_string((int)_constant[i])+"\n") ;
        write_to_output("call String.appendChar 2\n") ;
    }
}

// term case 3: boolean
void walk_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    
    //true => false + not
    if(_constant)
    {
        write_to_output("push constant 0\n") ;
        write_to_output("not\n") ;
    }
    // false => false
    else
    {
        write_to_output("push constant 0\n") ;
    }
}

// term case 4: null => push constant 0
void walk_null(ast t)
{
    write_to_output("push constant 0\n") ;
}

// term case 5: this => pointer 0
void walk_this(ast t)
{
    write_to_output("push pointer 0\n") ;
}

// term case 6: unary operation
void walk_unary_op(ast t)
{
    string uop = get_unary_op_op(t) ;
    ast term = get_unary_op_term(t) ;
    string result = translate_uop(uop) ;
    
    walk_term(term) ;
    
    write_to_output(result+"\n") ;
}

// term case 7: variable
void walk_var(ast t)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    int offset = get_var_offset(t) ;
    
    write_to_output("push "+segment+" "+to_string(offset)+"\n") ;
}

// term case 8: array index
void walk_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;
    
    //here i changed the order of index and var
    walk_expr(index) ;
    walk_var(var) ;
    
    write_to_output("add\npop pointer 1\npush that 0\n") ;
}

// term case 9: call_as_function
void walk_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;
    string subr_name = get_subr_call_subr_name(subr_call) ;
    ast expr_list = get_subr_call_expr_list(subr_call) ;
    int number = size_of_expr_list(expr_list) ;
    
    walk_subr_call(subr_call) ;
    
    write_to_output("call "+class_name+"."+subr_name+" "+to_string(number)+"\n") ;
}

// term case 10: call_as_method
void walk_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;
    
    string subr_name = get_subr_call_subr_name(subr_call) ;
    ast expr_list = get_subr_call_expr_list(subr_call) ;
    int number = size_of_expr_list(expr_list) ;
    
    // the first visible parameter is argument 1 coz the first one is the object
    int method_argument_number = 1 + number ;
    
    switch(ast_node_kind(var))
    {
    case ast_this:
        walk_this(var) ;
        break ;
    case ast_var:
        walk_var(var) ;
        break ;
    default:
        fatal_error(0,"Expected var or this") ;
        break ;
    }
    
    // walk the subroutines
    walk_subr_call(subr_call) ;
    
    write_to_output("call "+class_name+"."+subr_name+" "+to_string(method_argument_number)+"\n") ;
}

void walk_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;

    // walk the expr_list
    walk_expr_list(expr_list) ;
}

// traverse the expressions
void walk_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;

    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        walk_expr(get_expr_list(t,i)) ;
    }
}

void walk_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;
}

// translate the unary operation and return the corresponding string
static string translate_uop(string uop)
{
    if( uop == "~" ) return "not" ;
    if( uop == "-" ) return "neg" ;
    
    fatal_error(-1,"translate_op passed unknown op: " + uop + "\n") ;
    
    return uop ;
}

// translate the infix operation and return the corresponding string
static string translate_op(string op)
{
    if( op == "+" ) return "add" ;
    if( op == "-" ) return "sub" ;
    if( op == "*" ) return "call Math.multiply 2" ;
    if( op == "/" ) return "call Math.divide 2" ;
    if( op == "&" ) return "and" ;
    if( op == "|" ) return "or" ;
    if( op == "<" ) return "lt" ;
    if( op == ">" ) return "gt" ;
    if( op == "=" ) return "eq" ;
    
    fatal_error(-1,"translate_op passed unknown op: " + op + "\n") ;
    
    return op ;
}

// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and print VM code
    walk_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

