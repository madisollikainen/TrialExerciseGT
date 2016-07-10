/**
 *	Author:	Madis Ollikainen 
 *	File:	mySignatureInterface.hpp  
 *
 *	Implenets an empty signature fucntion.
 */

#ifndef MY_SIGNATURE_INTERFACE_HPP
#define MY_SIGNATURE_INTERFACE_HPP

#include <string>

// For commented out Guardtime signature
// #include <ksi/ksi.h>
// #include <ksi/net_uri.h>
// #include "ksi_common.h"

// An empty signature function, which just
// returns the same string again.

std::string signature(std::string& str)
{
	std::string sign = str;	
	// The commented out Guardtime SDK 
	// ... not sure if correctly called nor is I should do something with ksi
	// But it is not used, thus not big problems
	//
	// KSI_CTX *ksi = NULL;	
	// int res = KSI_createSignature(ksi, str, &sign);
	//
	return str;
}

#endif // MY_SIGNATURE_INTERFACE_HPP
