/**
 *	Author:	Madis Ollikainen 
 *	File:	myHashInterface.hpp  
 *
 *	Implemets a very simple wrapper function for OpenSSL 
 *	SHA256 hashing function usage with std::strings.
 *	
 * 	
 */

#ifndef MY_HASH_INTERFACE_HPP
#define MY_HASH_INTERFACE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// Hash functionality from OpenSSL
#include <openssl/sha.h>

// Typedef the hash_chain_t type as a vector of pairs of int and string
typedef std::vector< std::pair<int, std::string> > hash_chain_t;

// --- The SHA256 hashing function wrapper --- //
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


// --- My Hash mergin function just adds/concatinates the strings --- //
std::string myHashMerge(const std::string s1, const std::string s2 )
{
    return (s1+s2);
}




#endif // MY_HASH_INTERFACE_HPP
