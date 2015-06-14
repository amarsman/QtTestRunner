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
#ifndef JHTESTSUITE_H
#define JHTESTSUITE_H
#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QStringList>

typedef QObject* (*ObjectFactory)();

/** \brief JhTestSuite groups testcases in a group and tests them */
class JhTestSuite : public QMap<QByteArray, ObjectFactory>
{
private:
    /** \brief Templated factory that creates an instance of T */
    template<class T> static QObject* objectFactory()
    {
        return new T();
    }

    /** \brief Factory that creates an instance by classname */
    QObject* createObject(const QByteArray &a_className) const
    {
        ObjectFactory factory = value(a_className);
        return (factory == NULL) ? NULL : (*factory)();
    }

public:
    /** \brief Adds a testcase to the testsuite */
    template<class T> void add()
    {
        insert(T::staticMetaObject.className(), &objectFactory<T>);
    }

    /** \brief Runs the testsuite */
    int exec(int &argc, char *argv[])
    {
        // Put cmdline args in a stringlist for easier handling
        QStringList args;
        for (int i=0; i<argc; i++)
        {
            args.append(QByteArray(argv[i]));
        }

        // Inspect all cmdline args
        for (int i=0; i<args.length(); i++)
        {
            // Caller wants lists of all test cases
            if (!args[i].compare("-testcases"))
            {
                foreach (const QByteArray &className, uniqueKeys())
                {
                    fprintf(stdout, "- %s\n", className.data());
                }
                return EXIT_SUCCESS;
            }

            // Caller wants to test single testcase
            if (!args[i].compare("-testcase"))
            {
                if (i+1 >= args.length())
                {
                    fprintf(stdout, "Missing -testcase argument\n");
                    return EXIT_FAILURE;
                }
                QObject *testCase = createObject(args[i+1].toLatin1());

                // Remove testcase args and treat as regular test
                args.removeAt(i);
                args.removeAt(i);
                return QTest::qExec(testCase, args);
            }
        }

        // No special testcase, just run all
        bool fail = false;
        foreach (const QByteArray &className, uniqueKeys())
        {
            QObject *testObject = createObject(className);
            fail |= QTest::qExec(testObject, args);
        }
        return fail;
    }
};

#endif // JHTESTSUITE_H
