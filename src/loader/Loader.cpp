#include <baseload/loader/Loader.hpp>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

#undef PROCESSENTRY32
#undef Process32First
#undef Process32Next

namespace mdalvz {

	namespace baseload {

		Loader::Loader(std::shared_ptr<log4cpp::Logger> L) {
			this->L = L;
		}

		static bool getProcessId(const std::string& processName, DWORD& result) {
			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (snapshot == INVALID_HANDLE_VALUE) {
				return false;
			}
			PROCESSENTRY32 entry = { 0 };
			entry.dwSize = sizeof(entry);
			for (BOOL status = Process32First(snapshot, &entry); status; status = Process32Next(snapshot, &entry)) {
				if (!_strcmpi(processName.c_str(), entry.szExeFile)) {
					CloseHandle(snapshot);
					result = entry.th32ProcessID;
					return true;
				}
			}
			CloseHandle(snapshot);
			return false;
		}

		static bool getModulePath(const std::string& moduleName, std::string& modulePath) {
			std::vector<char> buffer(MAX_PATH);
			while (true) {
				DWORD n = GetFullPathNameA(moduleName.c_str(), (DWORD)buffer.size(), buffer.data(), 0);
				if (n == 0) {
					return false;
				} else if (n == buffer.size()) {
					buffer.resize(n * 2);
				} else {
					buffer.resize(n);
					modulePath.assign(buffer.begin(), buffer.end());
					return true;
				}
			}
		}

		static bool createRemotePath(HANDLE processHandle, const std::string& modulePath, void*& address) {
			size_t size = modulePath.length() + 1;
			address = VirtualAllocEx(processHandle, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (!address) {
				return false;
			}
			SIZE_T bytesWritten;
			if (!WriteProcessMemory(processHandle, address, modulePath.c_str(), size, &bytesWritten)) {
				VirtualFreeEx(processHandle, address, 0, MEM_RELEASE);
				return false;
			}
			return true;
		}
		
		static bool loadRemoteLibrary(HANDLE processHandle, void* remotePathAddress) {
			HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
			if (!kernel32) {
				return false;
			}
			void* loadLibraryAddress = (void*)GetProcAddress(kernel32, "LoadLibraryA");
			if (!loadLibraryAddress) {
				return false;
			}
			HANDLE threadHandle = CreateRemoteThread(
				processHandle,
				0,
				0,
				(LPTHREAD_START_ROUTINE)loadLibraryAddress,
				remotePathAddress,
				0,
				0
			);
			if (!threadHandle) {
				return false;
			}
			WaitForSingleObject(threadHandle, INFINITE);
			return true;
		}

		bool Loader::load(const std::string& processName, const std::string& moduleName) {
			L->i(TAG, "Loading ", moduleName, " into process ", processName);
			DWORD processId;
			if (!getProcessId(processName, processId)) {
				L->e(TAG, "Failed to get process id (", GetLastError(), ")");
				return false;
			}
			L->i(TAG, "Found process id ", processId);
			HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
			if (!processHandle) {
				L->e(TAG, "Failed to get process handle (", GetLastError(), ")");
				return false;
			}
			L->i(TAG, "Opened process handle ", processHandle);
			std::string modulePath;
			if (!getModulePath(moduleName, modulePath)) {
				L->e(TAG, "Failed to resolve module path (", GetLastError(), ")");
				CloseHandle(processHandle);
				return false;
			}
			L->i(TAG, "Resolved module path ", modulePath);
			void* remotePathAddress;
			if (!createRemotePath(processHandle, modulePath, remotePathAddress)) {
				L->e(TAG, "Failed to create remote path (", GetLastError(), ")");
				CloseHandle(processHandle);
				return false;
			}
			L->i(TAG, "Created remote path at ", remotePathAddress);
			L->i(TAG, "Loading remote module");
			if (!loadRemoteLibrary(processHandle, remotePathAddress)) {
				L->e(TAG, "Failed to load remote module (", GetLastError(), ")");
				VirtualFreeEx(processHandle, remotePathAddress, 0, MEM_RELEASE);
				CloseHandle(processHandle);
				return false;
			}
			L->i(TAG, "Remote module loaded");
			VirtualFreeEx(processHandle, remotePathAddress, 0, MEM_RELEASE);
			CloseHandle(processHandle);
			return true;
		}

	}

}