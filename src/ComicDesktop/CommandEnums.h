/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef KEYCOMMANDTYPE_H
#define KEYCOMMANDTYPE_H

#include <Xylasoft.hpp>

class CommandEnums
{
public:
    /*!
    Enumeration defining a key command.
    */
    enum KeyCommandType
    {
        /*! The command is unknown. */
        UNIDENTIFIED = 0,

        /*! Quit command. */
        QUIT,

        /*! Home command. */
        HOME,

        /*! Full screen command */
        FULLSCREEN,

        /*! Move to the next page. */
        PAGEFORWARD,

        /*! Move to the previous page. */
        PAGEBACKWARD,

        /*! Fit width. */
        FITWIDTH,

        /*! Fit height. */
        FITHEIGHT,

        /*! Fit width & height. */
        FITALL,

        /*! Show/hide the library. */
        LIBRARY,

        /*! Change to the next tab. */
        TABCHANGE,

        /*! Open a comic. */
        OPEN,

        /*! Find a comic in the library. */
        FINDINLIBRARY,

        /*! Change to single page view. */
        ONEPAGE,

        /*! Change to two page horizontal. */
        TWOPAGE,

        /*! Show the recent list. */
        RECENTLIST,

        /*! Show the options screen. */
        OPTIONS,

        /*! Close the selected comic. */
        CLOSECOMIC,

        /*! Move to the first page of the comic. */
        FIRSTPAGE,

        /*! Move to the last page of the comic. */
        LASTPAGE,

        /*! Zoom in on the page. */
        ZOOMIN,

        /*! Zoom out on the page. */
        ZOOMOUT
    };
};

#endif // KEYCOMMANDTYPE_H
