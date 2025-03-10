#include "../types.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

struct Field {
  uint32_t id;
  bool isRepeated;
  std::string name;
  user::UserTypes type;
  bool optional;
  struct ProtoMessage *parent =
      NULL; // Parent of the nested field, if it is nested
};

struct ProtoMessage {
  std::string name;
  std::vector<Field> fields;
};

void printProtoMessage(ProtoMessage m) {
  std::cout << m.name << std::endl;
  for (Field f : m.fields) {
    std::cout << f.id << " " << f.name << " "
              << user::TypeHelper::typeToStr(f.type)
              << (f.type == user::UserTypes::TYPE_MESSAGE
                      ? "(" + f.parent->name + ")"
                      : "")
              << " " << (f.isRepeated ? "repeated" : "") << std::endl;
  }
}

void createProtoMessage(
    ProtoMessage *msgPtr, std::vector<std::string> lines,
    std::unordered_map<std::string, ProtoMessage> &allMsgs) {
  for (std::string l : lines) {
    std::istringstream iss(l);
    std::string word;

    Field f = Field();

    // Take care of optional

    iss >> word;
    if (word == "repeated") {
      f.isRepeated = true;
      iss >> word;
    }

    /*
     * Set the type here
     * Handle for nested messages
     */
    if (allMsgs.find(word) != allMsgs.end()) {
      f.parent = &(allMsgs[word]);
      f.type = user::TypeHelper::stringToEnum("message"); // Message Type
    } else {
      f.type = user::TypeHelper::stringToEnum(word);
    }

    iss >> word;
    f.name = word;

    iss >> word; // '=' symbol

    iss >> word;
    uint32_t id_ = 0;
    for (char c : word) {
      if (c == ';')
        break;
      id_ *= 10;
      id_ += (c - '0');
    }
    f.id = id_;
    msgPtr->fields.push_back(f);
  }
}

std::unordered_map<std::string, ProtoMessage>
parseProtoFile(const std::string &filePath) {
  std::unordered_map<std::string, ProtoMessage> allMsgs;
  std::ifstream file(filePath);
  std::string line;
  std::stack<std::string> msgContent;
  std::stack<std::string> msgNames;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "message") {
      std::string name;
      iss >> name;
      msgNames.push(name);
      msgContent.push("{");
    } else if (token == "}") // End of message, get lines from stack
    {
      std::vector<std::string> msgLines;
      while (msgContent.top() != "{") {
        std::string s = msgContent.top();
        // msgLines.push_back(s);
        msgLines.insert(msgLines.begin(), s);
        msgContent.pop();
      }
      msgContent.pop();
      ProtoMessage *currentMessage = new ProtoMessage();
      currentMessage->name = msgNames.top();
      msgNames.pop();
      createProtoMessage(currentMessage, msgLines, allMsgs);
      msgLines.clear();
      allMsgs[currentMessage->name] = *currentMessage;
    } else if (!msgContent.empty() && line.size() > 0) {
      msgContent.push(line);
    }
  }

  file.close();

  return allMsgs;
}
