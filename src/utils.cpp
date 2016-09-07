#include "utils.h"

#include <string>

#include <app_common.h>

namespace pius {
namespace utils {

static std::string LogTag("NotInitialized");
static const char PathSeparator = '/';
static bool IsInitialized = false;
static std::string ResourceRoot;

void LogD(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	dlog_vprint(DLOG_ERROR, LogTag, fmt, args);
	va_end(args);
}

int InitializeConfigs(const std::string& appName)
{
	if (IsInitialized) return 0;
	LogTag = appName;

	char *resRoot = app_get_resource_path();
	if (resRoot == nullptr)
	{
		LogD("Can't get resource directory");
		return 1;
	}
	ResourceRoot = std::string(resRoot);
	free(resRoot);

}

static std::string CombinePath(const std::string& path1, const std::string& path2)
{
	if (path1.back() == PathSeparator)
		return path1 + path2;
	else
		return path1 + PathSeparator + path2;
}

std::string GetResource(const std::string& name)
{
	return CombinePath(ResourceRoot, name);
}

}  // namespace utils
}  // namespace pius
