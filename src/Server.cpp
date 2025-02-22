#include "Server.hpp"
#include "Logger.hpp"
#include "Protocol.hpp"
#include "papi.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
Server::Server(int port) : port(port) {
  serverSocket = Protocol::createSocket(port);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port);
}

void Server::start() {
  LOG_INFO("Starting server on port " + std::to_string(port));
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    LOG_ERROR("Failed to bind server socket");
    return;
  }
  if (listen(serverSocket, 5) < 0) {
    LOG_ERROR("Failed to listen on server socket");
    return;
  }
  LOG_INFO("Server started on port " + std::to_string(port));

  while (true) {
    LOG_INFO("Waiting for client connection");
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0) {
      LOG_ERROR("Failed to accept client connection");
      continue;
    }
    LOG_INFO("Client connected");
    std::thread(&Server::handleClient, this, clientSocket).detach();
  }
}
void Server::handleClient(int clientSocket) {
  LOG_INFO("Handling client");
  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    LOG_ERROR("PAPI thread initialization error!");
    throw std::runtime_error("PAPI thread initialization error!");
  }
  while (true) {
    std::string data;
    char buffer[1024];
    uint64_t dataSize;
    char dbuffer[8];
    int bytesRead = 0;
    int totalBytesRead = 0;

    while (totalBytesRead < sizeof(dataSize)) {
      bytesRead = recv(clientSocket, dbuffer + totalBytesRead,
                       sizeof(dataSize) - totalBytesRead, 0);
      if (bytesRead < 0) {
        LOG_ERROR("Failed to read data size from client bytesRead: " +
                  std::to_string(bytesRead));
        perror("recv");
        goto end;
      }
      if (bytesRead == 0)
        goto end;
      totalBytesRead += bytesRead;
    }
    if (totalBytesRead == sizeof(dataSize)) {
      std::memcpy(&dataSize, dbuffer, 8);
      dataSize = be64toh(dataSize);
    }

    totalBytesRead = 0;
    while (totalBytesRead < dataSize) {
      int bytesToRead = std::min(
          sizeof(buffer), static_cast<size_t>(dataSize - totalBytesRead));
      bytesRead = recv(clientSocket, buffer, bytesToRead, 0);
      if (bytesRead < 0) {
        LOG_ERROR("Failed to read from client");
        perror("recv");
        goto end;
      }
      if (bytesRead == 0)
        goto end;
      data.append(buffer, bytesRead);
      totalBytesRead += bytesRead;
    }

    // LOG_INFO("Received: " + data);
    processCommand(data, clientSocket);
    memset(buffer, 0, 1024);
  }
end:
  LOG_INFO("Closing client connection");
  close(clientSocket);
}
#include "Protocol.hpp"
#include <sstream>

void Server::processCommand(const std::string &command, int clientSocket) {
  // LOG_INFO("Processing command: " + command);
  Request msg = Protocol::deserialize(command);
  LOG_INFO("Deserialized message" + std::to_string(static_cast<int>(msg.type)));
  Response response;
  response.clientSocket = clientSocket;
  if (msg.type == Request::Type::INVALID) {
    response.type = Response::Type::RESPONSE;
    response.key = "Error";
    int result = Protocol::serializeAndSend(response);
    LOG_INFO("Sent response to client" + std::to_string(result));
    return;
  }
  response.key = msg.key;

  switch (msg.type) {
  case Request::Type::SET_FIELDS: {
    store.insert(msg.key);
    bool success = store.setFields(msg.key, msg.fields);
    if (success) {
      response.type = Response::Type::SET_FIELDS;
    } else {
      response.type = Response::Type::RESPONSE;
      response.key = "Error";
    }
    break;
  }

  case Request::Type::GETALL: {
    response.type = Response::Type::GETALL;
    response.fields = store.getFields(msg.key);
    // check the size of the fields
    for (const auto &[field, value] : response.fields) {
      if (value == nullptr) {
        response.type = Response::Type::RESPONSE;
        response.key = "Error";
        break;
      }
      LOG_INFO("Field: " + field +
               " ValueSize: " + std::to_string(value->size()));
    }

    if (response.fields.empty()) {
      response.type = Response::Type::RESPONSE;
      response.key = "Error";
    }
    break;
  }

  case Request::Type::GET_FIELDS: {
    response.type = Response::Type::GET_FIELDS;
    response.fields = store.getSelectedFields(msg.key, msg.fieldNames);
    if (response.fields.empty() ||
        response.fields.size() != msg.fieldNames.size()) {
      response.type = Response::Type::RESPONSE;
      response.key = "Error";
    }
    break;
  }

  default:
    response.type = Response::Type::RESPONSE;
    break;
  }
  LOG_INFO("Sending response to client");
  int result = Protocol::serializeAndSend(response);
  LOG_INFO("Sent response to client " + std::to_string(result) + "bytes");
}
