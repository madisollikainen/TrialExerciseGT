/**
 *	Author: Madis Ollikainen
 *	File:	merkleHasher.hpp
 *
 *	Implements class template MerkleHasher, which 
 *	hold methods for:
 *		a) 	Calcuating the root of Merkle hash trees 
 *			whose leaves correspond to hashes of 
 *			some text file lines.  
 *		b)	Extracting leaf-to-root hash chains 
 *			from a Merkle tree whose leaves 
 *			correspond to hashes of some text  
 *			file lines. 
 *
 *	MerkleHasher is templated on:
 *		a) The hash function used 
 *		b) The 'hash merging' function used
 *
 *	The algorithms used in the class are based on [1].
 *	For further details refer to the documentation in ../doc/doc.
 *	
 *	Ref: 
 *		[1] Ahto Buldas, Ahto Truu, Risto Laanoja, Rainer Gerhards:
 *			Efficient Record-Level Keyless Signatures for Audit Logs. NordSec 2014: 149-164
 *
 */

#ifndef MERKLE_HASHER_HPP
#define MERKLE_HASHER_HPP


#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <fstream>

#include "myHashInterface.hpp"

// ------------------------------------ //
// ----- MerkleHasher DECLARATION ----- //
// ------------------------------------ // 
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
class MerkleHasher
{

public:

	// --- Wrappers for hashing one or two inputs --- //
	std::string hash(const std::string s){	return H(s);	}
	std::string hash(const std::string s1, const std::string s2){	return H( M(s1,s2) );	}

	// --- Getter for the leaves vector --- //
	std::vector<std::string> getLeaves() { return leaves; }

	// --- Method for getting the root and leafs of a Merkle tree --- //
	std::string getRoot( const std::string file, bool saveLeaves);

	// --- Method for extracting hash chains from a Merkle tree --- //
	hash_chain_t getHashChain( const std::string file, std::string target_line, bool saveLeaves); 

	// --- Method for verifying if a hash chain is self-consistent --- //
	// Currently not used
	bool selfConsistentHashChain(hash_chain_t& chain);
	

private:
	std::vector<std::string> leaves;

}; // END MERKLEHASHER DECLARATION



// --------------------------------------- //
// ----- MerkleHasher IMPLEMENTATION ----- //
// --------------------------------------- // 


// --- Method for getting the root and leafs of a Merkle tree --- //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
std::string MerkleHasher<H,M>::getRoot( const std::string file, bool saveLeaves)
{
	// Always clear the leaves vector
	leaves.clear();

	// Initialise the output
	std::string root;

	// Make a vector to hold the roots of the 
	// forest consisting of complete trees.
	std::vector<std::string> roots_;

	// Open the file 
	std::ifstream input_file(file);
	if(input_file.is_open())
	{
		// Loop over the lines in the file 
		std::string line; 
		while( std::getline(input_file, line) )
		{
			// Get the hash of the line and 
			// store the leaf if asked
			std::string leaf = hash(line);
			if(saveLeaves) { leaves.push_back(leaf); }	

			// Loop over the complete-tree-forest roots
			for (uint i=0; i<roots_.size(); ++i)
			{
				// Push the agglomerated value into the
				// first slot where there is an empty string.
				// And set the leaf variable to be "" for later
				// checking
				if( roots_[i] == "" )
				{
					std::swap(roots_[i],leaf);
					break;
				}
				// Until an empty string is found, hash 
				// the roots toghether (merge trees) and 
				// set the roots of the merged trees to be
				// empty string
				else
				{
					leaf = hash(roots_[i],leaf);
					roots_[i] = "";
				}
			}
			// If there were no empty strings on in the 
			// roots_ vector, then push the agglomerated 
			// value to the end of the vector
			if(leaf != "" )
			{
				roots_.push_back(leaf);
				leaf = "";
			}
		} 
	}
	input_file.close();

	// Merge the complete-tree-forest from 
	// right-to-left. Start by finding the first
	// non-empty string in the roots_ vector
	// and setting it as the initial value for root
	std::vector<std::string>::iterator it = std::find_if(roots_.begin(), roots_.end(), [](std::string s){ return s != ""; } );
	root = *it;

	// Move the iterator by one
	++it;

	// Tansvers the roots_ vector and hash r with all non-empty strings
	for ( ; it < roots_.end(); ++it)
	{
		if (*it != "")
		{
			root = hash(*it,root);
		}
	}
	// Return the root
	return root;
} // END getRoot


