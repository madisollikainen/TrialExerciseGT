#include <iostream>
#include <string>

#include "myHashInterface.hpp"
#include "merkelHasher.hpp"

std::string self_hash(const std::string str)
{
	return str;
}

int main( int argc, char **argv )
{

	std::string infile(argv[1]);
	MerkelHasher<sha256,myHashMerge> myHasher;
	// MerkelHasher<self_hash,myHashMerge> myHasher;

	std::vector<std::string> leafs;

	std::string root = myHasher.getRoot(infile, leafs);

	std::cout << "Leafs: " << std::endls;
	for (uint i = 0; i<leafs.size(); ++i ){ std::cout << leafs[i] << std::endl;}
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "Root: " << root << std::endl;
	

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