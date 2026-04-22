/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"
#include "Utils/error.hpp"



Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    // Delete all parsed statements
    for (auto& pair : parsedStatements) {
        delete pair.second;
    }

    // Clear all containers
    sourceLines.clear();
    parsedStatements.clear();
    lineNumbers.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // If line already exists, delete its parsed statement
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }

    // Add or update the source line
    sourceLines[lineNumber] = line;
    lineNumbers.insert(lineNumber);
}

void Program::removeSourceLine(int lineNumber) {
    // Remove source line if it exists
    if (sourceLines.find(lineNumber) != sourceLines.end()) {
        sourceLines.erase(lineNumber);
        lineNumbers.erase(lineNumber);
    }

    // Remove parsed statement if it exists
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it != sourceLines.end()) {
        return it->second;
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Check if line exists
    if (sourceLines.find(lineNumber) == sourceLines.end()) {
        error("Line number does not exist in program");
    }

    // Delete existing statement if present
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
    }

    // Set the new statement
    parsedStatements[lineNumber] = stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
    auto it = parsedStatements.find(lineNumber);
    if (it != parsedStatements.end()) {
        return it->second;
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (lineNumbers.empty()) {
        return -1;
    }
    return *lineNumbers.begin();
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = lineNumbers.find(lineNumber);
    if (it == lineNumbers.end()) {
        error("Line number does not exist in program");
    }

    ++it;
    if (it == lineNumbers.end()) {
        return -1;
    }
    return *it;
}

//more func to add
//todo


