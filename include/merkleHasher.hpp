/*	Author: Madis Ollikainen
 *	File:	merkleHasher.hpp
 *
 *	Header file for the MerkleHasher class, which 
 *	implements methods for reading an text file and 
 *	generating a hash tree (Merkle tree) out of the 
 *	hashes of the lines in the file. 
 *
 *	The class will be templated on:
 *		a) The hash function 
 *		b) The 'hash merging' function
 *
 *	 
 *	The Merkle tree structure will be formed similarly 
 *	as suggested in [1]. Following [1] a 'canonical' 
 *	hash tree is formed, which can be grown without
 *	previously knowing the number of leaves it will 
 *	contain. 
 *
 *	The 'canonical' hash tree is a binary hash tree, which 
 *	can be constructed as follows:
 *		
 *		1)	The leaves (hashes of the log file entries) are
 *			added from left-to-right. 
 *		
 *		2)	Moving from left-to-right the leaves will be gathered
 *			into a forest of complete trees. 
 *		
 *		3)	All of the complete trees will be as large as possible
 *			with the currently available leaves. Due to the above 
 *			mentioned process it is clear that larger trees will be
 *			on the left and smaller on the right.
 *
 *		4)	When parsing of a file (or any other input entity) is 
 *			complete and no more leaves (entries) are added to the 
 *			tree, the resulting forest of complete trees can be 
 *			merged into a single 'canonical' tree. This merger is
 *			done by merging the root nodes of trees in the forest
 *			from right-to-left. Thus first the two smallest trees 
 *			are merged to form a larger tree, which is then in turn 
 *			merged with the third smallest tree. This procedure is
 *			repeated until all of the trees have been merge into one.
 *			
 *
 *	For singing/time-stamping a file, the root of the 'canonical' Merkle tree formed
 *	from the file can be signed. And for verifying the existance of a single line 
 *	in the file, the hash chain stsrting from the hash of that line to the root of the 
 *	tree can be used. Thus the MerkleHasher class will implement the following functions:
 *		
 *		a)	'getRoot', which takes a file and outputes the root of its Merkle Tree.
 *	 		Additionally this function will store the leaf values into a inpputed vector.
 *
 *		b)	'getHashChain', which takes a file and a string corresponding to a line in the 
 *			file. It will go trough a similar process as function 'getRoot', but in addition
 *			it will keep track of the nodes which should be present in the hash chain and 
 *			outputs the hash chain as an vector of pairs. The pairs will consist of an integer
 *			and a string. The integer will refer to the order in which the corresponding hash
 *			is submitted to the hash merging function to generate a new node. And the string
 *			will hold the hash itself. The hash chain will always have either zero or a odd
 *			number of entries. If the presented line is not present in the file, the hash chain
 *			will be zero. Otherwise, the odd entries correspond to the 'main chain' and the 
 *			even entries correspond to the 'side chains'.  
 *	 
 *
 * 	Ref: 
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


// Typedef the hash_chain_t type as a vector of pairs of int and string
typedef std::vector< std::pair<int, std::string> > hash_chain_t;


/*****************************************************
 *** The templated MerkleHasher class declaration. ***
 *****************************************************/
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
// class MerkleHasher
struct MerkleHasher
{

// public:

	// Wrappers for hashing: either for one or two inputs
	std::string hash(const std::string s){	return H(s);	}
	std::string hash(const std::string s1, const std::string s2){	return H( M(s1,s2) );	}

	// --- getMerkleRoot --> a unified method for getting the root and leafs of a MerkleTree --- //
	std::string getRoot( const std::string file, std::vector<std::string>& leafs); 

	// --- getMerkleRoot --> a unified method for getting the root and leafs of a MerkleTree --- //
	hash_chain_t getHashChain( const std::string file, std::string target_line); 

	// --- selfConsistentHashChain --> a method for verifying if the hash chain is self-consistent
	bool selfConsistentHashChain(hash_chain_t& chain);

	

// private:


}; // END MERKLEHASHER DECLARATION



/*****************************************************
 *** The MerkleHasher member function definitions. ***
 *****************************************************/


