/**
 *	Author:	Madis Ollikainen 
 *	File:	mySignatureInterface.hpp  
 *
 *	Implenets an empty signature fucntion.
 */

#ifndef MY_SIGNATURE_INTERFACE_HPP
#define MY_SIGNATURE_INTERFACE_HPP

#include <string>

// An empty signature function, which just
// returns the same string again.

std::string signature(std::string& str)
{
	return str;
}

#endif // MY_SIGNATURE_INTERFACE_HPP
