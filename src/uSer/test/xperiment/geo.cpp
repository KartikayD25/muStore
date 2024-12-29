// #include "user.hpp"
// #include <thread>
// #include "helper.hpp"
// #include "experiment.h"
// using namespace std;
// using namespace user;
// using namespace Helper;
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
//         InitMessage::getDeserialiserPtr("Geo", buf, size);

//     // Retrieve field sizes (if they are arrays)
//     std::optional<int> lat = 0, lon = 0;
//     lat = de_obj->get<float>("lat");
//     lon = de_obj->get<float>("long");
 

//     // Print sizes of arrays (or single fields)
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
//     LatencyRecorderMgr::Init()->StartNewStats(0,serialization_method_t::sg ,8, 2);
//     for (int i = 0 ; i < NUMBER_OF_MESSAGES ; i++){
//       user::Message *m2 = user::InitMessage::allocateMessage("Geo");
//       assert(m2 != nullptr);
//       float lat = Random::GetRandrange(-90.0f,90.0f), lon = Random::GetRandrange(-180.0f,180.0f) ;
//       m2->setVal("lat", lat);
//       m2->setVal("long", lon);

//       auto bytes = m2->serializeAndWrite(&conn, SerializationMethod::SG);
//       cout << "Wrote " << bytes << " bytes." << std::endl;
//     } 
//     std::string filename = "geo_all.csv";
//     std::string filename_agg = "geo_agg.csv";
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

  // thread _(server);
  // sleep(2);
  // client();

  return 0;
}
