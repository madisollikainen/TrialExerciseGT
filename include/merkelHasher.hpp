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
#include <algorithm>


/*****************************************************
 *** The templated MerkelHasher class declaration. ***
 *****************************************************/

 template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
 class MerkelHasher
 {

 public:

 	/*
 	 * Simple consturctor adding the initial empty
 	 * string to the roots_ vector. The placement of 
 	 * empty strings in the roots_ vector will be used
 	 * as information carriers during the construction of
 	 * the 'canonical' Merkel Tree. 
 	 *
 	 * No specific destructor must be declared, as only STL 
 	 * container have been used, which take care of destrying themselves.
 	 *
 	 */
 	// MerkelHasher(){ roots_.push_back("");}


 	/*
 	 *	The hashing functions. Both for one 
 	 *	and two std::string inputs.
 	 */
 	std::string hash(const std::string s){	return H(s);	}
 	std::string hash(const std::string s1, const std::string s2){	return H( M(s1,s2) );	}

 	// An add function for adding entries into the tree. 
 	// The entries will be hashed before becoming leafs.
 	void add(std::string s){ add_leaf_( hash(s) ); }

 	// Clear function for returning the MerkelHasher to the initial state
 	void clear(){ leafs_.clear(); roots_.clear();  }

 	// A function for merging the forest and getting the root
 	std::string root();


 	// A test function
 	void test(const std::string leaf)
 	{
 		add_leaf_(leaf);

 		std::cout << "Added a new leaf: " << leaf << std::endl;
 		std::cout << "leafs_ vector: ";
 		for (uint i=0; i<leafs_.size(); ++i) { std::cout << leafs_[i] << " "; }
 		std::cout << std::endl << "roots_ vector: ";
 		for (uint i=0; i<roots_.size(); ++i) 
 		{ 	if(roots_[i]=="") std::cout << "-" << " ";
 			else std::cout << roots_[i] << " ";  
 		}
 		std::cout<<std::endl;
 		std::cout<<std::endl;
 		std::cout << "Root of the Merkel tree: " << root() << std::endl;	
 	}


 private:

 	std::vector<std::string> leafs_;	// Vector to hold the leaf hashes
 	std::vector<std::string> roots_;	

 	/*
 	 * A helper function for adding a single leaf 
 	 * into the tree. The function body will be 
 	 * 
 	 */
 	 void add_leaf_(std::string leaf);

 }; // END MERKELHASHER DECLARATION



/*****************************************************
 *** The MerkelHasher member function definitions. ***
 *****************************************************/

// ------------------------------ //
// --- The add_leaf_ function --- //
// ------------------------------ //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
void MerkelHasher<H,M>::add_leaf_(std::string leaf)
{
	// Add the new leaf to the leafs vector
	leafs_.push_back(leaf);

	// Traverse the roots_ vector and update it.
	// Traversal is done with simple rules:
	//
	//		a)	If the roots_[i] is an empty string,
	//			set it to be the current 'leaf' value
	//			roots_[i] = leaf. Finish the loop.
	//
	// 		b)	If roots_[i] is a non-empty string,
	//		 	then hash it together with the leaf
	//			leaf = hash(roots_[i],leaf) and set 
	//			change it to empty string roots_[i]="". 
	//			After that carry on with the loop.
	//		
	//		c)	If the end of the vector roots_ is reach
	//			without finding an empty roots_[i], then 
	//			push the hashed together leaf to the end 
	//			end of the roots_ vector.
	//			   
	//
	for (uint i=0; i<roots_.size(); ++i)
	{
		if( roots_[i] == "" )
		{
			// roots_[i] = leaf;
			std::swap(roots_[i],leaf);
			break;
		}
		else
		{
			leaf = hash(roots_[i],leaf);
			roots_[i] = "";
		}
	}
	if(leaf != "" )
	{
		roots_.push_back(leaf);
		leaf = "";
	}

}


// --------------------------- //
// --- The root() function --- //
// --------------------------- //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
std::string MerkelHasher<H,M>::root()
{
	// First the forest of complete 
	// trees must be merged. This is 
	// done from right to left.

	// Find the first entry in the roots_ vector, which is a non-empty string.
	std::vector<std::string>::iterator it = std::find_if(roots_.begin(), roots_.end(), [](std::string s){ return s != ""; } );
	std::string r = *it;
	++it;

	// Tansvers the roots_ vector and hash r with all non-empty strings
	// for (uint i=0; i<roots_.size(); ++i)
	for ( ; it < roots_.end(); ++it)
	{
		// if (roots_[i] != "")
		if (*it != "")
		{
			// r = hash(roots_[i],r);
			r = hash(*it,r);
		}
	}

	// Return the root
	return r;
}


#endif // MERKEL_HASHER_HPP
