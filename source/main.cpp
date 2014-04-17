#include "SparCraft.h"
#include <boost/program_options.hpp>
#include "Map.h"

int main(int argc, char *argv[])
{
	SparCraft::init();

	try
	{
		if(argc>=2){
			std::string experimentArg, configArg, stateArg, mapArg;

			boost::program_options::options_description desc("Allowed options");
			desc.add_options()("help,h", "prints this help message")
            		          ("experiment,e", boost::program_options::value<std::string>(&experimentArg)->default_value("search"), "set experiment")
            		          ("config,c", boost::program_options::value<std::string>(&configArg), "config file")
            		          ("state,s", boost::program_options::value<std::string>(&stateArg), "state description file")
            		          ("map,m", boost::program_options::value<std::string>(&mapArg), "map file (replaces the one in config file)")
            		          ;
			boost::program_options::positional_options_description pd;
			pd.add("config", 1);
			pd.add("state", 1);
			pd.add("map", 1);
			boost::program_options::variables_map vm;
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pd).run(), vm);
			boost::program_options::notify(vm);

			if (vm.count("help")) {
				std::cout << desc << std::endl;
				return 1;
			}
			if(vm.count("config")!=1){
				SparCraft::System::FatalError("Please provide experiment file");
			}
			SparCraft::SearchExperiment exp(configArg);
			if(vm.count("state")>0){
			    std::stringstream ss;
			    ss<<"State StateDescriptionFile 1 "<<stateArg;
			    exp.addState(ss.str());
			}
			if(vm.count("map")>0){
			    std::stringstream ss;
			    SparCraft::Map *map = new SparCraft::Map();
			    map->load(mapArg);
			    exp.setMap(map);
			}
			if(experimentArg.compare("search")==0){
				exp.runExperiment();
			}else{
				SparCraft::System::FatalError("Error parsing arguments");
			}
		}
		else
		{
			SparCraft::System::FatalError("Please provide at least experiment file as only argument");
		}
	}
	catch(int e)
	{
		if (e == SparCraft::System::SPARCRAFT_FATAL_ERROR)
		{
			std::cerr << "\nSparCraft FatalError Exception, Shutting Down\n\n";
		}
		else
		{
			std::cerr << "\nUnknown Exception, Shutting Down\n\n";
		}
	}

	return 0;
}
