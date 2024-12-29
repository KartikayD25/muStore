# uSer codebase --- why not only declarations in hpp files and definitions in cpp files??  

## For buffer size : Change the BUFFER_SIZE macro in net::common, else don't pass the TEST_USER flag while compiling (modify the makefile in uSer/test). 

## src folder 

### allocator.hpp 
#### nothing as of now 

### define.hpp
#### defining general rules in this file for the whole library. 

### deserialise_message_meta.hpp 
#### 

### Symbol table 
#### Symtab class API
##### getSymbol : (overloaded method), the one taking member_name as arg, calls the one taking id as arg. 
##### getMessageId : looks up the name in the unordered map. 
##### Static APIs have the functions to make the symbol table (Singleton class) and to access it. 
##### addSymbol() : checks to ensure that the symbol to be added is unique. Upon asserting the same, proceeds to update the unordered_maps containing the relations between symbol, id and message. 
##### readFile() : Parses the protofile, extracts the symbols, adds them to the Symtab, adds the fields to each Symbol. 

## Go through a Demo : 

### Client Side: 
#### main() initializes the SymTab and proto files are read, preparing the symbol table for further serialization and sending the message. 
#### user::InitMessage::allocateMessage makes a new message object with the desired proto struct. 
#### this object can further be used to prepare the message by adding values to the instance of that class.
#### This addVal method adds the value to a vector of FieldValue* types. 

#### addVal(): 
##### It has declaration for each FieldValue type. Each of the four types are children of the class FieldValue, with each of them having addVal declaration.
##### addVal method in class Message is just a wrapper on the method of FieldValue class. 
##### setVal used for non repeated field types. 
##### bitmap gets updated with setVal and addVal but repeated bitmap functioning how exactly?? 
##### While preparing a symbol table, the message name itself, and its fields, all are considered to be symbols only?? 
##### Does each symbol have only 32 possible member symbols in it?? bitmap suggests that. 



#### serialiseAndWrite() is called. A method selector on the basis of two-copy or one-copy approach.
#### serialiseAndWriteSG() : need to be called for zerocopy ??  just call sendmsg with MSG_ZEROCOPY flag instead of writev?
##### while checking for the type of the field, why using negatives, why now straight forward affirmative boolean?
##### While copying data, into the buffer, simple non repeated field is copied directly (trivial), simple repeated had its size copied first, and then the contents, Complex non repeated the same way and complex repeated, 

### Server Side: 
####