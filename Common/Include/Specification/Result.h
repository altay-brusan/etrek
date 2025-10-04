#ifndef RESULT_H
#define RESULT_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <type_traits>

namespace Etrek::Specification
{
    // ====================== Generic Result<T> ======================
    template <typename T>
    class Result
    {
    public:
        bool isSuccess = false;
        QString message;
        T value;

        Result() = default;

        Result(bool success, const QString& msg, const T& val)
            : isSuccess(success), message(msg), value(val) {
        }

        static Result<T> Success(const T& val, const QString& msg = "Success")
        {
            return Result<T>(true, msg, val);
        }

        static Result<T> Failure(const QString& msg)
        {
            return Result<T>(false, msg, T{});
        }

        QJsonObject toJson() const
        {
            QJsonObject obj;
            obj["isSuccess"] = isSuccess;
            obj["message"] = message;

            if constexpr (std::is_same_v<T, QString>) {
                obj["value"] = value;
            }
            else if constexpr (std::is_arithmetic_v<T>) {
                obj["value"] = QJsonValue::fromVariant(QVariant::fromValue(value));
            }
            else {
                // For complex T you can extend this as needed
                obj["value"] = QJsonValue("Unsupported");
            }

            return obj;
        }
    };

    // ====================== Specialization: Result<void> ======================
    template <>
    class Result<void>
    {
    public:
        bool isSuccess = false;
        QString message;

        Result() = default;

        Result(bool success, const QString& msg)
            : isSuccess(success), message(msg) {
        }

        static Result<void> Success(const QString& msg = "Success")
        {
            return Result<void>(true, msg);
        }

        static Result<void> Failure(const QString& msg)
        {
            return Result<void>(false, msg);
        }

        QJsonObject toJson() const
        {
            QJsonObject obj;
            obj["isSuccess"] = isSuccess;
            obj["message"] = message;
            obj["value"] = QJsonValue(); // null
            return obj;
        }
    };
}

#endif // RESULT_H