// --- Method for extracting hash chains from a Merkle tree --- //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
hash_chain_t MerkleHasher<H,M>::getHashChain( const std::string file, std::string target_line, bool saveLeaves)
{
	// Always clear the leaves vector
	leaves.clear();

	// Initialise the output
	hash_chain_t chain;

	// Initialise the root
	std::string root;

	// Make a vector to hold the roots of the 
	// forest consisting of complete trees.
	std::vector<std::string> roots_;

	// Initialise the target
	std::string target = hash(target_line);

	// Open the file 
	std::ifstream input_file(file);
	if(input_file.is_open())
	{
		// Loop over the lines in the file 
		std::string line; 
		while( std::getline(input_file, line) )
		{
			// Get the hash of the line and 
			// store the leaf if asked
			std::string leaf = hash(line);
			if(saveLeaves) { leaves.push_back(leaf); }
			
			// Loop over the complete-tree-forest roots
			for (uint i=0; i<roots_.size(); ++i)
			{				
				// Push the agglomerated value into the
				// first slot where there is an empty string.
				// And set the leaf variable to be "" for later
				// checking
				if( roots_[i] == "" )
				{
					std::swap(roots_[i],leaf);
					break;
				}
				// Until an empty string is found, hash 
				// the roots toghether (merge trees) and 
				// set the roots of the merged trees to be
				// empty string
				else
				{	// Before conducting the hashing,
					// check if any of the two values
					// entering the hash function, is
					// equvalent to the target. If yes,
					// then store them into the hash_chain
					// in correct order.
					if ( roots_[i] != target && leaf != target )
					{
						leaf = hash(roots_[i],leaf);
						roots_[i] = "";
					}
					else if( roots_[i] == target )
					{
						// First push the one which corresponds to the target. 
						chain.push_back( std::make_pair(0,roots_[i]) ); 
						chain.push_back( std::make_pair(1,leaf) );		
						leaf = hash(roots_[i],leaf);
						roots_[i] = "";
						target = leaf;
					}
					else
					{
						// First push the one which corresponds to the target. 
						chain.push_back( std::make_pair(1,leaf) );		 
						chain.push_back( std::make_pair(0,roots_[i]) ); 
						leaf = hash(roots_[i],leaf);
						roots_[i] = "";
						target = leaf;
					}
				}
			}
			// If there were no empty strings on in the 
			// roots_ vector, then push the agglomerated 
			// value to the end of the vector
			if(leaf != "" )
			{
				roots_.push_back(leaf);
				leaf = "";
			}
		}
	}
	input_file.close();
	

	// Merge the complete-tree-forest from 
	// right-to-left. Start by finding the first
	// non-empty string in the roots_ vector
	// and setting it as the initial value for root
	std::vector<std::string>::iterator it = std::find_if(roots_.begin(), roots_.end(), [](std::string s){ return s != ""; } );
	root = *it;

	// Move the iterator by one
	++it;

	// Tansvers the roots_ vector and hash r with all non-empty strings
	// During the hashing stage, use the same logic as during roots_ traversal
	// above for adding values to the hash chain. In the end of this step the
	// only missing value from the hash chain will be the final root.
	for ( ; it < roots_.end(); ++it)
	{
		if (*it != "")
		{
			if ( root != target && *it != target)
			{
				root = hash(*it,root);	
			}
			else if (*it == target)
			{
				// First push the one which corresponds to the target. 
				chain.push_back( std::make_pair(0,*it) ); 
				chain.push_back( std::make_pair(1,root) );
				root = hash(*it,root);
				target = root;
			}
			else
			{
				// First push the one which corresponds to the target. 
				chain.push_back( std::make_pair(1,root) ); 
				chain.push_back( std::make_pair(0,*it) );
				root = hash(*it,root);
				target = root;
			}
			
		}
	}
	// Now check if the target == root. If it does,
	// add it to the chain. If not, then the initial 
	// target was not amongs the leafs.
	if ( target == root )
	{
		chain.push_back( std::make_pair(-1,root) );	
	}
	
	// Return the hash chain
	return chain;
} // END getHashChain


// --- Method for verifying if a hash chain is self-consistent --- //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
bool MerkleHasher<H,M>::selfConsistentHashChain(hash_chain_t& chain)
{
	// Chain of size 0 or 1 is always self-consisten
	if( chain.size() < 2)
	{
		return true;
	}
	// The chain must always be of size 0 or a odd number
	else if ( chain.size() % 2 == 0 )
	{
		return false;
	}
	// Transvers the chain and conduct the hashing calcualtions. 
	// Check each of the results against the entries in the chain.
	// If any discrepancy is found, return false.
	else
	{
		for(uint i=0; i < chain.size() - 2; i=i+2  )
		{
			std::string tmp_hash;
			if(chain[i].first == 0)
			{
				tmp_hash = hash( chain[i].second, chain[i+1].second );
			}
			else
			{
				tmp_hash = hash( chain[i+1].second, chain[i].second );
			}

			if( tmp_hash != chain[i+2].second)
			{
				return false;
			}
			if(chain[i].first == chain[i+1].first)
			{
				return false;
			}
		}
	}
	// If the control mechanism have not yet returned with false, then 
	// the chain is self consistent. 
	return true;
} // END selfConsistentHashChain


#endif // MERKLE_HASHER_HPP
