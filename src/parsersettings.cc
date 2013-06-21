#include "parsersettings.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "CocoaUtils.h"
#include <vector>

std::vector<std::string> librarypath;

void add_librarydir(const std::string &libdir)
{
	librarypath.push_back(libdir);
}

/*!
	Searces for the given file in library paths and returns the full path if found.
	Returns an empty path if file cannot be found or filename is a directory.
*/
std::string locate_file(const std::string &filename)
{
    return filename;
}

void parser_init(const std::string &applicationpath)
{

}
