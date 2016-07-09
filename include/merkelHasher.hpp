/*	Author: Madis Ollikainen
 *	File:	merkelHasher.hpp
 *
 *	Header file for the MerkelHasher class, which 
 *	implements methods for reading an text file and 
 *	generating a hash tree (Merkel tree) out of the 
 *	hashes of the lines in the file. 
 *
 *	The class will be templated on:
 *		a) The hash function 
 *		b) The 'hash merging' function
 *
 *	 
 *	The Merkel tree structure will be formed similarly 
 *	as suggested in [1]
 *
 * 	Ref: 
 *		[1] Ahto Buldas, Ahto Truu, Risto Laanoja, Rainer Gerhards:
 *			Efficient Record-Level Keyless Signatures for Audit Logs. NordSec 2014: 149-164
 *	 
 */

#ifndef MERKEL_HASHER_HPP
#define MERKEL_HASHER_HPP


#include <vector>
#include <string>


 template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
 class MerkelHasher
 {

 public:


 	/*
 	 *	The hashing functions. Both for one 
 	 *	and two std::string inputs.
 	 */
 	std::string hash(const std::string s){	return H(s);	}
 	std::string hash(const std::string s1, const std::string s2){	return H( M(s1,s2) );	}



 private:

 	std::vector<std::string> leafs_;	// Vector to hold the leaf hashes
 	std::vector<std::string> roots_;	


 };


#endif // MERKEL_HASHER_HPP
