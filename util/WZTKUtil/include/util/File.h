/*
 * QuteCom, a voice over Internet phone
 * Copyright (C) 2010 Mbdsys
 *
 */

#ifndef OWFILE_H
#define OWFILE_H

#include "util/Interface.h"
#include "util/StringList.h"
#include <string>
#include <fstream>

/**
 * \defgroup FILE_PATH 文件和路径操作
 * \brief 常见文件和路径操作
 * \{
 * \}
 */

/**
 * \brief File interface.
 * \ingroup FILE_PATH
 *
 * An abstract representation of file and directory pathnames.
 *
 * TODO use FileNotFoundException?
 *
 * @see QFile
 * @see java.io.*
 */
class File : NonCopyable 
{
public:
	struct FileTimes
	{
		time_t last_access_time;
		time_t last_modify_time;
		time_t create_time;
	};
	/**
	 * Filename encoding.
	 */
	enum Encoding 
	{
		EncodingDefault,
		EncodingUTF8
	};

	File(const std::string & filename, Encoding encoding = File::EncodingDefault);

	File(const File & file);

	virtual ~File() 
	{

	}
	
	File & operator=(const File & file);

	/**
	 * Gets the file extension.
	 *
	 * @return the file extension or empty string if there's no extension
	 */
	std::string getExtension() const;

	/**
	 * Moves a file.
	 *
	 * @param newPath the path we want to move the file to
	 * @param overwrite true to overwrite the file if it exists.
	 *
	 * @return true if the operation succeeds otherwise false.
	 */
	bool move(const std::string & newPath, bool overwrite = false);

	/**
	 * Removes a file or a directory recursively.
	 * 
	 * @return true if the operation succeeds otherwise false.
	 */
	bool remove();

	/**
	 * \brief 删除文件夹下的所有内容，但不删除自己这个目录。
	 * 
	 * \return true if the operation succeeds otherwise false.
	 */
	void removeDirContentsButThis();

	/**
	 * Copies a file or a directory recursively.
	 *
	 * @param dstPath destination path. Will be created if does not exist
	 * @return true if operation successful
	 */
	bool copy(const std::string & path);

	/**
	 * Copy a single file.
	 *
	 * @param dst destination path or file
	 * @param src source
	 * @return true if successful
	 */
	static bool copyFile(const std::string & dst, const std::string & src,bool isBinary=true);

	/**
	 * Gets the path to the file.
	 *
	 * The path does not contain the filename, it stops at the last /
	 *
	 * @return path to the file
	 */
	std::string getPath() const;

	/**
	 * Gets the full path to the file.
	 *
	 * @return path to the file
	 */
	std::string getFullPath() const;

	/**
	* Gets the name of the file without its path.
	*
	* @return the name of the file without its path
	*/
	std::string getFileName() const;

	/**
	 * Gets directory list.
	 *
	 * @return a list of directories contained in 'this' directory
	 */
	StringList getDirectoryList() const;

	/**
	 * Gets file list.
	 *
	 * TODO Not implemented yet.
	 *
	 * @return a list of files contained in 'this' directory
	 */
	StringList getFileList() const;

	StringList getFileListRecursive() const;

	/**
	 * 递归查找此文件夹下，以及每一个子文件夹。查看是否有至少一个文件
	 *
	 * return 如果文件夹内至少有一个文件，那么返回true.否则返回false.
	 */
	bool isEmptyFolderRecursive() const;

	/**
	 * Gets the file size.
	 *
	 * @return the file size in bytes.
	 */
	unsigned getSize() const;

	FileTimes getTimes() const;
	/**
	 * Gets pathName with the '/' separators converted to separators that are appropriate for the underlying operating system.
	 *
	 * On Windows, convertPathSeparators("c:/winnt/system32") returns "c:\winnt\system32".
	 *
	 * @param path path to convert
	 * @return path converted
	 */
	static std::string convertPathSeparators(const std::string & path);

	/**
	 * Gets path with either '/' or '\' separators converted to '/' separators
	 */
	static std::string convertToUnixPathSeparators(const std::string & path);

	/**
	 * Gets the native directory separator: "/" under Unix (including Mac OS X) and "\" under Windows.
	 *
	 * @return native system path separator
	 */
	static std::string getPathSeparator();

	/**
	 * 递归创建目录，如果目录已经存在则不做任何事情。
	 * 如果传入的参数是一个目录，结尾则一定要加反斜杠。
	 * 如 File::createPath("C:\\User\\Admistrator\\SB\\SB2.txt")会尝试
	 * - 创建C:
	 * - 创建C:\\User
	 * - 创建C:\\User\\Admistrator
	 * - 创建C:\\User\\Admistrator\\SB
	 *
	 * @param path the path to create
	 */
	static void createPath(const std::string & path);

	/**
	 * Creates a temporary file.
	 *
	 * @return the temporary file
	 */
	static File createTemporaryFile();

	/**
	 * @return true if the given path exists.
	 */
	static bool exists(const std::string & path);

	/**
	 * @return true if the file is a directory.
	 */
	static bool isDirectory(const std::string & filename);

protected:

	std::string _filename;

	File::Encoding _encoding;
};


/**
 * Interface for FileReader and FileWriter.
 *
 * Code factorization.
 *
 * @author Tanguy Krotoff
 */
class IFile : Interface 
{
public:

	/**
	 * Closes the file.
	 */
	virtual void close() = 0;

protected:

	/**
	 * Opens the file for writing or reading.
	 *
	 * @return true if success; false otherwise
	 */
	virtual bool open() = 0;

	/**
	 * Gets if the file is open or not.
	 *
	 * @return true if file open; false otherwise
	 */
	virtual bool isOpen() /*const*/ = 0;
};


/**
 * Reads from a file.
 *
 * @author Tanguy Krotoff
 */
class FileReader : public File, public IFile {
public:

	 FileReader(const std::string & filename);

	 FileReader(const FileReader & fileReader);

	 FileReader(const File & file);

	 ~FileReader();

	 bool open();

	/**
	 * Reads data from the file.
	 *
	 * You must call open() first and check the returned value otherwise
	 * it will make an assertion (a crash).
	 *
	 * @return data read from the file
	 */
	 std::string read();

	 void close();

private:

	bool isOpen();

	std::ifstream _file;
};


/**
 * Writes to a file.
 *
 * @author Tanguy Krotoff
 */
class FileWriter : public File, public IFile {
public:

	 FileWriter(const std::string & filename, bool binaryMode = true);

	 FileWriter(const FileWriter & fileWriter, bool binaryMode = true);

	 FileWriter(const File & file, bool binaryMode = true);

	 ~FileWriter();

	 bool open();
	/**
	 * Writes data to the file.
	 *
	 * You must call open() first and check the returned value otherwise
	 * it will make an assertion (a crash).
	 *
	 * @param data data to write to the file
	 */
	 void write(const std::string & data);

	 void close();

	/**
	 * Set a value to _appendMode
	 *
	 * WARNING : can NOT be used when the file is already open !!
	 *
	 * WHY THIS IS NOT INSIDE THE CONSTRUCTOR SINCE IT CANNOT BE CHANGED LATER ON???
	 *
	 * @param newAppendMode : value wanted
	 * @return value obtained
	 */
	 bool setAppendMode(bool appendMode);

private:


	bool isOpen();

	std::ofstream _file;

	bool _fileOpen;

	bool _binaryMode;

	bool _appendMode;
};

/**
 * \}
 */
#endif	//OWFILE_H
