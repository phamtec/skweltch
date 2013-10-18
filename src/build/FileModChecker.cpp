
#include "FileModChecker.hpp"

#include <boost/filesystem.hpp>
#include <boost/crc.hpp>

using namespace std;
using namespace boost;
using namespace boost::filesystem;

typedef crc_optimal<64, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true>
  crc_64_type;

void doDirCrc(crc_64_type *crc, const path &p) {

	if (p.filename() == "bin") {
		// skip. No C++ files in here.
	}
	else {
		directory_iterator i = directory_iterator(p);	
		for (; i != directory_iterator(); i++) {			
			if (is_directory(*i)) {
				doDirCrc(crc, *i);
			}
			else {
				path f = i->path();
				if (f.extension() == ".cpp" || f.extension() == ".hpp") {
					stringstream ss;
					ss << f.filename() << last_write_time(f);
					string s = ss.str();
					if (s.length() >= 64) {
						throw new string("buf too large for CRC.");
					}
					else {
						char buf[64];
						strcpy(buf, s.c_str());
						crc->process_bytes(buf, s.length());
					}
				}
			}
		}
	}
	
}

long FileModChecker::getCrc(const string &pathstr) {

	crc_64_type crc;
	path p(pathstr);
	doDirCrc(&crc, p);
	return crc.checksum();
	
}
