#pragma once
#include <log4cpp/log4cpp.hpp>
#include <baseload/utility/Utility.hpp>
#include <baseload/loader/Loader.hpp>
#include <memory>

namespace mdalvz {

	namespace baseload {

		class App : public NoCopy {
			
		private:

			constexpr static char TAG[] = "baseload::App";

			std::shared_ptr<log4cpp::Logger> L;

			std::shared_ptr<Loader> loader;

		public:

			void run(int argc, const char** argv);

		};

	}

}
