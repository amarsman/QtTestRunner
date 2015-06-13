#ifndef JHTESTSUITE_H
#define JHTESTSUITE_H
#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QStringList>

typedef QObject* (*ObjectConstructor)();

class JhTestSuite : public QMap<QByteArray, ObjectConstructor>
{
private:
    /** \brief Templated factory that creates an instance of T */
    template<class T> static QObject* objectFactory()
    {
        return new T();
    }

    /** \brief Templated factory that creates an instance by classname */
    QObject* createObject(const QByteArray &a_className) const
    {
        ObjectConstructor factory = value(a_className);
        return (factory == NULL) ? NULL : (*factory)();
    }

public:
    template<class T> void add()
    {
        insert(T::staticMetaObject.className(), &objectFactory<T>);
    }

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
