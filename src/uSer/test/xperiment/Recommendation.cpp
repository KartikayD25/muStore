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
//         InitMessage::getDeserialiserPtr("Recommendation", buf, size);

//     // Retrieve field sizes (if they are arrays)
//     std::optional<double> lat = 0.0, lon = 0.0;
//     std::optional required = de_obj->getString("required");
//     lat = de_obj->get<double>("lat");
//     lon = de_obj->get<double>("long");
   

//     // Print sizes of arrays (or single fields)
//     cout << "lat: " << (lat.has_value() ? to_string(*lat) : "NULL") << endl;
//     cout << "lon: " << (lon.has_value() ? to_string(*lon) : "NULL") << endl;
//     cout << "required: " << (required.has_value() ? *required : "NULL") << endl;

//     break;
//   }

//   free(buf);  // Don't forget to free the allocated memory
// };
void client() {
  // ssize_t bytes = 0;
  // net::TCPClient conn(server_address);
  // // cout << "Connecting...\n";
  // assert(conn.Connect());
  // cout << "Connection Done\n";
  // /*
  // {
  //   user::Message *m1 = user::InitMessage::allocateMessage("Person");
  //   assert(m1 != nullptr);
  //   int age = 26;
  //   m1->setVal("name", "Siddharth");
  //   m1->setVal("age", age);

  //   auto bytes = m1->serializeAndWrite(&conn, SerializationMethod::COPY);
  //   cout << "Wrote " << bytes << " bytes." << std::endl;
  // }
  // */
  // {
  //     LatencyRecorderMgr::Init()->StartNewStats(0,serialization_method_t::sg ,3471, 420);
  //   for(int i = 0; i < NUMBER_OF_MESSAGES ; i ++){
  //   user::Message *m2 = user::InitMessage::allocateMessage("Recommendation");
  //   assert(m2 != nullptr);
  //   double lat = 26, lon = 36;
  //   m2->setVal("required", getStr(22));
  //   m2->setVal("lat", lat);
  //   m2->setVal("long", lon);
  //   auto bytes = m2->serializeAndWrite(&conn, SerializationMethod::SG);
  //   cout << "Wrote " << bytes << " bytes." << std::endl;
  //    }
  //   std::string filename = "recommendation_all.csv";
  //   std::string filename_agg = "recommendation_agg.csv";
  //   LatencyRecorderMgr::Get()->DumpAllMetricsToCSV(filename);
  //   LatencyRecorderMgr::Get()->DumpStatToCSV(filename_agg);
  // }
}

int main() {
  // std::cout << "PID " << getpid() << std::endl;
  /****** INIT uSser *********/
  // SymTab::init();
  // /***************************/

  // /***** Read Proto files *****/
  // SymTab::readFile("../proto/final.proto");
  // /***************************/

  // client();

  return 0;
}
