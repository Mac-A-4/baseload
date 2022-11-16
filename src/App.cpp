#include <baseload/App.hpp>
#include <string>

namespace mdalvz {

	namespace baseload {

		void App::run(int argc, const char** argv) {

			L = std::make_shared<log4cpp::Logger>(log4cpp::Level::VERBOSE);
			L->add(std::make_shared<log4cpp::ConsoleWriter>());
			
			if (argc != 3) {
				L->e(TAG, "Invalid number of arguments specified");
				return;
			}
			std::string moduleName = argv[1];
			L->i(TAG, "moduleName = ", moduleName);
			std::string processName = argv[2];
			L->i(TAG, "processName = ", processName);

			L->i(TAG, "Initializing loader");
			loader = std::make_shared<Loader>(L);
			L->i(TAG, "Executing loader");
			loader->load(processName, moduleName);
			L->i(TAG, "Exiting");
			L->flush();

		}

	}

}
