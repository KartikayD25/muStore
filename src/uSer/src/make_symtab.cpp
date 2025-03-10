#include "symtab.hpp"
#include <thread>
#include "helper.hpp"
using namespace std;
using namespace user;
using namespace Helper;


int main(){
    SymTab::init();
    SymTab::readFile("../proto/final.proto");
}