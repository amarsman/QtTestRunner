/*
Copyright (C) 2015 Henk van der Laak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H

#include <QString>

/******************************************************************************/
/** \brief Struct that holds all test options */
struct TestSettings
{
    TestSettings() {
        basepath     = ".";
        recursive    = false;
        nrjobs       = 1;
        debug        = false;
        useldd       = true;
        shuffle      = true;
        repeat       =  1;
        isolated     = true;
        jhextensions = true;
        verbosity    = 1;
        no_colors    = false;
        graphical    = true;
    }

    QString basepath;    /*!< \brief Basepath where to look for tests */
    bool recursive;      /*!< \brief Look for tests recursively */
    int nrjobs;          /*!< \brief Run test in n threads */
    bool debug;          /*!< \brief Print debug info */
    bool useldd;         /*!< \brief Use ldd to determine if a file is a QTest */
    bool shuffle;        /*!< \brief Shuffle test order */
    int  repeat;         /*!< \brief Repeat tests n times */
    bool isolated;       /*!< \brief Run test functions one at a time */
    bool jhextensions;   /*!< \brief Use JH extensions (multi testcases) */
    int  verbosity;      /*!< \brief Set verbosity level at n */
    bool no_colors;      /*!< \brief Do not produce colored output */
    bool graphical;      /*!< \brief Use GUI */
};

/******************************************************************************/

#endif // TEST_SETTINGS_H
