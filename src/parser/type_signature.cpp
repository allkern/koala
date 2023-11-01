#include "type_signature.hpp"

std::string koala::get_signature_string(type_signature sig) {
    std::string str;

    if (sig.is_static)
        str.append("static ");

    if (sig.is_const)
        str.append("const ");
    
    str.append(sig.base);

    while (sig.pointer_rank) {
        str.push_back('*');

        --sig.pointer_rank;
    }

    return str;
}