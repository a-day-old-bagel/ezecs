/*
 * Copyright (c) 2016 Galen Cochrane
 * Galen Cochrane <galencochrane@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef ECS_HELPERS_H
#define ECS_HELPERS_H

#include <string>
#include "ecsState.h"

#define _FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define ECS_ERR_MSG(res, msg) EcsResult(_FILENAME, __LINE__, res, msg)
#define ECS_ERR(res) EcsResult(_FILENAME, __LINE__, res)
#define ECS_MSG(msg) EcsResult(_FILENAME, __LINE__, -1, msg)
#define ECS_SUCCESS EcsResult();

#define ECS_CHECK_ERR_MSG(res, msg) if (res != SUCCESS) return ECS_ERR_MSG(res, msg)
#define ECS_CHECK_ERR(res) if (res != SUCCESS) { return ECS_ERR(res); }
#define ECS_CHECK_MSG(res, msg) if (res != SUCCESS) { return ECS_MSG(msg); }

namespace ecs {
  std::string resolveErrorToString(CompOpReturn err);

  struct EcsResult {
    int lineNumber;
    CompOpReturn errCode;
    std::string fileName, message;
    EcsResult(const char* message = "");
    EcsResult(const char *fileName, int lineNumber, CompOpReturn errCode, const char *message = "");
    std::string toString();
    inline bool isError() { return errCode != SUCCESS; }
  };
}

#endif //ECS_HELPERS_H
