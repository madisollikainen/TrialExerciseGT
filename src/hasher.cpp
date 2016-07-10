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
	 *	--leaves <file>	If given, save the leafs into <file_name> 				
	 * 
	 *
	 */

	// Initialise variables

	std::string log_file;
	std::string log_signature_file;
	// std::string hash_chain_file;
	std::string hash_chain_lines_file;
	std::string leaves_file;

	bool SIGN=false;
	bool HASH_CHAIN=false;
	bool LEAVES=false;
	// bool VERIFY=false;

	// Get the VERSION and EXE_NAME which and generate NAME_HEAD and USAGE_MESSAGE
	std::string NAME_HEAD = "Guardtime trial excersise by Madis Ollikainen.\nVersion: " + std::string(VERSION);
	std::string USAGE_MESSAGE = "Usage: \n\t./" + std::string(EXE_NAME) + " -i <log_file path> (--leaves)\n\t./"
												// + std::string(EXE_NAME) + " -i <log_file path> --sign (--leaves)\n\t./" 
												+ std::string(EXE_NAME) + " -i <log_file path> --chain <lines_file> (--leaves)\n\t./"
												+ std::string(EXE_NAME) + " -i <log_file path> --chain <lines_file> --sign (--leaves)";

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
	// HELP_MESSAGE +=  "\t-l\t<line_string>\tThe log file line, for which to\n\t\t\t\tget the hash chain. Required if\n\t\t\t\t--chain flag is used.\n";
	HELP_MESSAGE +=	 "\n";
	HELP_MESSAGE +=  "\t--sign\t\t\tIf given, then the log file\n\t\t\t\twill be signed.\n";
	HELP_MESSAGE +=  "\t--chain <lines_file>\tIf given, then the hash chains\n\t\t\t\tcorresponding to lines in <lines_file>\n\t\t\t\twill be retrived.\n";
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

	// --- sign or chain option parsing --- //
	if(cmdOptionExists(argv, argv+argc, "--sign") )
	{
		SIGN=true; 
	}
	if(cmdOptionExists(argv, argv+argc, "--chain") )
	{
		HASH_CHAIN=true;
		char * tmp = getCmdOption(argv, argv + argc, "--chain"); 
		hash_chain_lines_file = std::string(tmp);

		// // If hash chain is requested, search for the line
		// if(cmdOptionExists(argv, argv+argc, "-l") )
		// {
		// 	char * tmp = getCmdOption(argv, argv + argc, "-l"); 
		// 	hash_chain_lines_file = std::string(tmp);
		// 	hash_chain_file = log_file + ".hash_chain";
		// }
		// else
		// {
		// 	std::cout << "\nMissing log file line for which to extract hash chain!" << std::endl;
		// 	std::cout << "For more details see: -h or --help" << std::endl; 
		// }
	}
	// By default only sign
	if( !SIGN && !HASH_CHAIN )
	{
		SIGN=true;
	}

	// --- leaves saving option --- //
	if(cmdOptionExists(argv, argv+argc, "--leaves") )
	{
		LEAVES=true;
		leaves_file = log_file + ".leaves";
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
		std::string root; // In case signing is in order

		// Open the lines file for the hash chain
		std::ifstream lines(hash_chain_lines_file);
		if( lines.is_open() )
		{
			// Loop the lines
			std::string line;
			int line_nr = 0; 
			while( std::getline(lines, line) )
			{	
				line_nr++;
				// Information massage 
				std::cout << "Calculating hash chain number " + std::to_string(line_nr) + " ... ";

				// Calculate the hash chain
				hash_chain_t hash_chain_out = myHasher.getHashChain(log_file, line, LEAVES);
				
				// Check if the hash chain is not empty
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
					
					// Print the hash chain
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


					// If --leaves was active, print the leaves (do it only once)
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

						// Now set LEAVES to false (no need to print it again)
						LEAVES=false;
					}

					// // If --sign was also active, get the Merkle tree root
					// // from the hash chain and sign it. (do it only once)
					// if(SIGN)
					// {
					// 	// Information massage 
					// 	std::cout << "Signing the Merkle root ... "; 

					// 	std::ofstream signature_out(log_signature_file);
					// 	if(signature_out.is_open())
					// 	{
					// 		// The last entry in the hash_chain_out corresponds to the root 
					// 		signature_out << signature( hash_chain_out.back().second ) << std::endl;
					// 	}
					// 	signature_out.close();

					// 	// Information massage 
					// 	std::cout << "completed" << std::endl;

					// 	// Now set SIGN to false (no need to print it again)
					// 	SIGN=false;

					// } // END SIGN
				}
			} 
		}
		lines.close();
	} // END HASH_CHAIN
	if(SIGN)
	{
		// Information massage 
		std::cout << "Calculating the Merkle root ... "; 
		
		// Calculate the root of the Merkle tree of the log file
		std::string root = myHasher.getRoot(log_file, LEAVES);

		// Information massage 
		std::cout << "completed" << std::endl;
		std::cout << "Signing the Merkle root ... ";

		// Output the signed Merkle root
		std::ofstream signature_out(log_signature_file);
		if(signature_out.is_open())
		{
			signature_out << signature( root ) << std::endl;
		}
		signature_out.close();

		// Information massage 
		std::cout << "completed" << std::endl;

		// If --leaves was active, print the leaves
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

