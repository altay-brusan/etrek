#ifndef ETREK_DICOM_DATA_UTIL_DICOMNAMEFORMATTER_H
#define ETREK_DICOM_DATA_UTIL_DICOMNAMEFORMATTER_H

#include <QString>

namespace Etrek::Dicom::Data::Util {

    /**
     * @brief Utility class for formatting names in DICOM Person Name (PN) format
     *
     * DICOM PN format: FamilyName^GivenName^MiddleName^NamePrefix^NameSuffix
     * This utility handles the common case: LastName^FirstName^MiddleName
     * and properly handles empty components (no trailing/doubled carets).
     */
    class DicomNameFormatter
    {
    public:
        /**
         * @brief Format name in DICOM PN format: LastName^FirstName^MiddleName
         *
         * @param firstName First/given name
         * @param middleName Middle name (optional, can be empty)
         * @param lastName Last/family name
         * @return QString in DICOM PN format with proper caret delimiters
         *
         * Examples:
         * - ("John", "", "Doe") -> "Doe^John"
         * - ("John", "Q", "Doe") -> "Doe^John^Q"
         * - ("John", "Q Public", "Doe") -> "Doe^John^Q Public"
         * - ("", "", "Doe") -> "Doe"
         * - ("John", "", "") -> "^John"
         */
        static QString formatPatientName(const QString& firstName,
                                         const QString& middleName,
                                         const QString& lastName)
        {
            QString trimmedFirst = firstName.trimmed();
            QString trimmedMiddle = middleName.trimmed();
            QString trimmedLast = lastName.trimmed();

            // Build name with carets, avoiding trailing or doubled carets
            QString result;

            // Always start with last name (even if empty, it's the first component)
            result = trimmedLast;

            // Add first name if present
            if (!trimmedFirst.isEmpty()) {
                result += "^" + trimmedFirst;

                // Only add middle name if first name is present
                if (!trimmedMiddle.isEmpty()) {
                    result += "^" + trimmedMiddle;
                }
            }
            else if (!trimmedMiddle.isEmpty()) {
                // Edge case: middle name without first name
                // Format as: LastName^^MiddleName
                result += "^^" + trimmedMiddle;
            }

            return result;
        }

        /**
         * @brief Format date in DICOM DA (Date) format: YYYYMMDD
         *
         * @param date QDate to format
         * @return QString in YYYYMMDD format, or empty if date is invalid
         */
        static QString formatDicomDate(const QDate& date)
        {
            if (!date.isValid()) {
                return QString();
            }
            return date.toString("yyyyMMdd");
        }

        /**
         * @brief Normalize DICOM sex code
         *
         * @param sex Input sex string (various formats accepted)
         * @return Normalized DICOM sex code: M, F, O, or empty for unknown
         *
         * Accepted inputs (case-insensitive):
         * - M, Male, Man -> M
         * - F, Female, Woman -> F
         * - O, Other -> O
         * - Anything else -> empty string
         */
        static QString normalizeSexCode(const QString& sex)
        {
            QString upper = sex.trimmed().toUpper();

            if (upper == "M" || upper == "MALE" || upper == "MAN") {
                return "M";
            }
            else if (upper == "F" || upper == "FEMALE" || upper == "WOMAN") {
                return "F";
            }
            else if (upper == "O" || upper == "OTHER") {
                return "O";
            }

            // Unknown/unspecified - return empty for database NULL
            return QString();
        }
    };

} // namespace Etrek::Dicom::Data::Util

#endif // ETREK_DICOM_DATA_UTIL_DICOMNAMEFORMATTER_H
