/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <cstdlib>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "statement.hpp"
#include "evalstate.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            if (!getline(std::cin, input)) {
                // EOF reached - exit cleanly
                break;
            }
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    // Check if this is a numbered line
    if (!scanner.hasMoreTokens()) {
        return;  // Empty line
    }

    // Peek at the first token without consuming it
    std::string firstToken = scanner.nextToken();
    scanner.saveToken(firstToken);
    TokenType firstTokenType = scanner.getTokenType(firstToken);

    if (firstTokenType == NUMBER) {
        // This is a numbered program line
        std::string lineNumStr = scanner.nextToken();
        int lineNumber = stringToInteger(lineNumStr);

        // Check if there's more content after the line number
        if (scanner.hasMoreTokens()) {
            // Add or replace the line
            program.addSourceLine(lineNumber, line);

            // Parse the statement if possible
            std::string cmd = scanner.nextToken();
            Statement *stmt = nullptr;

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
                int targetLine = stringToInteger(scanner.nextToken());
                stmt = new GotoStatement(targetLine);
            } else if (cmd == "IF") {
                // Parse left expression
                Expression *lhs = nullptr;
                std::string lhsStr = "";
                while (scanner.hasMoreTokens()) {
                    std::string token = scanner.nextToken();
                    if (token == "=" || token == "<>" || token == "<" || token == "<=" || token == ">" || token == ">=") {
                        scanner.saveToken(token);
                        break;
                    }
                    if (!lhsStr.empty()) lhsStr += " ";
                    lhsStr += token;
                }
                if (!lhsStr.empty()) {
                    TokenScanner lhsScanner;
                    lhsScanner.ignoreWhitespace();
                    lhsScanner.scanNumbers();
                    lhsScanner.setInput(lhsStr);
                    lhs = parseExp(lhsScanner);
                }

                // Parse operator
                std::string op = scanner.nextToken();

                // Parse right expression
                Expression *rhs = nullptr;
                std::string rhsStr = "";
                while (scanner.hasMoreTokens()) {
                    std::string token = scanner.nextToken();
                    if (token == "THEN") {
                        scanner.saveToken(token);
                        break;
                    }
                    if (!rhsStr.empty()) rhsStr += " ";
                    rhsStr += token;
                }
                if (!rhsStr.empty()) {
                    TokenScanner rhsScanner;
                    rhsScanner.ignoreWhitespace();
                    rhsScanner.scanNumbers();
                    rhsScanner.setInput(rhsStr);
                    rhs = parseExp(rhsScanner);
                }

                scanner.verifyToken("THEN");
                int targetLine = stringToInteger(scanner.nextToken());
                stmt = new IfStatement(lhs, op, rhs, targetLine);
            } else {
                error("Invalid statement: " + cmd);
            }

            if (stmt != nullptr) {
                program.setParsedStatement(lineNumber, stmt);
            }
        } else {
            // Empty line number - remove the line
            program.removeSourceLine(lineNumber);
        }
    } else {
        // This is a direct command
        if (!scanner.hasMoreTokens()) {
            return;  // Empty line after number
        }
        std::string cmd = scanner.nextToken();

        if (cmd == "RUN") {
            RunStatement stmt;
            stmt.execute(state, program);
        } else if (cmd == "LIST") {
            ListStatement stmt;
            stmt.execute(state, program);
        } else if (cmd == "CLEAR") {
            ClearStatement stmt;
            stmt.execute(state, program);
        } else if (cmd == "QUIT") {
            QuitStatement stmt;
            stmt.execute(state, program);
        } else if (cmd == "HELP") {
            std::cout << "Available commands: RUN, LIST, CLEAR, QUIT, HELP" << std::endl;
        } else if (cmd == "REM") {
            // REM in direct mode does nothing
        } else if (cmd == "LET") {
            std::string var = scanner.nextToken();
            scanner.verifyToken("=");
            Expression *exp = parseExp(scanner);
            LetStatement stmt(var, exp);
            stmt.execute(state, program);
            delete exp;
        } else if (cmd == "PRINT") {
            Expression *exp = parseExp(scanner);
            PrintStatement stmt(exp);
            stmt.execute(state, program);
            delete exp;
        } else if (cmd == "INPUT") {
            std::string var = scanner.nextToken();
            InputStatement stmt(var);
            stmt.execute(state, program);
        } else if (cmd == "END") {
            // END in direct mode does nothing
        } else if (cmd == "GOTO") {
            error("GOTO can only be used in program mode");
        } else if (cmd == "IF") {
            error("IF can only be used in program mode");
        } else {
            error("Invalid command: " + cmd);
        }
    }
}

