// #include "user.hpp"
// #include "experiment.h"
// #include <thread>

// using namespace std;
// using namespace user;
// using namespace Helper;
// net::common::Address server_address(SERVER_IP, SERVER_PORT);
// char * getStr(int size){
//   char * str = (char *) calloc(1,size);
//   Random::FillRandomReadableASCII(str,size);
//   return str;
// }
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
//         InitMessage::getDeserialiserPtr("Profile", buf, size);

//     // Retrieve field sizes (if they are arrays)
//     std::optional locale = de_obj->getString("locale");
//     std::optional<int> string_arr = de_obj->size("hotel_ids");
 

//     // Print sizes of arrays (or single fields)
//     cout << "locale: " << (locale.has_value() ? *locale : "NULL") << endl;
//      // Print the "comments" field (assuming it is an array of strings)
//     if (string_arr.has_value()) {
//       cout << "string_arr: [";
//       for (uint32_t i = 0; i < *string_arr; i++) {
//         std::optional<byte_t> s = de_obj->getByte("hotel_ids", i);
//         if (s.has_value()) {
//           cout << "\"" << string(s->first, s->first + s->second) << "\" ";
//         }
//       }
//       cout << "]" << endl;
//     }

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
//     LatencyRecorderMgr::Init()->StartNewStats(0,serialization_method_t::sg ,3471, 420);
//     for(int i = 0; i < NUMBER_OF_MESSAGES ; i ++){
//         user::Message *m2 = user::InitMessage::allocateMessage("Profile");
//         assert(m2 != nullptr);
//         for (int j = 0 ; j < 30 ; j++){
//           char * str = (char *)calloc(1,6);
//           Random::FillRandomReadableASCII(str,6);
//           m2->addVal("hotel_ids", str,6);
//         }
//         char * locale = (char *) calloc(1,26);
//         Random::FillRandomReadableASCII(locale,26);
//         m2->setVal("locale", locale,26);
//         auto bytes = m2->serializeAndWrite(&conn, SerializationMethod::SG);
//         cout << "Wrote " << bytes << " bytes." << std::endl;
//     }
//     std::string filename = "profile_all.csv";
//     std::string filename_agg = "profile_agg.csv";
//     LatencyRecorderMgr::Get()->DumpAllMetricsToCSV(filename);
//     LatencyRecorderMgr::Get()->DumpStatToCSV(filename_agg);
//   }
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