// --- getMerkleRoot --> a unified method for getting the root and leafs of a MerkleTree --- //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
std::string MerkleHasher<H,M>::getRoot( const std::string file, std::vector<std::string>& leafs)
{
	// Initialise the output
	std::string root;

	// Open the file 
	std::ifstream input_file(file);

	// Make a vector to hold the roots of the 
	// forest consisting of complete trees.
	std::vector<std::string> roots_;

	// Loop over the lines in the file and 		
	// generate the complete-tree-forest
	// Traverse the roots_ vector and update it.
	// The traversal is done with simple rules:
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
	if(input_file.is_open())
	{

		std::string line; 
		while( std::getline(input_file, line) )
		{
			// Get the hash of the line and store the leaf
			std::string leaf = hash(line);
			leafs.push_back(leaf);

			// Loop over the complete-tree-forest roots
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
		} // end while
	}
	input_file.close();

	// Now merge the complete-tree-forest from 
	// right-to-left. Start by finding the first
	// non-empty string in the roots_ vector.
	std::vector<std::string>::iterator it = std::find_if(roots_.begin(), roots_.end(), [](std::string s){ return s != ""; } );

	// This will be initial value for the root
	root = *it;

	// Move the iterator
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


} 


// --- getMerkleRoot --> a unified method for getting the root and leafs of a MerkleTree --- //
template <std::string (*H)(const std::string), std::string (*M)(const std::string, const std::string )>
hash_chain_t MerkleHasher<H,M>::getHashChain( const std::string file, std::string target_line)
{
	// Initialise the output
	hash_chain_t chain;

	// Initialise the root
	std::string root;

	// Initialise the target
	std::string target = hash(target_line);

	// Open the file 
	std::ifstream input_file(file);

	// Make a vector to hold the roots of the 
	// forest consisting of complete trees.
	std::vector<std::string> roots_;

	// Loop over the lines in the file and 
	// generate the complete-tree-forest
	// Traverse the roots_ vector and update it.
	// The traversal is done with simple rules:
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
	// During the traversal, the hash chain will also
	// be gatherd. That is done in the following way:
	//
	//		1) 	We always keep track of the next value, 
	//			which should be added to the hash chain.
	//			This value is stored in the variable 'target'
	//			and its initial value is the hash of the  
	//			target_line.
	//
	//		2)	Every time when two hashes are 'merged' and 
	//			re-hashed to form a new node in the tree 
	//			(during roots_ traversal) the code checks 
	//			if one of these hashes matches the target value.
	//			If it does, then both of the hashes are added
	//			to the hash chain, such that the hash, which
	//			matched the target value is added first.
	//			After that we know that the new node resulting
	//			from hashing the above two together has to be 
	//			the next value, which we add to the chain. Thus
	//			it will be set to be the next target value.
	//
	//
	if(input_file.is_open())
	{
		std::string line; 
		while( std::getline(input_file, line) )
		{
			// Get the hash of the line (a new leaf)
			std::string leaf = hash(line);
			
			// Loop over the complete-tree-forest roots
			for (uint i=0; i<roots_.size(); ++i)
			{
				if( roots_[i] == "" )
				{
					// roots_[i] = leaf;
					std::swap(roots_[i],leaf);
					break;
				}
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
						chain.push_back( std::make_pair(0,roots_[i]) ); // First push the one which 
						chain.push_back( std::make_pair(1,leaf) );		// corresponds to the target.
						leaf = hash(roots_[i],leaf);
						roots_[i] = "";
						target = leaf;
					}
					else
					{
						chain.push_back( std::make_pair(1,leaf) );		// First push the one which 
						chain.push_back( std::make_pair(0,roots_[i]) ); // corresponds to the target.
						leaf = hash(roots_[i],leaf);
						roots_[i] = "";
						target = leaf;
					}
				}
			}
			if(leaf != "" )
			{
				roots_.push_back(leaf);
				leaf = "";
			}
		} // end while
	}
	input_file.close();
	

	// Now merge the complete-tree-forest from 
	// right-to-left. Start by finding the first
	// non-empty string in the roots_ vector.
	std::vector<std::string>::iterator it = std::find_if(roots_.begin(), roots_.end(), [](std::string s){ return s != ""; } );

	// This will be initial value for the root
	root = *it;

	// Move the iterator
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
				chain.push_back( std::make_pair(0,*it) ); 	// First push the one which 
				chain.push_back( std::make_pair(1,root) );	// corresponds to the target.
				root = hash(*it,root);
				target = root;
			}
			else
			{
				chain.push_back( std::make_pair(1,root) ); 	// First push the one which 
				chain.push_back( std::make_pair(0,*it) );	// corresponds to the target.
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
} 




// --- selfConsistentHashChain --> a method for verifying if the hash chain is self-consistent
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
}





#endif // MERKLE_HASHER_HPP
