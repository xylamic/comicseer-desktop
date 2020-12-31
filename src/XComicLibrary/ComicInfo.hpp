/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICINFO_HPP
#define COMICINFO_HPP

#include <Xylasoft.hpp>
#include <XByteArray.hpp>

namespace ComicSeer
{
    /*!
    @brief The ComicInfo class builds upon the ComicInfo XSD format to read the corresponding
    XML files, which can be present in comic book archive files. This file is typically called
    "ComicInfo.xml"
    */
    class ComicInfo
    {
    public:
        /*!
        @brief The YesNo enumeration is used for optional meta-data.
        */
        enum YesNo
        {
            /*! Value is undefined (default). */
            UNDEFINED,

            /*! Yes. */
            YES,

            /*! No. */
            NO
        };

    public:
        /*!
        @brief Construct the comic information.
        @param data The file data to construct with.
        throws XmlFileReadException If the file cannot be read.
        */
        ComicInfo(const XByteArray &data);

        std::string Title() const;
        std::string Series() const;
        std::string Number() const;
        int Count() const;
        int Volume() const;
        std::string AlternateSeries() const;
        std::string AlternateNumber() const;
        int AlternateCount() const;
        std::string Summary() const;
        std::string Notes() const;
        int Year() const;
        int Month() const;
        std::string Writer() const;
        std::string Penciller() const;
        std::string Inker() const;
        std::string Colorist() const;
        std::string Letterer() const;
        std::string CoverArtist() const;
        std::string Editor() const;
        std::string Publisher() const;
        std::string Imprint() const;
        std::string Genre() const;
        std::string Web() const;
        int PageCount() const;
        std::string LanguageISO() const;
        std::string Format() const;
        std::string Characters() const;
        std::string Teams() const;
        std::string Locations() const;
        YesNo IsBlackAndWhite() const;
        YesNo IsManga() const;

    private:
        std::string _title;
        std::string _series;
        std::string _number;
        int _count;
        int _volume;
        std::string _alternateSeries;
        std::string _alternateNumber;
        int _alternateCount;
        std::string _summary;
        std::string _notes;
        int _year;
        int _month;
        std::string _writer;
        std::string _penciller;
        std::string _inker;
        std::string _colorist;
        std::string _letterer;
        std::string _coverArtist;
        std::string _editor;
        std::string _publisher;
        std::string _imprint;
        std::string _genre;
        std::string _web;
        int _pageCount;
        std::string _languageISO;
        std::string _format;
        std::string _characters;
        std::string _teams;
        std::string _locations;
        YesNo _isBlackAndWhite;
        YesNo _isManga;
    };
}
#endif // COMICINFO_HPP
