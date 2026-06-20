#include "file_io.h"

#include <expected>
#include <fstream>
#include <iostream>

/*
   Free wrapper functions around C++ IO
*/

std::expected<std::string, ReadFileError> read_entire_file(
    const std::string& filename) noexcept {
  try {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
      return std::unexpected(
          ReadFileError(FileErrorCode::FileNotFound, "Failed to open file"));
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    if (size < 0) {
      return std::unexpected(ReadFileError(FileErrorCode::FileSizeError,
                                           "Unable to read file size"));
    }

    file.seekg(0, std::ios::beg);

    std::string contents;
    contents.resize(size);

    if (!file.read(contents.data(), size)) {
      return std::unexpected(
          ReadFileError(FileErrorCode::ReadFailed, "Failed to read file"));
    }

    return contents;
  } catch (const std::bad_alloc&) {
    return std::unexpected(
        ReadFileError(FileErrorCode::OutOfMemory, "Out of memory"));
  } catch (const std::exception& e) {
    return std::unexpected(ReadFileError(FileErrorCode::Unknown, e.what()));
  }
}
