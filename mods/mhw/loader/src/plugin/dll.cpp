#include "dll.h"

#include <MemoryModule.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "../loader/log.h"

namespace dll {
  using namespace loader;

  namespace fs = std::filesystem;
  using FilePath = fs::path;
  using FileTime = fs::file_time_type;
  using DLL = std::tuple<HMEMORYMODULE, FilePath, FileTime>;

  std::vector<DLL> dlls;

  static void* currentModule;

  HCUSTOMMODULE CustomLoadLibrary(const char* path, void* _) {
    if (std::string(path) == "loader.dll") {
      return currentModule;
    }
    return LoadLibraryA(path);
  }

  FARPROC CustomGetProcAddress(HCUSTOMMODULE target, const char* path, void* _) {
    if (target == currentModule) {
      return MemoryGetProcAddress(target, path);
    }
    return GetProcAddress((HMODULE)target, path);
  }

  auto LoadDll(const char* path) {
    std::ifstream dll(path, std::ios::binary);
    std::vector<char> dllRead(std::istreambuf_iterator<char>(dll), {});

    size_t size = dllRead.size();
    char* allocatedMem = (char*)malloc(size);
    memcpy(allocatedMem, &dllRead[0], size);

    return MemoryLoadLibraryEx(allocatedMem,
                               size,
                               MemoryDefaultAlloc,
                               MemoryDefaultFree,
                               CustomLoadLibrary,
                               CustomGetProcAddress,
                               MemoryDefaultFreeLibrary,
                               nullptr);
  }

  void liveReloading() {
    for (auto& dll : dlls) {
      auto filePath = std::get<FilePath>(dll);
      auto writeTime = fs::last_write_time(filePath);

      if (writeTime != std::get<FileTime>(dll)) {
        // TODO: consider comparing file hash
        LOG(INFO) << "Reloading " << filePath;
        MemoryFreeLibrary(std::get<HMEMORYMODULE>(dll));
        auto memModule = LoadDll(filePath.string().c_str());

        if (!memModule) {
          LOG(ERR) << "Failed to load " << filePath;
        }
        std::get<HMEMORYMODULE>(dll) = memModule;
        std::get<FileTime>(dll) = writeTime;
      }
    }
  }

  void LoadAllPluginDlls(void* memModule) {
    currentModule = memModule;
    for (auto& entry : std::filesystem::directory_iterator("nativePC\\plugins")) {
      std::string name = entry.path().filename().string();
      if (entry.path().filename().extension().string() != ".dll")
        continue;
      LOG(INFO) << "Loading plugin " << entry.path();
      auto dll = LoadDll(entry.path().string().c_str());
      if (!dll) {
        LOG(ERR) << "Failed to load " << entry.path();
      } else {
        dlls.push_back({ dll, entry.path(), entry.last_write_time() });
      }
    }
  }

  void watch(std::stop_token token) {
    while (!token.stop_requested()) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      liveReloading();
    }
  }
}  // namespace dll