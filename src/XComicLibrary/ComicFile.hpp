/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICFILE_HPP
#define COMICFILE_HPP

#include <Xylasoft.hpp>
#include <XByteArray.hpp>
#include <XException.hpp>
#include <boost/smart_ptr.hpp>
#include <list>
#include "ComicFileName.hpp"
#include "ComicInfo.hpp"

namespace ComicSeer
{
    class ComicFile
    {
	public:
		/*!
		Enumeration defining the type of comic file.
		*/
		enum ComicFileType
		{
			/*! Zip or CBZ file. */
			Zip,

			/*! Rar or CBR file. */
			Rar,

            /*! Image file. */
            Image,

#ifdef PDFSUPPORT
            /*! PDF file. */
            Pdf,
#endif

			/*! Unknown or not a comic file. */
			Unknown
		};

    public:
        /*!
        @brief Virtual destructor for abstract class.
        */
        virtual ~ComicFile();

        /*!
        Initialize the comic file and read the contents of the file.
       */
        void Initialize();

        /*!
        Get the number of pages in the comic.
       */
        uint32_t NumberOfPages() const;

        /*!
        @brief Get the comic file type.
        @return The file type.
        */
        virtual ComicFileType GetFileType() const = 0;

        /*!
        Get the title image for the comic.
        @param[in] data The array to write to.
       */
        void GetTitleImage(XByteArray& data) const;

		/*!
		Get the comic information in the comic file.
		@param[in] content The content of the comic information file.
		@returns True if comic information was found and retrieved, false otherwise.
		*/
		bool GetComicInformation(XByteArray& content);

        /*!
        @brief Get the comic information file. This will throw an exception
        if no file is present, the file cannot be read, or the file cannot be
        parsed.
        @return The comic information file.
        */
        ComicInfo GetComicInformation();

		/*!
		Get the specified file for the comic.
		@param[in] name The name of the file.
		@param[in] data The array to write to.
		*/
		void GetFile(const char* name, XByteArray& data) const;

		/*!
		Get the list of image file names in the book.
		@returns The list to populate with the names.
		*/
		std::list<std::string> ImageList() const;

		/*!
		Get the list of non-image file names in the book.
		@returns The list to populate with the names.
		*/
		std::list<std::string> NonImageList() const;

        /*!
        Get the specified file path.
        @returns The file path.
       */
        std::string FilePath() const;

        /*!
        @brief Says whether this archive contains a comic information file.
        @return True if present, false otherwise.
        */
        bool HasComicInformationFile() const;

    public:
        /*!
        Create a comic file instance.
        @param[in] filepath The path to the comic file.
       */
        static ComicFile* CreateComicFile(const char* filepath);

        /*!
        Dispose of a comic file instance.
        @param[in] comic The instance to dispose.
       */
        static void DisposeComicFile(ComicFile* comic);

		/*!
		Classify the given file extension as a file type.
		@param[in] extension The file extension.
		@returns The comic fille type.
		*/
		static ComicFileType ClassifyFileExtension(const char* extension);

    protected:
        /*!
        Constructor for abstract base class.
        @param[in] filepath The path to the comic file.
       */
        ComicFile(const char* filepath);

        /*!
        Get the list of files within the comic file.
        @returns The list of file names.
       */
        virtual std::list<std::string> GetFileList() const = 0;

        /*!
        Read the specified file.
        @param[in] name The name of the file.
        @param[out] data The array to write the content to.
       */
        virtual void ReadFile(const char* name, XByteArray& data) const = 0;

    private:
        std::string _filepath;

		std::list<std::string> _imageEntries;
		std::list<std::string> _altEntries;
        std::string _comicInfoPresent;
    };
}

#endif // COMICFILE_H
