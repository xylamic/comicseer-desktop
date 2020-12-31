/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include <iostream>
#include "gtest/gtest.h"

using namespace std;

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    if (RUN_ALL_TESTS() != 0)
    {
        cout << "Test run failed, press any key to end." << endl;
        cin.get();
        return -1;
    }
    else
    {
        cout << "\nPress any key to end..." << endl;
        cin.get();
        return 0;
    }
}
