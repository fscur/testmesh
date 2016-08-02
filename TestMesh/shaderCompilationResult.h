#pragma once
#include <GL\glew.h>

#include <string>

class shaderCompilationResult
{
private:
    bool _succeeded;
    GLuint _shaderId;
    std::string _fileName;
    std::string _resultMessage;

public:
    shaderCompilationResult(bool succeeded, GLuint shaderId, std::string fileName, std::string resultMessage) :
        _succeeded(succeeded),
        _shaderId(shaderId),
        _fileName(fileName),
        _resultMessage(resultMessage)
    {
    }

    std::string toString()
    {
        auto message = std::string();

        message.append(_fileName);
        message.append(" id: " + std::to_string(_shaderId));

        if (_succeeded)
        {
            message.append(" succeeded compilation");
        }
        else
        {
            message.append(" failed to the ground \\m/");
            message.append("\n");
            message.append("Error message: ");
            message.append("\n");
            message.append(_resultMessage);
        }

        message.append("\n");

        return message;
    }

    bool getSucceeded() { return _succeeded; }
};