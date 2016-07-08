/*	Madis Ollikainen 
 *
 *	myHashInterface.hpp  
 *
 *	Implenets a very simple wrapper function for OpenSSL 
 *	SHA256 hashing function usage with std::strings.
 *	
 * 	
 */

#ifndef MY_HASH_INTERFACE_HPP
#define MY_HASH_INTERFACE_HPP

// STD includes
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// Hash functionality from OpenSSL
#include <openssl/sha.h>



std::string sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
} 




#endif // MY_HASH_INTERFACE_HPP
