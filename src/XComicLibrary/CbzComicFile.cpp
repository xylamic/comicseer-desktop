/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "CbzComicFile.hpp"
#include <fstream>
#include <XZip.hpp>

using namespace ComicSeer;
using namespace std;

CbzComicFile::CbzComicFile(const char* filepath)
    : ComicFile(filepath)
{
}

ComicFile::ComicFileType CbzComicFile::GetFileType() const
{
    return ComicFile::Zip;
}

std::list<std::string> CbzComicFile::GetFileList() const
{
	XZip zip(this->FilePath().c_str());

	list<string> entries;
	zip.FileEntries(entries);

	return entries;
}

void CbzComicFile::ReadFile(const char* name, XByteArray& data) const
{
	XZip zip(this->FilePath().c_str());

	zip.ReadFile(name, data);
}

/* QUAZIP IMPLEMENTATION
std::list<std::string> CbzComicFile::GetFileList() const
{
    QuaZip zip(QString::fromUtf8(this->FilePath().c_str()));
    if (!zip.open(QuaZip::mdUnzip))
    {
        throw FileException("The file could not be opened for read.");
    }

    list<string> entries;

    QStringList list = zip.getFileNameList();
    for_each (begin(list), end(list), [&entries](QString name)
    {
        entries.push_back(string(name.toUtf8()));
    });

    zip.close();

    return entries;
}

void CbzComicFile::ReadFile(const char* name, XByteArray& data) const
{
    QString qname = QString::fromUtf8(name);

    QuaZipFile zip(QString::fromUtf8(this->FilePath().c_str()), qname, QuaZip::csSensitive);
    if (!zip.open(QuaZipFile::ReadOnly))
    {
        throw FileException("The file could not be opened for read.");
    }

    QByteArray qdata = zip.readAll();

    data.Clear();
    data.Append(qdata.data(), qdata.size());

    zip.close();
}
*/
