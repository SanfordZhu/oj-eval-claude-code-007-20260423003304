/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

Statement::Statement() = default;

Statement::~Statement() = default;

/* Implementation of RemStatement */

RemStatement::RemStatement(const std::string& comment) : comment(comment) {}

void RemStatement::execute(EvalState &state, Program &program) {
    // REM statements do nothing - they're just comments
}

/* Implementation of LetStatement */

LetStatement::LetStatement(const std::string& var, Expression *exp) : var(var), exp(exp) {}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    state.setValue(var, value);
}

/* Implementation of PrintStatement */

PrintStatement::PrintStatement(Expression *exp) : exp(exp) {}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    std::cout << value << std::endl;
}

/* Implementation of InputStatement */

InputStatement::InputStatement(const std::string& var) : var(var) {}

void InputStatement::execute(EvalState &state, Program &program) {
    int value;
    std::cin >> value;
    state.setValue(var, value);
}

/* Implementation of EndStatement */

EndStatement::EndStatement() {}

void EndStatement::execute(EvalState &state, Program &program) {
    // END statement terminates program execution
    // We'll handle this by throwing a special exception or setting a flag
    // For now, we'll just return normally and let the main loop handle it
}

/* Implementation of GotoStatement */

GotoStatement::GotoStatement(int lineNumber) : lineNumber(lineNumber) {}

void GotoStatement::execute(EvalState &state, Program &program) {
    // GOTO changes the program counter
    // We'll need to modify the execution logic to handle this
    // For now, store the target line in the state
    state.setCurrentLine(lineNumber);
}

/* Implementation of IfStatement */

IfStatement::IfStatement(Expression *lhs, const std::string& op, Expression *rhs, int targetLine)
    : lhs(lhs), op(op), rhs(rhs), targetLine(targetLine) {}

IfStatement::~IfStatement() {
    delete lhs;
    delete rhs;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int leftVal = lhs->eval(state);
    int rightVal = rhs->eval(state);
    bool condition = false;

    if (op == "=") {
        condition = (leftVal == rightVal);
    } else if (op == "<>") {
        condition = (leftVal != rightVal);
    } else if (op == "<") {
        condition = (leftVal < rightVal);
    } else if (op == "<=") {
        condition = (leftVal <= rightVal);
    } else if (op == ">") {
        condition = (leftVal > rightVal);
    } else if (op == ">=") {
        condition = (leftVal >= rightVal);
    } else {
        error("Invalid comparison operator: " + op);
    }

    if (condition) {
        state.setCurrentLine(targetLine);
    }
}

/* Implementation of RunStatement */

RunStatement::RunStatement() {}

void RunStatement::execute(EvalState &state, Program &program) {
    // RUN executes the program from the beginning
    int currentLine = program.getFirstLineNumber();
    state.clearCurrentLine();

    while (currentLine != -1) {
        Statement *stmt = program.getParsedStatement(currentLine);
        if (stmt == nullptr) {
            // Parse the statement if not already parsed
            std::string line = program.getSourceLine(currentLine);
            TokenScanner scanner;
            scanner.ignoreWhitespace();
            scanner.scanNumbers();
            scanner.setInput(line);

            // Skip line number
            scanner.nextToken();

            // Parse statement
            std::string cmd = scanner.nextToken();
            if (cmd == "REM") {
                std::string comment = "";
                while (scanner.hasMoreTokens()) {
                    comment += scanner.nextToken();
                    if (scanner.hasMoreTokens()) comment += " ";
                }
                stmt = new RemStatement(comment);
            } else if (cmd == "LET") {
                std::string var = scanner.nextToken();
                scanner.verifyToken("=");
                Expression *exp = parseExp(scanner);
                stmt = new LetStatement(var, exp);
            } else if (cmd == "PRINT") {
                Expression *exp = parseExp(scanner);
                stmt = new PrintStatement(exp);
            } else if (cmd == "INPUT") {
                std::string var = scanner.nextToken();
                stmt = new InputStatement(var);
            } else if (cmd == "END") {
                stmt = new EndStatement();
            } else if (cmd == "GOTO") {
                int lineNum = stringToInteger(scanner.nextToken());
                stmt = new GotoStatement(lineNum);
            } else if (cmd == "IF") {
                Expression *lhs = parseExp(scanner);
                std::string op = scanner.nextToken();
                Expression *rhs = parseExp(scanner);
                scanner.verifyToken("THEN");
                int targetLine = stringToInteger(scanner.nextToken());
                stmt = new IfStatement(lhs, op, rhs, targetLine);
            }

            program.setParsedStatement(currentLine, stmt);
        }

        // Store the next line before executing (in case of GOTO or modifications)
        int nextLine = -1;
        if (!state.hasCurrentLine()) {
            nextLine = program.getNextLineNumber(currentLine);
        }

        // Execute the statement
        stmt->execute(state, program);

        // Check if we need to jump to a different line
        if (state.hasCurrentLine()) {
            currentLine = state.getCurrentLine();
            state.clearCurrentLine();
        } else {
            // Move to next line
            currentLine = nextLine;
        }

        // Check for END statement
        if (dynamic_cast<EndStatement*>(stmt) != nullptr) {
            break;
        }
    }
}

/* Implementation of ListStatement */

ListStatement::ListStatement() {}

void ListStatement::execute(EvalState &state, Program &program) {
    int lineNumber = program.getFirstLineNumber();
    while (lineNumber != -1) {
        std::cout << program.getSourceLine(lineNumber) << std::endl;
        lineNumber = program.getNextLineNumber(lineNumber);
    }
}

/* Implementation of ClearStatement */

ClearStatement::ClearStatement() {}

void ClearStatement::execute(EvalState &state, Program &program) {
    program.clear();
    state.Clear();
}

/* Implementation of QuitStatement */

QuitStatement::QuitStatement() {}

void QuitStatement::execute(EvalState &state, Program &program) {
    exit(0);
}
