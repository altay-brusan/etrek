#ifndef DATETIMESPAN_H
#define DATETIMESPAN_H

#include <QDateTime>

struct DateTimeSpan
{
    QDateTime from;
    QDateTime to;

    // Optional: Equality operators
    bool operator==(const DateTimeSpan& other) const
    {
        return from == other.from && to == other.to;
    }
    bool operator!=(const DateTimeSpan& other) const
    {
        return !(*this == other);
    }
};

#endif // DATETIMESPAN_H
