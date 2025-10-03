#ifndef IDELEGATE_H
#define IDELEGATE_H

#include <QString>
#include <QVector>
#include <QObject>

/**
 * @interface IDelegate
 * @brief Standard interface for all delegates in the system.
 *
 * In the project architecture, the system follows a variation of the
 * Model-View-Controller (MVC) pattern. Delegates act as controllers: they
 * receive instances of the view and repositories, wire them together via
 * signals and slots, and perform any business logic needed to mediate
 * between the UI and the data layer.
 *
 * Delegates may also depend on other delegates. This interface provides a
 * standardized way to inject prerequisite delegates and to uniquely identify
 * each delegate by name.
 */
class IDelegate {

public:
    virtual ~IDelegate() = default;

    /**
     * @brief Returns the unique name of the delegate.
     *
     * Each delegate must provide a unique name so that other delegates
     * or builders can identify it.
     * @return Unique name of the delegate.
     */
    virtual QString name() const = 0;

    /**
     * @brief Injects prerequisite delegates needed by this delegate.
     *
     * Some delegates rely on the services or signals of other delegates.
     * The builder is responsible for creating delegates in the correct
     * order and injecting the required dependencies using this method.
     *
     * @param delegates List of delegate instances that this delegate depends on.
     */
    virtual void attachDelegates(const QVector<QObject*>& delegates) = 0;
};

/**
 * @brief Declares the interface to Qt's meta-object system.
 *
 * This allows the interface to be recognized by Qt's
 * Q_DECLARE_INTERFACE mechanism for QObject-based delegates.
 */
Q_DECLARE_INTERFACE(IDelegate, "com.etrek.IDelegate/1.0")

#endif // IDELEGATE_H
