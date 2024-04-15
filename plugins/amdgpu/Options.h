/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 */
#pragma once

#define ZSTD_STATIC_LINKING_ONLY
#define ZSTD_DISABLE_DEPRECATE_WARNINGS /* No deprecation warnings, pzstd itself is deprecated
                                         * and uses deprecated functions
                                         */
#include <zstd.h>
#undef ZSTD_STATIC_LINKING_ONLY

#include <string>
#include <vector>
#include <mutex>  
#include <condition_variable> 

#include "Options_CWrapper.h"

namespace pzstd {

//TODO：此处改变了Options中的几处类型。
struct Options {
  enum class WriteMode { Regular, Auto, Sparse };

  unsigned numThreads;
  unsigned maxWindowLog;
  unsigned compressionLevel;
  bool decompress;
  void* inputBuffer;
  uint64_t input_len;
  
  FILE* outputFile;
  bool overwrite;
  bool keepSource;
  WriteMode writeMode;
  bool checksum;
  int verbosity;
  int id;
  void* isLast;
  int data_type; // 0为主存数据，1为显存数据

  enum class Status {
    Success, // Successfully parsed options
    Failure, // Failure to parse options
    Message  // Options specified to print a message (e.g. "-h")
  };

  Options();
  Options(unsigned numThreads, unsigned maxWindowLog, unsigned compressionLevel,
          bool decompress, void* inputBuffer, uint64_t input_len,
          FILE *outputFile, bool overwrite, bool keepSource,
          WriteMode writeMode, bool checksum, int verbosity)
      : numThreads(numThreads), maxWindowLog(maxWindowLog),
        compressionLevel(compressionLevel), decompress(decompress),
        inputBuffer(inputBuffer), input_len(input_len), outputFile(outputFile),
        overwrite(overwrite), keepSource(keepSource), writeMode(writeMode),
        checksum(checksum), verbosity(verbosity) {}

  //TODO：此处将类main函数的参数进行更改，改为一个struct。
  Status parse(Options_Args args);

  ZSTD_parameters determineParameters() const {
    ZSTD_parameters params = ZSTD_getParams(compressionLevel, 0, 0);
    params.fParams.contentSizeFlag = 0;
    params.fParams.checksumFlag = checksum;
    if (maxWindowLog != 0 && params.cParams.windowLog > maxWindowLog) {
      params.cParams.windowLog = maxWindowLog;
      params.cParams = ZSTD_adjustCParams(params.cParams, 0, 0);
    }
    return params;
  }

};
}
