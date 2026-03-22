#include "api/memory/Hook.h"
#include <mutex>
#include <set>
#include <type_traits>
#include <unordered_map>

#include "Memory.h"

#include <android/log.h>
#include <chrono>
#include <inttypes.h>

#if defined(__arm__) || defined(__aarch64__)
#include "pl/Hook.h"
#include "pl/Signature.h"
#endif

#define LOGI(...)                                                              \
  __android_log_print(ANDROID_LOG_INFO, "LeviLogger", __VA_ARGS__)

namespace memory {

int hook(FuncPtr target, FuncPtr detour, FuncPtr *originalFunc,
         HookPriority priority, bool) {
#if defined(__arm__) || defined(__aarch64__)
  return pl::hook::pl_hook(target, detour, originalFunc,
                           pl::hook::Priority(priority));
#else
  return -1;
#endif
}

bool unhook(FuncPtr target, FuncPtr detour, bool) {
#if defined(__arm__) || defined(__aarch64__)
  return pl::hook::pl_unhook(target, detour);
#else
  return false;
#endif
}

// No hooks are installed on x86_64 (hook() returns -1), so this is a safe no-op
void unhookAll() {}

FuncPtr resolveIdentifier(char const *identifier) {
#if defined(__arm__) || defined(__aarch64__)
  return reinterpret_cast<FuncPtr>(pl::signature::resolveSignature(identifier));
#else
  return reinterpret_cast<FuncPtr>(resolveSignature(identifier));
#endif
}

FuncPtr resolveIdentifier(std::initializer_list<const char *> identifiers) {
  for (const auto &identifier : identifiers) {
    FuncPtr result = resolveIdentifier(identifier);
    if (result != nullptr) {
      return result;
    }
  }
  return nullptr;
}

} // namespace memory