#include <iostream>
#include <string>

#include "myHashInterface.hpp"
#include "merkelHasher.hpp"



int main( int argc, char **argv )
{


	MerkelHasher<sha256,myHashMerge> myHasher;

	const std::string s1("LogTest1");
	const std::string s2("LogTest2");
	
	std::string h1 = myHasher.hash(s1);
	std::string h2 = myHasher.hash(s2);
	std::string h3 = myHasher.hash(h1,h2);



	std::cout << "hash(" << s1 << "):\t" << h1 << std::endl;
	std::cout << "hash(" << s2 << "):\t" << h2 << std::endl;
	std::cout << "hash( hash(" << s1 << ")+hash(" << s2 << ") ):\t" << h3 << std::endl;

	return 0 ;

}