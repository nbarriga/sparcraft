#include "Common.h"
#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#endif
// SEARCH PARAMETERS
namespace SparCraft
{
    namespace System
    {


/** Print a demangled stack backtrace of the caller function to FILE* out. */
void printStackTrace(int skip, std::ostream &out, unsigned int max_frames)
{
#ifdef __linux__
    out << "stack trace:\n";

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0) {
	out << "  <empty, possibly corrupt>\n";
	return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(&(addrlist[skip]), addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++)
    {
	char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

	// find parentheses and +address offset surrounding the mangled name:
	// ./module(function+0x15c) [0x8048a6d]
	for (char *p = symbollist[i]; *p; ++p)
	{
	    if (*p == '(')
		begin_name = p;
	    else if (*p == '+')
		begin_offset = p;
	    else if (*p == ')' && begin_offset) {
		end_offset = p;
		break;
	    }
	}

	if (begin_name && begin_offset && end_offset
	    && begin_name < begin_offset)
	{
	    *begin_name++ = '\0';
	    *begin_offset++ = '\0';
	    *end_offset = '\0';

	    // mangled name is now in [begin_name, begin_offset) and caller
	    // offset in [begin_offset, end_offset). now apply
	    // __cxa_demangle():

	    int status;
	    char* ret = abi::__cxa_demangle(begin_name,
					    funcname, &funcnamesize, &status);
	    if (status == 0) {
		funcname = ret; // use possibly realloc()-ed string
		out<< "  "<<symbollist[i]<<" : "<<funcname<<"+"<<begin_offset<<std::endl;
	    }
	    else {
		// demangling failed. Output function name as a C function with
		// no arguments.
	        out<< "  "<<symbollist[i]<<" : "<<begin_name<<"()+"<<begin_offset<<std::endl;
	    }
	}
	else
	{
	    // couldn't parse the line? print the whole line.
	    out <<"  "<<symbollist[i]<<std::endl;
	}
    }

    free(funcname);
    free(symbollist);
#endif
}

        void FatalError(const std::string & errorMessage)
        {

            std::cerr << "\n\n\nSparCraft Fatal Error: \n\n\n      " << errorMessage << "\n\n";
        	printStackTrace(1);
            throw(SPARCRAFT_FATAL_ERROR);
        }
        
        void checkSupportedUnitType(const BWAPI::UnitType & type)
        {
            if (type == BWAPI::UnitTypes::None || type == BWAPI::UnitTypes::Unknown)
            {
                System::FatalError("Unknown unit type in experiment file, not supported");
            }

            if (type == BWAPI::UnitTypes::Protoss_Corsair || 
                type == BWAPI::UnitTypes::Zerg_Devourer || 
                type == BWAPI::UnitTypes::Zerg_Scourge ||
                type == BWAPI::UnitTypes::Terran_Valkyrie)
            {
                System::FatalError("Units with just air weapons currently not supported correctly: " + type.getName());
            }

//            if (type.isBuilding() && !(type == BWAPI::UnitTypes::Protoss_Photon_Cannon && type == BWAPI::UnitTypes::Zerg_Sunken_Colony && type == BWAPI::UnitTypes::Terran_Missile_Turret))
//            {
//                System::FatalError("Non-attacking buildings not currently supported: " + type.getName());
//            }

            if (type.isSpellcaster() && type!=BWAPI::UnitTypes::Terran_Medic)
            {
                System::FatalError("Spell casting units not currently supported: " + type.getName());
            }

            // Don't support units loading other units yet
            if (type == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine ||
            		type == BWAPI::UnitTypes::Protoss_Carrier ||
            		type == BWAPI::UnitTypes::Protoss_Interceptor ||
            		type == BWAPI::UnitTypes::Protoss_Reaver ||
            		type == BWAPI::UnitTypes::Protoss_Scarab ||
            		type == BWAPI::UnitTypes::Zerg_Broodling)
            {

            	System::FatalError("Units which have unit projectiles not supported: " + type.getName());
            }

        }

        bool isSupportedUnitType(const BWAPI::UnitType & type)
        {
        	if (type == BWAPI::UnitTypes::None || type == BWAPI::UnitTypes::Unknown)
        	{
        		return false;
        	}

        	if (type == BWAPI::UnitTypes::Protoss_Corsair ||
        			type == BWAPI::UnitTypes::Zerg_Devourer ||
        			type == BWAPI::UnitTypes::Zerg_Scourge ||
        			type == BWAPI::UnitTypes::Terran_Valkyrie)
        	{
        		return false;
        	}

//        	if (type.isBuilding() && !(type == BWAPI::UnitTypes::Protoss_Photon_Cannon || type == BWAPI::UnitTypes::Zerg_Sunken_Colony || type == BWAPI::UnitTypes::Terran_Missile_Turret))
//        	{
//        		return false;
//        	}

        	if (type.isSpellcaster() && type!=BWAPI::UnitTypes::Terran_Medic)
        	{
        		return false;
        	}

        	// Don't support units loading other units yet
        	if (type == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine ||
        			type == BWAPI::UnitTypes::Protoss_Carrier ||
        			type == BWAPI::UnitTypes::Protoss_Interceptor ||
        			type == BWAPI::UnitTypes::Protoss_Reaver ||
        			type == BWAPI::UnitTypes::Protoss_Scarab ||
        			type == BWAPI::UnitTypes::Zerg_Broodling)
        	{
        		return false;
        	}

        	return true;
        }

    }
};
