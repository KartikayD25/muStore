// #include "user.hpp"
// #include "experiment.h"
// #include <thread>

// using namespace std;
// using namespace user;

// net::common::Address server_address(SERVER_IP, SERVER_PORT);

// void server() {
//   net::TCPServer server(server_address, 5);
//   assert(server.StartListen());
//   net::TCPConn *conn = server.GetNewConn();
//   size_t size = user::net::common::BUFFER_SIZE;
//   char *buf = (char *)malloc(size);

//   while (true) {
//     bzero(buf, size);
//     cout << "Waiting for read..\n";
//     ssize_t bytes = conn->Read(buf, size);
//     cout << "Read " << bytes << std::endl;

//     // Deserialise the message as VideoMetadata
//     Deserialiser *de_obj =
//         InitMessage::getDeserialiserPtr("user", buf, size);

//     // Retrieve field sizes (if they are arrays)
//      std::optional locale = de_obj->getString("username");
//     std::optional outdate = de_obj->getString("password");
 

//     // Print sizes of arrays (or single fields)
//      cout << "username: " << (locale.has_value() ? *locale : "NULL") << endl;
//     cout << "password: " << (outdate.has_value() ? *outdate : "NULL") << endl;

//     break;
//   }

//   free(buf);  // Don't forget to free the allocated memory
// }
// void client() {
//   // ssize_t bytes = 0;
//   net::TCPClient conn(server_address);
//   // cout << "Connecting...\n";
//   assert(conn.Connect());
//   cout << "Connection Done\n";
//   /*
//   {
//     user::Message *m1 = user::InitMessage::allocateMessage("Person");
//     assert(m1 != nullptr);
//     int age = 26;
//     m1->setVal("name", "Siddharth");
//     m1->setVal("age", age);

//     auto bytes = m1->serializeAndWrite(&conn, SerializationMethod::COPY);
//     cout << "Wrote " << bytes << " bytes." << std::endl;
//   }
//   */
//   {
//     user::Message *m2 = user::InitMessage::allocateMessage("user");
//     assert(m2 != nullptr);
//     m2->setVal("username", "qwerty");
//     m2->setVal("password", "1122132");
//     auto bytes = m2->serializeAndWrite(&conn, SerializationMethod::SG);
//     cout << "Wrote " << bytes << " bytes." << std::endl;
//   }
//   sleep(5);
// }

int main() {
  // std::cout << "PID " << getpid() << std::endl;
  // /****** INIT uSser *********/
  // SymTab::init();
  // /***************************/

  // /***** Read Proto files *****/
  // SymTab::readFile("../proto/final.proto");
  // /***************************/

  // thread _(server);
  // sleep(2);
  // client();

  return 0;
}
