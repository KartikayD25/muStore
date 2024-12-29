// #include "user.hpp"
// #include "experiment.h"
// #include <thread>

// using namespace std;
// using namespace user;
// using namespace Helper;
// char * getStr(int size){
//   char * str = (char *) calloc(1,size);
//   Random::FillRandomReadableASCII(str,size);
//   return str;
// }
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
//         InitMessage::getDeserialiserPtr("Search", buf, size);

//     // Retrieve field sizes (if they are arrays)
//     std::optional<int> lat = 0, lon = 0;
//     lat = de_obj->get<float>("lat");
//     lon = de_obj->get<float>("long");
//      std::optional locale = de_obj->getString("indate");
//     std::optional outdate = de_obj->getString("outdate");
 

//     // Print sizes of arrays (or single fields)
//      cout << "indate: " << (locale.has_value() ? *locale : "NULL") << endl;
//     cout << "outdate: " << (outdate.has_value() ? *outdate : "NULL") << endl;
//     cout << "lat: " << (lat.has_value() ? to_string(*lat) : "NULL") << endl;
//     cout << "lon: " << (lon.has_value() ? to_string(*lon) : "NULL") << endl;

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
//     user::Message *m2 = user::InitMessage::allocateMessage("Search");
//     assert(m2 != nullptr);
//     float lat = 26, lon = 36;
//     m2->setVal("lat", lat);
//     m2->setVal("long", lon);
//     m2->setVal("indate", getStr(6));
//     m2->setVal("outdate", getStr(6));
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


  // client();

  return 0;
}
