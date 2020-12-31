/*
Copyright 2012-2014 Adam Jordan (xylamic@gmail.com)

This file is part of Comic Seer.

Comic Seer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

Comic Seer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Comic Seer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicState.hpp>
#include <XUtilities.hpp>
#include <ComicExceptions.hpp>

using namespace ComicSeer;
using namespace std;

const char* TestFileName = "ComicStateTest.ini";

void UnloadReloadViaIni()
{
    ComicState* state = ComicState::State();

    XIni ini;
    state->StoreState(ini);
    ini.Save(TestFileName);

    state->Reinitialize();

    XIni ini2(TestFileName);
    state->LoadState(ini2);
}

TEST(ComicState, Instance)
{
	ComicState* state = ComicState::State();
	EXPECT_TRUE(state != NULL);

    EXPECT_EQ(OnePage, state->DefaultImageView());
    EXPECT_EQ(Manual, state->DefaultImageState());
    EXPECT_EQ(2, state->TwoPageMoveNumber());
    EXPECT_EQ(size_t(0), state->LibraryDirectories().size());
    EXPECT_EQ(false, state->ShowPageNumbers());
    EXPECT_EQ(1, state->PageNumberLocation());
    EXPECT_STREQ("#006600", state->PageNumberColor().c_str());
}

TEST(ComicState, LibrariesNone)
{
	ComicState* state = ComicState::State();
    state->Reinitialize();

	map<string, string> libs = state->LibraryDirectories();
	EXPECT_EQ(0, libs.size());
}

TEST(ComicState, LibrariesAdd)
{
	ComicState* state = ComicState::State();
    state->Reinitialize();

	state->AddLibraryDirectory("name1", "path1");
	state->AddLibraryDirectory("name3", "path2");
	state->AddLibraryDirectory("name2", "path3");

	map<string, string> libs = state->LibraryDirectories();
	EXPECT_EQ(3, libs.size());

	map<string, string>::const_iterator it = libs.begin();
	EXPECT_EQ(string("name1"), it->first);
	EXPECT_EQ(string("path1"), it->second);

	it++;
	EXPECT_EQ(string("name2"), it->first);
	EXPECT_EQ(string("path3"), it->second);

	it++;
	EXPECT_EQ(string("name3"), it->first);
	EXPECT_EQ(string("path2"), it->second);
}

TEST(ComicState, LibrariesRemove)
{
	ComicState* state = ComicState::State();
    state->Reinitialize();

	state->AddLibraryDirectory("name1", "path1");
	state->AddLibraryDirectory("name3", "path2");
	state->AddLibraryDirectory("name2", "path3");

	state->RemoveLibraryDirectory("name2");
	state->RemoveLibraryDirectory("name1");

	map<string, string> libs = state->LibraryDirectories();
	EXPECT_EQ(1, libs.size());

	map<string, string>::const_iterator it = libs.begin();
	EXPECT_EQ(string("name3"), it->first);
	EXPECT_EQ(string("path2"), it->second);
}

TEST(ComicState, LibraryNameAlreadyExist)
{
	ComicState* state = ComicState::State();
    state->Reinitialize();

	state->AddLibraryDirectory("name1", "path1");
	state->AddLibraryDirectory("name3", "path2");
	state->AddLibraryDirectory("name2", "path3");

	ASSERT_THROW(state->AddLibraryDirectory("name1", "path4"), ComicLibraryAlreadyExistsException);
}

TEST(ComicState, LibraryPathAlreadyExist)
{
	ComicState* state = ComicState::State();
    state->Reinitialize();

	state->AddLibraryDirectory("name1", "path1");
	state->AddLibraryDirectory("name3", "path2");
	state->AddLibraryDirectory("name2", "path3");

	ASSERT_THROW(state->AddLibraryDirectory("name4", "path1"), ComicLibraryAlreadyExistsException);
}

TEST(ComicState, IniStoreRetrieve)
{
	ComicState* state = ComicState::State();
    state->Reinitialize();

	state->AddLibraryDirectory("name1", "path1");
	state->AddLibraryDirectory("name3", "path2");
	state->AddLibraryDirectory("name2", "path3");

    UnloadReloadViaIni();

	map<string, string> libs = state->LibraryDirectories();
	EXPECT_EQ(3, libs.size());

	map<string, string>::const_iterator it = libs.begin();
	EXPECT_EQ(string("name1"), it->first);
	EXPECT_EQ(string("path1"), it->second);

	it++;
	EXPECT_EQ(string("name2"), it->first);
	EXPECT_EQ(string("path3"), it->second);

	it++;
	EXPECT_EQ(string("name3"), it->first);
	EXPECT_EQ(string("path2"), it->second);
}

TEST(ComicState, DefaultImageState)
{
    ComicState* state = ComicState::State();

    state->SetDefaultImageState(Manual);
    EXPECT_EQ(Manual, state->DefaultImageState());
    UnloadReloadViaIni();
    EXPECT_EQ(Manual, state->DefaultImageState());

    state->SetDefaultImageState(FitWidth);
    EXPECT_EQ(FitWidth, state->DefaultImageState());
    UnloadReloadViaIni();
    EXPECT_EQ(FitWidth, state->DefaultImageState());

    state->SetDefaultImageState(FitHeight);
    EXPECT_EQ(FitHeight, state->DefaultImageState());
    UnloadReloadViaIni();
    EXPECT_EQ(FitHeight, state->DefaultImageState());

    state->SetDefaultImageState(FitWidthHeight);
    EXPECT_EQ(FitWidthHeight, state->DefaultImageState());
    UnloadReloadViaIni();
    EXPECT_EQ(FitWidthHeight, state->DefaultImageState());
}

TEST(ComicState, DefaulteImageView)
{
    ComicState* state = ComicState::State();

    state->SetDefaultImageView(OnePage);
    EXPECT_EQ(OnePage, state->DefaultImageView());
    UnloadReloadViaIni();
    EXPECT_EQ(OnePage, state->DefaultImageView());

    state->SetDefaultImageView(TwoPage);
    EXPECT_EQ(TwoPage, state->DefaultImageView());
    UnloadReloadViaIni();
    EXPECT_EQ(TwoPage, state->DefaultImageView());
}

TEST(ComicState, TwoPageMoveNumber)
{
    ComicState* state = ComicState::State();

    state->SetTwoPageMoveNumber(1);
    EXPECT_EQ(1, state->TwoPageMoveNumber());
    UnloadReloadViaIni();
    EXPECT_EQ(1, state->TwoPageMoveNumber());

    state->SetTwoPageMoveNumber(2);
    EXPECT_EQ(2, state->TwoPageMoveNumber());
    UnloadReloadViaIni();
    EXPECT_EQ(2, state->TwoPageMoveNumber());

    state->SetTwoPageMoveNumber(0);
    EXPECT_EQ(2, state->TwoPageMoveNumber());
    UnloadReloadViaIni();
    EXPECT_EQ(2, state->TwoPageMoveNumber());

    state->Reinitialize();
    state->SetTwoPageMoveNumber(4);
    EXPECT_EQ(2, state->TwoPageMoveNumber());
    UnloadReloadViaIni();
    EXPECT_EQ(2, state->TwoPageMoveNumber());
}

TEST(ComicState, TwoPageWideAutodetect)
{
    ComicState* state = ComicState::State();
    state->Reinitialize();

    EXPECT_TRUE(state->TwoPageAutodetectWidePage());

    state->SetTwoPageAutodetectWidePage(false);
    EXPECT_FALSE(state->TwoPageAutodetectWidePage());
    UnloadReloadViaIni();
    EXPECT_FALSE(state->TwoPageAutodetectWidePage());

    state->SetTwoPageAutodetectWidePage(true);
    EXPECT_TRUE(state->TwoPageAutodetectWidePage());
    UnloadReloadViaIni();
    EXPECT_TRUE(state->TwoPageAutodetectWidePage());
}

TEST(ComicState, ColorBackground)
{
    ComicState* state = ComicState::State();
    state->Reinitialize();

    EXPECT_STREQ("#000000", state->BackgroundColor().c_str());

    state->SetBackgroundColor("#102030");
    EXPECT_STREQ("#102030", state->BackgroundColor().c_str());
    UnloadReloadViaIni();
    EXPECT_STREQ("#102030", state->BackgroundColor().c_str());

    state->SetBackgroundColor("#302010");
    EXPECT_STREQ("#302010", state->BackgroundColor().c_str());
    UnloadReloadViaIni();
    EXPECT_STREQ("#302010", state->BackgroundColor().c_str());

    state->SetBackgroundColor("#8484848");
    EXPECT_STREQ("#302010", state->BackgroundColor().c_str());
    UnloadReloadViaIni();
    EXPECT_STREQ("#302010", state->BackgroundColor().c_str());

    state->SetBackgroundColor("8484848");
    EXPECT_STREQ("#302010", state->BackgroundColor().c_str());
    UnloadReloadViaIni();
    EXPECT_STREQ("#302010", state->BackgroundColor().c_str());
}

TEST(ComicState, ReopenLastSession)
{
    ComicState* state = ComicState::State();
    state->Reinitialize();

    EXPECT_TRUE(state->ReopenLastSession());

    state->SetReopenLastSession(false);
    EXPECT_FALSE(state->ReopenLastSession());
    UnloadReloadViaIni();
    EXPECT_FALSE(state->ReopenLastSession());

    state->SetReopenLastSession(true);
    EXPECT_TRUE(state->ReopenLastSession());
    UnloadReloadViaIni();
    EXPECT_TRUE(state->ReopenLastSession());
}

TEST(ComicState, ThumbnailSize)
{
    ComicState* state = ComicState::State();
    state->Reinitialize();

    EXPECT_EQ(150, state->ThumbnailSize());

    state->SetThumbnailSize(75);
    EXPECT_EQ(75, state->ThumbnailSize());
    UnloadReloadViaIni();
    EXPECT_EQ(75, state->ThumbnailSize());

    state->SetThumbnailSize(70);
    EXPECT_EQ(75, state->ThumbnailSize());
    UnloadReloadViaIni();
    EXPECT_EQ(75, state->ThumbnailSize());

    state->SetThumbnailSize(700);
    EXPECT_EQ(75, state->ThumbnailSize());
    UnloadReloadViaIni();
    EXPECT_EQ(75, state->ThumbnailSize());

    state->SetThumbnailSize(150);
    EXPECT_EQ(150, state->ThumbnailSize());
    UnloadReloadViaIni();
    EXPECT_EQ(150, state->ThumbnailSize());

    state->SetThumbnailSize(600);
    EXPECT_EQ(600, state->ThumbnailSize());
    UnloadReloadViaIni();
    EXPECT_EQ(600, state->ThumbnailSize());
}

TEST(ComicState, CombineAll)
{
    ComicState* state = ComicState::State();

    state->Reinitialize();
    state->AddLibraryDirectory("lib1", "path1\\//\\//\\");
    state->SetDefaultImageState(FitWidth);
    state->SetTwoPageMoveNumber(1);
    state->SetDefaultImageView(OnePage);
    state->SetBackgroundColor("#FFFFFF");
    state->SetTwoPageAutodetectWidePage(false);
    state->SetThumbnailSize(300);
    state->SetReopenLastSession(true);
    state->SetPageNumberSettings(true, ComicSeer::Top, "#123456");

    EXPECT_EQ(FitWidth, state->DefaultImageState());
    EXPECT_EQ(OnePage, state->DefaultImageView());
    EXPECT_EQ(1, state->LibraryDirectories().size());
    EXPECT_EQ(1, state->TwoPageMoveNumber());
    EXPECT_STREQ("#FFFFFF", state->BackgroundColor().c_str());
    EXPECT_FALSE(state->TwoPageAutodetectWidePage());
    EXPECT_EQ(300, state->ThumbnailSize());
    EXPECT_TRUE(state->ReopenLastSession());
    EXPECT_EQ(true, state->ShowPageNumbers());
    EXPECT_EQ(0, state->PageNumberLocation());
    EXPECT_STREQ("#123456", state->PageNumberColor().c_str());

    UnloadReloadViaIni();

    EXPECT_EQ(FitWidth, state->DefaultImageState());
    EXPECT_EQ(OnePage, state->DefaultImageView());
    EXPECT_EQ(1, state->LibraryDirectories().size());
    EXPECT_EQ(1, state->TwoPageMoveNumber());
    EXPECT_STREQ("#FFFFFF", state->BackgroundColor().c_str());
    EXPECT_EQ(false, state->TwoPageAutodetectWidePage());
    EXPECT_EQ(300, state->ThumbnailSize());
    EXPECT_TRUE(state->ReopenLastSession());
    EXPECT_EQ(true, state->ShowPageNumbers());
    EXPECT_EQ(0, state->PageNumberLocation());
    EXPECT_STREQ("#123456", state->PageNumberColor().c_str());
}
