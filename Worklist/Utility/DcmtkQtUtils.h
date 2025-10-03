#ifndef DCMTKQTUTILS_H
#define DCMTKQTUTILS_H

#include <QVector>
#include <QString>
#include "dcmtk/ofstd/oftypes.h" // for Uint16
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstring.h"

#define MAX_UINT16 65535
#define QString_To_OFString(qstr) OFString((qstr).toStdString().c_str())
#define OFString_TO_QString(ofstr) QString::fromUtf8((ofstr).c_str())

/**
 * @namespace Etrek::Utility
 * @brief Provides utility functions for converting between DCMTK and Qt types.
 *
 * This namespace contains helper functions for type conversion and adaptation
 * between DCMTK (DICOM Toolkit) types and Qt types, such as QString and OFString.
 */
namespace Etrek::Worklist::Utility {

    /**
     * @brief Converts an int to DCMTK's Uint16 type, with range checking.
     * @param value The integer value to convert.
     * @return The value as Uint16.
     * @throws std::out_of_range if the value is not in the range [0, 65535].
     */
    inline Uint16 IntToUint16(int value) {
        if (value < 0 || value > MAX_UINT16) {
            throw std::out_of_range("Value out of range for Uint16");
        }
        return static_cast<Uint16>(value);
    }

    /**
     * @brief Converts a QList of QString to a DCMTK OFList of OFString.
     * @param input The QList of QString to convert.
     * @return An OFList containing the converted OFString values.
     */
    inline OFList<OFString> QListQStringToOFListOFString(const QList<QString>& input)
    {
        OFList<OFString> result;
        for (const QString& str : input)
        {
            result.push_back(OFString(str.toStdString().c_str()));
        }
        return result;
    }
}

#endif // DCMTKQTUTILS_H
