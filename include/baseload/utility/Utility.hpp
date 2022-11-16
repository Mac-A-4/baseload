#pragma once

namespace mdalvz {
	
	namespace baseload {

		struct NoCopy {

			NoCopy() = default;

			virtual ~NoCopy() = default;

			NoCopy(const NoCopy&) = delete;

			NoCopy& operator=(const NoCopy&) = delete;

		};

	}

}
