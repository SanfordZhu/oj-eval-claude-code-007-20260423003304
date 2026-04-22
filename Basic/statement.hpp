/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

    Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

    virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

    virtual void execute(EvalState &state, Program &program) = 0;

};


/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

/*
 * Class: RemStatement
 * -------------------
 * This subclass represents a REM statement.
 */

class RemStatement : public Statement {
public:
    RemStatement(const std::string& comment);
    virtual void execute(EvalState &state, Program &program);
private:
    std::string comment;
};

/*
 * Class: LetStatement
 * -------------------
 * This subclass represents a LET statement.
 */

class LetStatement : public Statement {
public:
    LetStatement(const std::string& var, Expression *exp);
    virtual ~LetStatement();
    virtual void execute(EvalState &state, Program &program);
private:
    std::string var;
    Expression *exp;
};

/*
 * Class: PrintStatement
 * ---------------------
 * This subclass represents a PRINT statement.
 */

class PrintStatement : public Statement {
public:
    PrintStatement(Expression *exp);
    virtual ~PrintStatement();
    virtual void execute(EvalState &state, Program &program);
private:
    Expression *exp;
};

/*
 * Class: InputStatement
 * ---------------------
 * This subclass represents an INPUT statement.
 */

class InputStatement : public Statement {
public:
    InputStatement(const std::string& var);
    virtual void execute(EvalState &state, Program &program);
private:
    std::string var;
};

/*
 * Class: EndStatement
 * -------------------
 * This subclass represents an END statement.
 */

class EndStatement : public Statement {
public:
    EndStatement();
    virtual void execute(EvalState &state, Program &program);
};

/*
 * Class: GotoStatement
 * --------------------
 * This subclass represents a GOTO statement.
 */

class GotoStatement : public Statement {
public:
    GotoStatement(int lineNumber);
    virtual void execute(EvalState &state, Program &program);
private:
    int lineNumber;
};

/*
 * Class: IfStatement
 * ------------------
 * This subclass represents an IF statement.
 */

class IfStatement : public Statement {
public:
    IfStatement(Expression *lhs, const std::string& op, Expression *rhs, int targetLine);
    virtual ~IfStatement();
    virtual void execute(EvalState &state, Program &program);
private:
    Expression *lhs;
    std::string op;
    Expression *rhs;
    int targetLine;
};

/*
 * Class: RunStatement
 * -------------------
 * This subclass represents a RUN statement.
 */

class RunStatement : public Statement {
public:
    RunStatement();
    virtual void execute(EvalState &state, Program &program);
};

/*
 * Class: ListStatement
 * --------------------
 * This subclass represents a LIST statement.
 */

class ListStatement : public Statement {
public:
    ListStatement();
    virtual void execute(EvalState &state, Program &program);
};

/*
 * Class: ClearStatement
 * ---------------------
 * This subclass represents a CLEAR statement.
 */

class ClearStatement : public Statement {
public:
    ClearStatement();
    virtual void execute(EvalState &state, Program &program);
};

/*
 * Class: QuitStatement
 * --------------------
 * This subclass represents a QUIT statement.
 */

class QuitStatement : public Statement {
public:
    QuitStatement();
    virtual void execute(EvalState &state, Program &program);
};

#endif
