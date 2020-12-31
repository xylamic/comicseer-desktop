/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "PdfComicFile.hpp"

#ifdef  PDFSUPPORT
#include "ComicExceptions.hpp"
#include <cstring>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/process.hpp>
#include <XUtilities.hpp>
#include "ComicExceptions.hpp"
#include <XException.hpp>

using namespace ComicSeer;
using namespace std;

#ifdef _WIN32
    #define GSNAME "gswin32c.exe"
    #include <Windows.h>
#else
    #define GSNAME "gs"
#endif

PdfComicFile::PdfComicFile(const char* filepath)
    : ComicFile(filepath)
{
}

void PdfComicFile::ValidateSupported()
{
    // create the arguments
    vector<string> args;
    args.push_back("--version");

    // execute
    XByteArray out;
    ExecuteGhostScript(args, out);

    // translate to double and check version
    string sout = out.ConstData();
    boost::algorithm::trim(sout);
    double version = ::atof(sout.c_str());
    if (version < 8.99)
    {
        throw GhostscriptInitException("The GhostScript version is not atleast version 9.");
    }
}

std::list<std::string> PdfComicFile::GetFileList() const
{
    // create the arguments
    vector<string> args;

    args.push_back("-q");
    args.push_back("-dNODISPLAY");
    args.push_back("-c");

    stringstream cmdArg;
    cmdArg << "(" << this->FilePath() << ") (r) file runpdfbegin pdfpagecount = quit";
    args.push_back(cmdArg.str());

    // execute GS
    XByteArray out;
    ExecuteGhostScript(args, out);

    // create the list
    list<string> entries;

    // get the number
    string sout = out.ConstData();
    boost::algorithm::trim(sout);
    int numPages = ::atoi(sout.c_str());

    // create the pages
    for (int i = 1; i <= numPages; i++)
    {
        stringstream pageStr;
        pageStr << std::setw(4) << std::setfill('0') << i << ".png";
        entries.push_back(pageStr.str());
    }

    return entries;
}

void PdfComicFile::ReadFile(const char* name, XByteArray& data) const
{
    // get the page number
    string indexStr = name;
    size_t extIndex = indexStr.find(".png");
    if (extIndex == indexStr.npos)
    {
        throw FileException("The requested file is invalid.");
    }
    indexStr = indexStr.substr(0, extIndex);
    int page = ::atoi(indexStr.c_str());
    if (page <= 0)
    {
        throw FileException("The requested file is not a valid page number.");
    }

    // create the arguments
    vector<string> args;
    args.push_back("-q");
    args.push_back("-dNOPAUSE");
    args.push_back("-dBATCH");
    args.push_back("-dSAFER");
    args.push_back("-sDEVICE=png16m");
    //args.push_back("-sDEVICE=jpeg");
    //args.push_back("-dJPEGQ=100");
    args.push_back("-r300");
    stringstream argFirstPage;
    argFirstPage << "-dFirstPage=" << page;
    args.push_back(argFirstPage.str());
    stringstream argLastPage;
    argLastPage << "-dLastPage=" << page;
    args.push_back(argLastPage.str());
    args.push_back("-sOutputFile=-");
    args.push_back(this->FilePath());

    /* TEST ARGS
    vector<string> args;
    args.push_back("-q");
    args.push_back("-dNOPAUSE");
    args.push_back("-dBATCH");
    args.push_back("-dSAFER");
    args.push_back("-sDEVICE=png16m");
    args.push_back("-r300");
    args.push_back("-dFirstPage=2");
    args.push_back("-dLastPage=2");
    args.push_back("-sOutputFile=-");//C:\\Users\\XYLAMIC\\Development\\XLMC-LAPX64\\src\\XComicLibraryTest\\Test1.png");
    args.push_back("C:\\Users\\XYLAMIC\\Development\\XLMC-LAPX64\\src\\XComicLibraryTest\\Intelligent Systems.pdf");
    */

    // execute GS
    ExecuteGhostScript(args, data);
}

string PdfComicFile::GetGhostscriptLocation()
{
    const char *gsProg = ::getenv("GS_PROG");
    if (gsProg != NULL && ::strlen(gsProg) > 0)
    {
        return gsProg;
    }

    return GSNAME;
}

void PdfComicFile::ReadProcStream(boost::process::pistream &stream, XByteArray &buffer)
{
    int c = stream.get();
    while (!stream.eof())
    {
        buffer.Append(c);
        c = stream.get();
    }
}

void PdfComicFile::ExecuteGhostScript(vector<std::string>& args, XByteArray &out)
{
    // get the executable location
    string execLocation = GetGhostscriptLocation();

    // add the exec arg
    args.insert(args.begin(), execLocation);

    // define the process context
    boost::process::context procContext;
    procContext.stdout_behavior = boost::process::capture_stream();
    procContext.stderr_behavior = boost::process::capture_stream();

    boost::scoped_ptr<boost::process::child> proc;
    try
    {
        proc.reset(new boost::process::child(boost::process::launch(execLocation, args, procContext)));
    }
    catch (std::exception& ex)
    {
        string err = "Failed to launch GhostScript: ";
        err.append(ex.what());

        throw GhostscriptLoadException(err.c_str());
    }
    catch (...)
    {
        throw GhostscriptLoadException("Failed to launch GhostScript.");
    }

    // get the streams
    boost::process::pistream &errStream = proc->get_stderr();
    boost::process::pistream &stream = proc->get_stdout();

    // cycle while running
    while (XUtilities::IsProcessRunning(proc->get_id()))
    {
        // yield for a very small period of time to prevent thrashing
        XUtilities::Sleep(5);

        // read the output string
        ReadProcStream(stream, out);
    }

    // read the error string after the process ended
    string err;
    int c = errStream.get();
    while (!errStream.eof())
    {
        err.push_back(c);
        c = errStream.get();
    }
    boost::algorithm::trim(err);

    // make sure no error occurred
    if (!err.empty())
    {
        throw GhostscriptReadException(err.c_str());
    }

    // read the output string for any remaining output
    ReadProcStream(stream, out);
}
#endif
