/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICPAGETYPES_HPP
#define COMICPAGETYPES_HPP

namespace ComicSeer
{
    /*!
    @brief The ComicPageTypes class contains the enumeration used in defining comic page options.
    */
    class ComicPageTypes
    {
    public:
        /*!
        @brief The PageRotationType enum is utilizes to describe the rotation of a page.
        */
        enum PageRotationType
        {
            /*! No rotation. */
            NONE = 0,

            /*! 90 degree rotation. */
            DEGREE90,

            /*! 180 degree rotation. */
            DEGREE180,

            /*! 270 degree rotation. */
            DEGREE270
        };
    };
}
#endif // COMICPAGETYPES_HPP
