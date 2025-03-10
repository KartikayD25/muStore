import re
from typing import List, Dict, Any
import os

class Field:
    def __init__(self, field_type: str, name: str, tag: int, is_repeated: bool = False):
        self.type = field_type
        self.name = name
        self.tag = tag
        self.is_repeated = is_repeated

class Message:
    def __init__(self, name: str):
        self.name = name
        self.fields: List[Field] = []

class ProtoCodeGenerator:
    def __init__(self):
        self.messages: Dict[str, Message] = {}
        
    def parse_proto_file(self, filepath: str) -> None:
        """Parse the proto-like file and build message definitions"""
        current_message = None
        
        with open(filepath, 'r') as f:
            content = f.read()
            
        # Remove comments
        content = re.sub(r'//.*?\n|/\*.*?\*/', '', content, flags=re.DOTALL)
        
        # Parse messages
        for line in content.split('\n'):
            line = line.strip()
            if not line:
                continue
                
            # Message definition start
            if line.startswith('message'):
                message_name = line.split()[1].strip('{')
                current_message = Message(message_name)
                self.messages[message_name] = current_message
                
            # Field definition
            elif current_message and '=' in line:
                parts = line.strip(';').split()
                is_repeated = False
                
                if parts[0] == 'repeated':
                    is_repeated = True
                    parts = parts[1:]
                    
                field_type = parts[0]
                field_name = parts[1]
                tag = int(parts[3])
                
                field = Field(field_type, field_name, tag, is_repeated)
                current_message.fields.append(field)

    def generate_cpp_code(self, output_dir: str) -> None:
        """Generate C++ header and implementation files"""
        os.makedirs(output_dir, exist_ok=True)
        
        for message in self.messages.values():
            # Generate header file
            header_content = self._generate_header(message)
            with open(f"{output_dir}/{message.name}.h", 'w') as f:
                f.write(header_content)
            
            # Generate implementation file
            impl_content = self._generate_implementation(message)
            with open(f"{output_dir}/{message.name}.cpp", 'w') as f:
                f.write(impl_content)

    def _generate_header(self, message: Message) -> str:

        code = f"""
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "symtab.hpp"
#include "types.hpp"

class {message.name} {{
public:
    {message.name}(
        message_name_(f"{message.name}")
        symbol_(SYMTAB->getSymbol(message_name_))  
        init()
    );
    ~{message.name}();

  void init() {
    assert(symbol_ != nullptr);
    for (const auto &mem_sym : symbol_->members_) {
      FieldValue *val = nullptr;
      if (TypeHelper::isComplexType(mem_sym.type_)) {
        if (mem_sym.repeated_) {
          val = new RepeatedComplexFieldValue(mem_sym);
          rc_val_.push_back(val);
        } else {
          val = new SingleComplexFieldValue(mem_sym);
          sc_val_.push_back(val);
        }
      } else {
        if (mem_sym.repeated_) {
          val = new RepeatedSimpleFieldValue(mem_sym);
          rs_val_push_back(val);
        } else {
          val = new SingleSimpleFieldValue(mem_sym);
          ss_val_.push_back(val);
        }
      }
    }
  }





    // Getters
"""
        
        # Generate getters
        for field in message.fields:
            cpp_type = type_mapping.get(field.type, field.type)
            if field.is_repeated:
                code += f"    const std::vector<{cpp_type}>& get{field.name.capitalize()}() const;\n"
            else:
                code += f"    {cpp_type} get{field.name.capitalize()}() const;\n"
        
        code += "\n    // Setters\n"
        
        # Generate setters
        for field in message.fields:
            cpp_type = type_mapping.get(field.type, field.type)
            if field.is_repeated:
                code += f"    void add{field.name.capitalize()}(const {cpp_type}& value);\n"
            else:
                code += f"    void set{field.name.capitalize()}({cpp_type} value);\n"
        
        code += """
private:
    const std::string message_name_{f"{message.name}"};
    const Symbol *symbol_{};
    std::vector<RepeatedSimpleFieldValue> rs_val_;
    std::vector<SingleSimpleFieldValue> ss_val_;
    std::vector<RepeatedComplexFieldValue> rc_val_;
    std::vector<SingleComplexFieldValue> sc_val_;

};
"""
        return code

    def _generate_implementation(self, message: Message) -> str:
        """Generate implementation file content for a message"""
        type_mapping = {
            'int32': 'int32_t',
            'int64': 'int64_t',
            'string': 'std::string',
            'float': 'float',
            'double': 'double',
            'bool': 'bool'
        }
        
        code = f"""
#include "{message.name}.h"

{message.name}::{message.name}() : symbol_table_(std::make_unique<SymbolTable>()) {{
    // Initialize symbol table with field definitions
}}

{message.name}::~{message.name}() = default;

"""
        
        # Generate getter implementations
        for field in message.fields:
            cpp_type = type_mapping.get(field.type, field.type)
            if field.is_repeated:
                code += f"""
const std::vector<{cpp_type}>& {message.name}::get{field.name.capitalize()}() const {{
    return symbol_table_->getRepeatedValue<{cpp_type}>({field.tag});
}}
"""

            else:
                code += f"""
{cpp_type} {message.name}::get{field.name.capitalize()}() const {{
    return symbol_table_->getValue<{cpp_type}>({field.tag});
}}
"""
        
        # Generate setter implementations
        for field in message.fields:
            cpp_type = type_mapping.get(field.type, field.type)
            if field.is_repeated:
                code += f"""
void {message.name}::add{field.name.capitalize()}(const {cpp_type}& value) {{
    symbol_table_->addValue({field.tag}, value);
}}
"""
            else:
                code += f"""
void {message.name}::set{field.name.capitalize()}({cpp_type} value) {{
    symbol_table_->setValue({field.tag}, value);
}}
"""
        
        return code

def main():
    # Example usage
    generator = ProtoCodeGenerator()
    generator.parse_proto_file("example.proto")
    generator.generate_cpp_code("generated")

if __name__ == "__main__":
    main()