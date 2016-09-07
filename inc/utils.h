/*
 * utils.h
 *
 *  Created on: Sep 7, 2016
 *      Author: idkiller
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

namespace pius {
namespace utils {

void LogD(const char* fmt, ...);

int InitializeConfigs(const std::string& appName);

std::string GetResource(const std::string& name);


}  // namesapce utils
}  // name

#endif /* UTILS_H_ */
