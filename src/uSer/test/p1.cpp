#include "user.hpp"
#include <thread>


using namespace std;
using namespace user;

net::common::Address server_address("127.0.0.1", 8085);

void server() {
  //	cout << "Server Starting..." << endl;
  net::TCPServer server(server_address, 5);
  assert(server.StartListen());
  // cout << "Waiting for request...";
  net::TCPConn *conn = server.GetNewConn();
  // cout << "Got connection\n";
  size_t size = user::net::common::BUFFER_SIZE;
  // char buf[size] = {0};
  char *buf = (char *)malloc(size);
  // while (true) {
      
  //   {
  //     bzero(buf, size);
  //     cout << "Waiting for read..\n";
  //     ssize_t bytes = conn->Read(buf, size);
  //     cout << "Read " << bytes << std::endl;
  //     Deserialiser *de_obj =
  //         InitMessage::getDeserialiserPtr("Person", buf, size);
  //     std::optional<int> a;
  //     a = de_obj->get<int32_t>("age");
  //     std::optional name = de_obj->getString("name");
  //     cout << "age: " << (a.has_value() ? to_string(*a) : "NULL") << endl;
  //     cout << "name: " << (name.has_value() ? *name : "NULL") << endl;
  //   }
    
    // {
    //   bzero(buf, size);
    //   cout << "Waiting for read..\n";
    //   ssize_t bytes = conn->Read(buf, size);
    //   cout << "Read " << bytes << std::endl;
    //   Deserialiser *de_obj =
    //       InitMessage::getDeserialiserPtr("Persons", buf, size);

    //   std::optional<int> a_size = 0, b_size = 0;
    //   a_size = de_obj->size("age"), b_size = de_obj->size("name");
    //   cout << "a_size: " << (a_size.has_value() ? to_string(*a_size) : "NULL")
    //        << endl;
    //   cout << "b_size: " << (b_size.has_value() ? to_string(*b_size) : "NULL")
    //        << endl;
    //   cout << "age: [";
    //   for (uint32_t i = 0; i < *a_size; i++) {
    //     std::optional<int32_t> val = de_obj->get<int32_t>("age", i);
    //     std::cout << *val << " ";
    //   }
    //   cout << "]" << endl;
    //   cout << "name: [";
    //   for (uint32_t i = 0; i < *b_size; i++) {
    //     std::optional<byte_t> val = de_obj->getByte("name", i);
    //     std::cout << "\"" << string(val->first, val->first + val->second)
    //               << "\" ";
    //   }
    //   cout << "]" << endl;
  //   // }
  //   break;
  // }
}


void client() {
  // // ssize_t bytes = 0;
  // net::TCPClient conn(server_address);
  // // cout << "Connecting...\n";
  // assert(conn.Connect());
  // cout << "Connection Done\n";
  
  // {

  //   user::Message *m1 = user::InitMessage::allocateMessage("Person");
  //   assert(m1 != nullptr);
  //   int age = 26;
  //   m1->setVal("name", "Siddharth");
  //   m1->setVal("age", age);

  //   auto bytes = m1->serializeAndWrite(&conn, SerializationMethod::COPY);
  //   cout << "Wrote " << bytes << " bytes." << std::endl;
  // }
  
  // // {
  // //   user::Message *m2 = user::InitMessage::allocateMessage("Persons");
  // //   assert(m2 != nullptr);
  // //   int age1 = 26, age2 = 36, age3 = 73;
  // //   m2->addVal("name", "Siddharth");
  // //   m2->addVal("name", "Siddharthhh");
  // //   m2->addVal("name", "Siddharthhhhhhhhhh");
  // //   m2->addVal("age", &age1);
  // //   m2->addVal("age", &age2);
  //   m2->addVal("age", &age3);

  //   auto bytes = m2->serializeAndWrite(&conn, SerializationMethod::COPY);
  //   cout << "Wrote " << bytes << " bytes." << std::endl;
  // }
  sleep(5);
}

int main() {
  // std::cout << "PID " << getpid() << std::endl;
  // /****** INIT uSser *********/
  // SymTab::init();
  // /***************************/

  // /***** Read Proto files *****/
  // SymTab::readFile("proto/s1.proto");
  // /***************************/

  // thread _(server);
  // sleep(2);
  // client();

  return 0;
}
