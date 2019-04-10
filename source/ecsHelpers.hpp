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

#pragma once

#include <cstring>
#include <string>
#include "ecsState.generated.hpp"
#include "topics.hpp"

#define EZECS_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define EZECS_ERR_MSG(res, msg) ezecs::EzecsResult(EZECS_FILENAME, __LINE__, res, msg)
#define EZECS_ERR(res) ezecs::EzecsResult(EZECS_FILENAME, __LINE__, res)
#define EZECS_MSG(msg) ezecs::EzecsResult(EZECS_FILENAME, __LINE__, -1, msg)
#define EZECS_SUCCESS ezecs::EzecsResult();

#define EZECS_REQUIRE_REPORT(res, msg) if (res != SUCCESS) return EZECS_ERR_MSG(res, msg)
#define EZECS_REQUIRE(res) if (res != SUCCESS) { return EZECS_ERR(res); }
#define EZECS_REPORT(res, msg) if (res != SUCCESS) { return EZECS_MSG(msg); }

// #define EZECS_CHECK_PRINT(res) if (res.isError()) { printf("%s\n", res.toString().c_str()); }
#define EZECS_CHECK_PRINT(res) if (res.isError()) { publishf("err", "%s\n", res.toString().c_str()); }

namespace ezecs {
  std::string resolveErrorToString(CompOpReturn err);

  struct EzecsResult {
    int lineNumber;
    CompOpReturn errCode;
    std::string fileName, message;
    EzecsResult(const char* message = "");
    EzecsResult(const char *fileName, int lineNumber, CompOpReturn errCode, const char *message = "");
    std::string toString();
    inline bool isError() { return errCode != SUCCESS; }
  };
}
