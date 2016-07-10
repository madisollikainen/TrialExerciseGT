/**
 *	Author: Madis Ollikainen
 *	File:	hasher.cpp
 *
 *	The hasher commandline tool, which implements 
 *	functionality for singing log files. It enables 
 *	the user to:
 *	
 *		a) 	Sign text files by singing the roots
 *			of the Merkle hash tree where the leaves
 *			correspond to the hashes of the text files
 *			liens.
 *
 *		b)	Extract leaf-to-root hash chains for any 
 *			line of a text file.
 *  
 *	Commandline options:
 *
 *	-h	(--help) 		Produces help message
 *	-v 	(--version)		Prints the code version
 *						defined by the git version.
 *
 *	-i 	<file_name>		The log file (REQUIRED)
 *
 *	--sign 				If given, then the log file 
 *						will be signed. (DEFAULT)
 *	--chain <file_name>	If given, then the hash chain
 *						corresponding to <file>
 *						will be retrived.
 *	--leaves 			If given, save the leafs into <file_name> 
 *
 */


#include <iostream>
#include <string>
#include <fstream>

#include "readcmd.hpp"
#include "myHashInterface.hpp"
#include "mySignatureInterface.hpp"
#include "merkleHasher.hpp"




int main( int argc, char **argv )
{

	// -------------------------------- //
	// ------ COMMANDLINE PARSING ----- //
	// -------------------------------- //

	// --- Initialise the variable to hold commandline options --- //

	//  Whether to generate the signature or not. 
	// If no other option is given, the signature is created.
	bool SIGN=false;			
	
	//  Whether to generate hash chains or not.
	bool HASH_CHAIN=false;

	//  Whether to save the leaves (hashes of lines) or not.
	bool LEAVES=false;

	// The path to the log file
	std::string log_file;

	// The file name for the log files signature.
	// At the moment will be log_file + ".signature"
	std::string log_signature_file;

	// The path to the file which holds the lines
	// for which to extract the hash chains
	std::string hash_chain_lines_file;

	// The file where to store the leaves.
	// At the moment will be log_file + ".leaves"
	std::string leaves_file;


	// --- Combining the VERSION/NAME message and the USAGE_MESSAGE --- //
	// The VERSION and EXE_NAME are variables defined during compilation
	// for their details see the Makefile. 
	#ifdef TEST
		std::string NAME_HEAD = "Guardtime trial excersise by Madis Ollikainen. Test version with identity hash function.\nCode version (git): " + std::string(VERSION);
	#else
		std::string NAME_HEAD = "Guardtime trial excersise by Madis Ollikainen. Production version with SHA256 hash function.\nCode version (git): " + std::string(VERSION);
	#endif
	std::string USAGE_MESSAGE = "Usage: \n\t./" + std::string(EXE_NAME) + " -i <log_file path> (--leaves)\n\t./"
												+ std::string(EXE_NAME) + " -i <log_file path> --chain <lines_file> (--leaves)\n\t./"
												+ std::string(EXE_NAME) + " -i <log_file path> --chain <lines_file> --sign (--leaves)";

	// --- Combining the HELP_MESSAGE --- //
	std::string HELP_MESSAGE = "\n";
	HELP_MESSAGE +=	NAME_HEAD;
	HELP_MESSAGE +=	 "\n\n";
	HELP_MESSAGE +=	USAGE_MESSAGE;
	HELP_MESSAGE +=	 "\n\nOptions:\n";
	HELP_MESSAGE +=  "\t-h\t(--help)\tProduces help message\n";
	HELP_MESSAGE +=  "\t-v\t(--version)\tPrints the code version\n\t\t\t\tdefined by the git version.\n";
	HELP_MESSAGE +=	 "\n";
	HELP_MESSAGE +=  "\t-i\t<file_name>\tThe log file (REQUIRED).\n";
	HELP_MESSAGE +=	 "\n";
	HELP_MESSAGE +=  "\t--sign\t\t\tIf given, then the log file\n\t\t\t\twill be signed. (DEFAULT)\n";
	HELP_MESSAGE +=  "\t--chain <file_name>\tIf given, then the hash chains\n\t\t\t\tcorresponding to lines in <file_name>\n\t\t\t\twill be retrived.\n";
	HELP_MESSAGE +=  "\t--leaves\t\tIf given, save the leaves.\n";
	HELP_MESSAGE +=	 "\n";

	// --- Help message parsing --- //
	if(cmdOptionExists(argv, argv+argc, "-h") || cmdOptionExists(argv, argv+argc, "--help") )
	{
		std::cout << HELP_MESSAGE << std::endl; 
	}

	// --- Version message parsing --- //
	if(cmdOptionExists(argv, argv+argc, "-v") || cmdOptionExists(argv, argv+argc, "--version") )
	{
		std::cout << NAME_HEAD << std::endl; 
	}

	// --- Log file name parsing --- //
	if(cmdOptionExists(argv, argv+argc, "-i") )
	{
		char * tmp = getCmdOption(argv, argv + argc, "-i");
		log_file = std::string(tmp);
		log_signature_file = log_file + ".signature";
	}
	else
	{
		std::cout << "\nMissing log file path! Log file path is compulsory." << std::endl;
		std::cout << "For more details see: -h or --help" << std::endl;
		return -1;
	}

	// --- Signture and hash chain option parsing --- //
	if(cmdOptionExists(argv, argv+argc, "--sign") )
	{
		SIGN=true; 
	}
	if(cmdOptionExists(argv, argv+argc, "--chain") )
	{
		HASH_CHAIN=true;
		char * tmp = getCmdOption(argv, argv + argc, "--chain"); 
		hash_chain_lines_file = std::string(tmp);
	}
	if( !SIGN && !HASH_CHAIN )
	{
		// If neither is given, then just generate the signature
		SIGN=true; 
	}

	// --- Leaves saving option parsing --- //
	if(cmdOptionExists(argv, argv+argc, "--leaves") )
	{
		LEAVES=true;
		leaves_file = log_file + ".leaves";
	}


	// ---------------------------- //
	// ----- RUNNING THE CODE ----- //
	// ---------------------------- //

	// --- Constructing a MerkleHasher instance --- //
	#ifdef TEST
		// For test use the identity_hash and myHashMerge functions 
		MerkleHasher<identity_hash,myHashMerge> myHasher;
	#else
		// Ese the sha256 and myHashMerge functions
		MerkleHasher<sha256,myHashMerge> myHasher;
	#endif

	// --- Generating the hash chains if asked --- //
	if(HASH_CHAIN)
	{
		// Opening the lines file for the hash chain
		std::ifstream lines(hash_chain_lines_file);
		if( lines.is_open() )
		{
			// Looping the lines
			std::string line;
			int line_nr = 0; 
			while( std::getline(lines, line) )
			{	
				line_nr++;
				// Information massage 
				std::cout << "Calculating hash chain number " + std::to_string(line_nr) + " ... ";

				// Calculating the hash chain
				hash_chain_t hash_chain_out = myHasher.getHashChain(log_file, line, LEAVES);
				
				// Checking if the hash chain is empty or not
				if( (hash_chain_out.empty()) )
				{
					// Information massage 
					std::cout << "failed" << std::endl;
					std::cout << "Input line number " << line_nr << " :\n" << line << std::endl;
					std::cout << "\nThe line is not present in the log file!\n" << std::endl;
				}
				else
				{
					// Information massage 
					std::cout << "completed" << std::endl;
					
					// Printing the hash chain
					std::string hash_chain_file = log_file + ".hash_chain_" + std::to_string(line_nr);
					std::ofstream hc_out(hash_chain_file);
					if(hc_out.is_open())
					{
						for (uint i=0; i<hash_chain_out.size(); ++i)
						{
							hc_out << hash_chain_out[i].first << "\t" << hash_chain_out[i].second << std::endl; 
						}
					}
					hc_out.close();


					// If --leaves was active, printing the leaves (do it only once)
					if(LEAVES)
					{

						// Information massage 
						std::cout << "Printing leaves ... "; 
						
						std::vector<std::string> leaves = myHasher.getLeaves();
						std::ofstream leaves_out(leaves_file);
						if(leaves_out.is_open())
						{
							for (uint i=0; i<leaves.size(); ++i)
							{
								leaves_out << leaves[i] << std::endl; 
							}
						}
						leaves_out.close();


						// Information massage 
						std::cout << "completed" << std::endl;

						// Setting LEAVES to false (no need to print it again)
						LEAVES=false;
					} 
				}
			} 
		}
		lines.close();
	} // END HASH_CHAIN


	// --- Signing the Merkle root if asked --- //
	if(SIGN)
	{
		// Information massage 
		std::cout << "Calculating the Merkle root ... "; 
		
		// Calculating the root of the Merkle tree of the log file
		std::string root = myHasher.getRoot(log_file, LEAVES);

		// Information massage 
		std::cout << "completed" << std::endl;
		std::cout << "Signing the Merkle root ... ";

		// Outputing the signed Merkle root
		std::ofstream signature_out(log_signature_file);
		if(signature_out.is_open())
		{
			signature_out << signature( root ) << std::endl;
		}
		signature_out.close();

		// Information massage 
		std::cout << "completed" << std::endl;

		// If --leaves was active, printing the leaves
		if(LEAVES)
		{
			// Information massage 
			std::cout << "Printing leaves ... "; 
			
			std::vector<std::string> leaves = myHasher.getLeaves();
			std::ofstream leaves_out(leaves_file);
			if(leaves_out.is_open())
			{
				for (uint i=0; i<leaves.size(); ++i)
				{
					leaves_out << leaves[i] << std::endl; 
				}
			}
			leaves_out.close();

			// Information massage 
			std::cout << "completed" << std::endl;
		}

	} // END SIGN

} // END MAIN

