#pragma once

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(A) (sizeof(A)/sizeof((A)[0]))
#endif

#ifndef OUT
#define OUT
#endif

#include <vector>
#include <string>


/**
 * \defgroup ZIPS 压缩和解压缩
 * \brief 压缩和解压缩
 * \{
 */

/**
 * \brief 将lpszFilePath指定的ZIP文件释放到目标目录lpszUnzipPath中。ReleaseFiles作为输出参数记录了已释放的所有文件（全路径）。
 * \return 成功返回TRUE否则返回FALSE。
 */
BOOL UnzipFileToPath(LPCSTR lpszFilePath, LPCSTR lpszUnzipPath, OUT std::vector<std::string>& ReleaseFiles);

/**
 * \}
 */