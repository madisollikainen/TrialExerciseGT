/**	
 *	Simple helper functions for parsing commandline arguments
 * 	with option flags.
 */

#ifndef READCMD_HPP
#define READCMD_HPP


#include <stdlib.h>
#include <algorithm>
#include <fstream>


// Function for checking if a option flag exists
bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

// Function for retriving an option/input value corresponding to an option flag
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
	    return *itr;
	}
	return 0;
}


#endif // READCMD_HPP

