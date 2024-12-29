#include "user.hpp"
#include <thread>

using namespace std;
using namespace user;

net::common::Address server_address("127.0.0.1", 8085);

void server() {
  net::TCPServer server(server_address, 5);
  assert(server.StartListen());
  net::TCPConn *conn = server.GetNewConn();
  size_t size = user::net::common::BUFFER_SIZE;
  char *buf = (char *)malloc(size);

  // // while (true) {
  // //   bzero(buf, size);
  // //   cout << "Waiting for read..\n";
  // //   ssize_t bytes = conn->Read(buf, size);
  // //   cout << "Read " << bytes << std::endl;

  // //   // Deserialise the message as VideoMetadata
  // //   Deserialiser *de_obj =
  // //       InitMessage::getDeserialiserPtr("VideoMetadata", buf, size);

  // //   // Retrieve field sizes (if they are arrays)
  // //   std::optional<int> duration = 0, likes = 0, dislikes = 0, comments_size = 0;
  // //   std::optional name = de_obj->getString("name");
  // //   duration = de_obj->get<int32_t>("duration");
  // //   likes = de_obj->get<int32_t>("Likes");
  // //   dislikes = de_obj->get<int32_t>("Dislikes");
 

  // //   // Print sizes of arrays (or single fields)
  // //   cout << "name: " << (name.has_value() ? *name : "NULL") << endl;
  // //   cout << "duration: " << (duration.has_value() ? to_string(*duration) : "NULL") << endl;
  // //   cout << "likes: " << (likes.has_value() ? to_string(*likes) : "NULL") << endl;
  // //   cout << "dislikes: " << (dislikes.has_value() ? to_string(*dislikes) : "NULL") << endl;
  // //   comments_size = de_obj->size("comments");
  // //   cout << "comments_size: " << (comments_size.has_value() ? to_string(*comments_size) : "NULL") << endl;

  // //   // Print the "comments" field (assuming it is an array of strings)
  // //   if (comments_size.has_value()) {
  // //     cout << "Comments: [";
  // //     for (uint32_t i = 0; i < *comments_size; i++) {
  // //       std::optional<byte_t> comment = de_obj->getByte("comments", i);
  // //       if (comment.has_value()) {
  // //         cout << "\"" << string(comment->first, comment->first + comment->second) << "\" ";
  // //       }
  // //     }
  // //     cout << "]" << endl;
  // //   }

  //   break;
  // }

  // free(buf);  // Don't forget to free the allocated memory
}
void client() {
  // ssize_t bytes = 0;
  net::TCPClient conn(server_address);
  // cout << "Connecting...\n";
  assert(conn.Connect());
  cout << "Connection Done\n";
  /*
  {
    user::Message *m1 = user::InitMessage::allocateMessage("Person");
    assert(m1 != nullptr);
    int age = 26;
    m1->setVal("name", "Siddharth");
    m1->setVal("age", age);

    auto bytes = m1->serializeAndWrite(&conn, SerializationMethod::COPY);
    cout << "Wrote " << bytes << " bytes." << std::endl;
  }
  */
  // {
  //   user::Message *m2 = user::InitMessage::allocateMessage("VideoMetadata");
  //   assert(m2 != nullptr);
  //   int duration = 26, likes = 36, dislikes = 73;
  //   m2->setVal("name", "Video 1");
  //   m2->setVal("duration", duration);
  //   m2->setVal("Likes", likes);
  //   m2->setVal("Dislikes", dislikes);
  //   m2->addVal("comments", "This is a comment");
  //   m2->addVal("comments", "This is another comment");
  //   m2->addVal("comments", "This is yet another comment");
  //   m2->addVal("comments", "This is the last comment");

  //   auto bytes = m2->serializeAndWrite(&conn, SerializationMethod::COPY);
  //   cout << "Wrote " << bytes << " bytes." << std::endl;
  // }
  // sleep(5);
}

int main() {
  std::cout << "PID " << getpid() << std::endl;

  return 0;
}
