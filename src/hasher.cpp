/*	Author: Madis Ollikainen
 *	File:	main.cpp
 *
 *	The main function for my prototype log file 
 *	signing and verification system.
 *
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

	/**
	 *	Read the commandline options:
	 *
	 *	-h	(--help) 		Produces help message
	 *	-v 	(--version)		Prints the code version
	 *						defined by the git version.
	 *
	 *	-i 	<file_name>		The log file (REQUIRED)
	 *	-l 	<line_string>	The log file line, for which to 
	 * 						get the hash chain. Required if
	 *						--chain flag is used.
	 *
	 *	--sign 				If given, then the log file 
	 *						will be signed.
	 *	--chain 			If given, then the hash chain
	 *						corresponding to -l <line_string>
	 *						will be retrived. 				
	 * 
	 *
	 */

	// Initialise variables

	std::string log_file;
	std::string log_signature_file;
	std::string hash_chain_file;
	std::string hash_chain_line;


	bool SIGN=false;
	bool HASH_CHAIN=false;
	bool VERIFY=false;

	// Get the VERSION and EXE_NAME which and generate NAME_HEAD and USAGE_MESSAGE
	std::string NAME_HEAD = "Guardtime trial excersise by Madis Ollikainen.\nVersion: " + std::string(VERSION);
	std::string USAGE_MESSAGE = "Usage: \n\t./" + std::string(EXE_NAME) + " -i <log_file path> --sign\n\t./" 
												+ std::string(EXE_NAME) + " -i <log_file path> -l <line_string> --chain\n\t./"
												+ std::string(EXE_NAME) + " -i <log_file path> -l <line_string> --sign --chain";

	// Combine the HELP_MESSAGE
	std::string HELP_MESSAGE = "\n";
	HELP_MESSAGE +=	NAME_HEAD;
	HELP_MESSAGE +=	 "\n\n";
	HELP_MESSAGE +=	USAGE_MESSAGE;
	HELP_MESSAGE +=	 "\n\nOptions:\n";
	HELP_MESSAGE +=  "\t-h\t(--help)\tProduces help message\n";
	HELP_MESSAGE +=  "\t-v\t(--version)\tPrints the code version\n\t\t\t\tdefined by the git version.\n";
	HELP_MESSAGE +=	 "\n";
	HELP_MESSAGE +=  "\t-i\t<file_name>\tThe log file (REQUIRED).\n";
	HELP_MESSAGE +=  "\t-l\t<line_string>\tThe log file line, for which to\n\t\t\t\tget the hash chain. Required if\n\t\t\t\t--chain flag is used.\n";
	HELP_MESSAGE +=	 "\n";
	HELP_MESSAGE +=  "\t--sign\t\t\tIf given, then the log file\n\t\t\t\twill be signed.\n";
	HELP_MESSAGE +=  "\t--chain\t\t\tIf given, then the hash chain\n\t\t\t\tcorresponding to -l <line_string>\n\t\t\t\twill be retrived.\n";
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

	// --- sign or chain option parsing --- //
	if(cmdOptionExists(argv, argv+argc, "--sign") )
	{
		SIGN=true; 
	}
	if(cmdOptionExists(argv, argv+argc, "--chain") )
	{
		HASH_CHAIN=true;

		// If hash chain is requested, search for the line
		if(cmdOptionExists(argv, argv+argc, "-l") )
		{
			char * tmp = getCmdOption(argv, argv + argc, "-l"); 
			hash_chain_line = std::string(tmp);
			hash_chain_file = log_file + ".hash_chain";
		}
		else
		{
			std::cout << "\nMissing log file line for which to extract hash chain!" << std::endl;
			std::cout << "For more details see: -h or --help" << std::endl; 
		}
	}
	// By default only sign
	if( !SIGN && !HASH_CHAIN )
	{
		SIGN=true;
	}


	/*
	 *	Dependent on the commandline arguments, now run the code.
	 */

	// Generate a MerkleHasher instance using the sha256 and myHashMerge
	// functions defined in myHashInterface.hpp
	MerkleHasher<sha256,myHashMerge> myHasher;


	// If --chain, then generate the hash chain
	if(HASH_CHAIN)
	{
		// Calculate the hash chain
		hash_chain_t hash_chain_out = myHasher.getHashChain(log_file, hash_chain_line);
		
		// Check if the hash chain is not empty
		if(hash_chain_out.empty())
		{
			std::cout << "The inputed line is not present in the log file!" << std::endl;
			return -1;
		}

		// Print the hash chain
		std::ofstream hc_out(hash_chain_file);
		if(hc_out.is_open())
		{
			for (uint i=0; i<hash_chain_out.size(); ++i)
			{
				hc_out << hash_chain_out[i].first << "\t" << hash_chain_out[i].second << std::endl; 
			}
		}
		hc_out.close();

		// If --sign was also active, get the Merkle tree root
		// from the hash chain and sign it.
		if(SIGN)
		{
			std::ofstream signature_out(log_signature_file);
			if(signature_out.is_open())
			{
				// The last entry in the hash_chain_out corresponds to the root 
				signature_out << signature( hash_chain_out.back().second ) << std::endl;
			}
			signature_out.close();
		} // END SIGN
	} // END HASH_CHAIN
	else if(SIGN)
	{
		// Initialise the leafs vector
		std::vector<std::string> leafs;

		// Calculate the root of the Merkle tree of the log file
		std::string root = myHasher.getRoot(log_file, leafs);

		// Output the signed Merkle root
		std::ofstream signature_out(log_signature_file);
		if(signature_out.is_open())
		{
			signature_out << signature( root ) << std::endl;
		}
		signature_out.close();
	} // END SIGN




} // END MAIN

