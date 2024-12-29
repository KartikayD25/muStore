#include "user.hpp"

using namespace user;

int main(int argc, char const *argv[])
{
  /****** INIT uSser *********/
  SymTab::init();
  /***************************/

  /***** Prepare Message *****/
  SymTab::readFile("/home/xeon01/vishesh/uSer/src/parser/person.proto");
  /***************************/

  /*
  std::vector<ProtoMessage> res = parseProtoFile("employee.proto");
  int x = test_fn();
  std::cout << "Hello " << res[0].name << std::endl;
  */
  return 0;
}
