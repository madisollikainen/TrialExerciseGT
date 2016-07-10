#include <iostream>
#include <string>

#include "myHashInterface.hpp"
#include "merkleHasher.hpp"

std::string self_hash(const std::string str)
{
	return str;
}

int main( int argc, char **argv )
{

	std::string infile(argv[1]);
	MerkleHasher<sha256,myHashMerge> myHasher;
	// MerkleHasher<self_hash,myHashMerge> myHasher;

	std::vector<std::string> leafs;

	std::string root = myHasher.getRoot(infile, leafs);

	std::cout << "Leafs: " << std::endl;
	for (uint i = 0; i<leafs.size(); ++i ){ std::cout << leafs[i] << std::endl;}
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "Root: " << root << std::endl;
	
	hash_chain_t hc_3 = myHasher.getHashChain(infile, std::string(argv[2]) );
	for (uint i=0; i < hc_3.size(); ++i)
	{
		std::cout << hc_3[i].first << "\t|\t" << hc_3[i].second << std::endl;

	}

	if ( myHasher.selfConsistentHashChain(hc_3))
	{
		std::cout << "Hash chain hc_3 is self-consistant" << std::endl; 
	}
	else
	{
		std::cout << "Hash chain hc_3 is NOT self-consistant" << std::endl; 
	}

	hash_chain_t hc_3_7 = hc_3;
	hc_3_7[0].second = myHasher.hash("7");
	
	if ( myHasher.selfConsistentHashChain(hc_3_7))
	{
		std::cout << "Hash chain hc_3_7 is self-consistant" << std::endl; 
	}
	else
	{
		std::cout << "Hash chain hc_3_7 is NOT self-consistant" << std::endl; 
	}

	hc_3_7[0].second = myHasher.hash("2");
	
	if ( myHasher.selfConsistentHashChain(hc_3_7))
	{
		std::cout << "Hash chain hc_3_7 is self-consistant" << std::endl; 
	}
	else
	{
		std::cout << "Hash chain hc_3_7 is NOT self-consistant" << std::endl; 
	}



	// const std::string s1("LogTest1");
	// const std::string s2("LogTest2");
	
	// std::string h1 = myHasher.hash(s1);
	// std::string h2 = myHasher.hash(s2);
	// std::string h3 = myHasher.hash(h1,h2);



	// std::cout << "hash(" << s1 << "):\t" << h1 << std::endl;
	// std::cout << "hash(" << s2 << "):\t" << h2 << std::endl;
	// std::cout << "hash( hash(" << s1 << ")+hash(" << s2 << ") ):\t" << h3 << std::endl;

	// std::cout<<std::endl;
	// std::vector<std::string> vs{"1","2","3","4", "5", "6", "7"};
	// for( uint i =0; i<vs.size(); ++i ) 
	// { 
	// 	myHasher.test(vs[i]); 
	// }


	
		

	return 0 ;

}