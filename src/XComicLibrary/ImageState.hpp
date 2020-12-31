/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_IMAGESTATE_HPP
#define X_IMAGESTATE_HPP

#include <Xylasoft.hpp>

namespace ComicSeer
{
    /*!
    Enumeration defining the image state.
    */
    enum ImageState
    {
        /*! Enter manual move mode. */
        Manual = 0,

        /*! Fit to the width of the view. */
        FitWidth,

        /*! Fit to the height of the view */
        FitHeight,

        /*! Fit to the width and height of the view. */
        FitWidthHeight
    };
}

#endif // IMAGESTATE_HPP
