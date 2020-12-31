/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICUTILITIES_HPP
#define COMICUTILITIES_HPP

#include <Xylasoft.hpp>
#include <list>
#include <map>
#include <vector>

namespace ComicSeer
{
    class ComicUtilities
    {
    public:
		/*!
		Get the list of supported image file extensions.
		@returns The list of file extensions.
		*/
		static std::list<std::string> SupportedImageExtensions();

		/*!
		Get the directory for file storage.
		@returns The directory ending in a slash.
		*/
		static std::string StorageDirectory();

		/*!
		Get the directory for temporary file storage.
		@returns The directory ending in a slash.
		*/
		static std::string TemporaryDirectory();

		/*!
		Get the scale for the specified image size and maximum sizes while keeping aspect ratio.
		@param[in] imageWidth The image width.
		@param[in] imageHeight The image height.
		@param[in] maxWidth The maximum width.
		@param[in] maxHeight The maximum height.
		@returns The scale factor to use.
		*/
		static double ScaleKeepAspectRatio(int imageWidth, int imageHeight, int maxWidth, int maxHeight);

        /*!
        @brief Save the specified list of comics to the specified path.
        @param names The list of names of comics.
        @param path The path to save to.
        */
        static void SaveComicList(const std::map<std::string, std::vector<std::string> >& names, const char* path);

    private:
        ComicUtilities();
    };
}

#endif // COMICUTILITIES_HPP
