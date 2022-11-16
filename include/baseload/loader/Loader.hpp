#pragma once
#include <log4cpp/log4cpp.hpp>
#include <baseload/utility/Utility.hpp>
#include <string>
#include <memory>

namespace mdalvz {

	namespace baseload {

		class Loader : public NoCopy {

		private:

			constexpr static char TAG[] = "baseload::Loader";

			std::shared_ptr<log4cpp::Logger> L;

		public:

			Loader(std::shared_ptr<log4cpp::Logger> L);

			bool load(const std::string& processName, const std::string& moduleName);

		};

	}

}
