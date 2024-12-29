#include "user.hpp"

using namespace std;
using namespace user;

int main() {
  SymTab::init();
  auto symTab = SymTab::getSymTab();
  /*
     M1 {
      int32 a = 1;
      repeated int32 b = 2;
     }
  */
  Symbol m1 = Symbol("M1");
  Symbol m1_a = Symbol(1, "a", false, &m1, UserTypes::TYPE_INT32, false);
  Symbol m1_b = Symbol(2, "b", true, &m1, UserTypes::TYPE_INT32, false);
  Symbol m1_c = Symbol(4, "c", true, &m1, UserTypes::TYPE_INT32, false);

  assert(m1.addMember(m1_a));
  assert(m1.addMember(m1_b));
  // assert(!m1.addMember(m1_b));
  // assert(!m1.addMember(m1_c));
  // Symbol err = Symbol(2, "b", true, &m1, UserTypes::TYPE_INT64, false);
  // assert(!m1.addMember(err));

  assert(m1.members_.size() == 2);
  symTab->addSymbol(&m1);
  /*
     M2 {
      M1 a = 1;
      int32 b = 2;
     }
     */
  Symbol m2 = Symbol("M2");
  Symbol m2_a = Symbol(1, "a", false, &m2, UserTypes::TYPE_MESSAGE, &m1, false);
  Symbol m2_b = Symbol(2, "b", false, &m2, UserTypes::TYPE_INT32, false);

  assert(m2.addMember(m2_a));
  assert(m2.addMember(m2_b));
  // assert(!m2.addMember(m2_b));
  assert(m2.members_.size() == 2);

  symTab->addSymbol(&m2);
  return 0;
}
