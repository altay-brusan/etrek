#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Context { class IContextManager; }

namespace Etrek::Device::Delegate
{
	class ConnectionSetupDelegate :
	public QObject,
	public IDelegate, 
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a ConnectionSetupDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] contextManager Weak pointer to the context manager for audit tracking.
		 * @param[in] parent Parent QObject for memory management.
		 */
		ConnectionSetupDelegate(
			QWidget* widget,
			std::weak_ptr<Etrek::Context::IContextManager> contextManager,
			QObject* parent);

		// Inherited via IDelegate
		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~ConnectionSetupDelegate();
	private:
		QWidget* m_widget = nullptr;
		std::weak_ptr<Etrek::Context::IContextManager> m_contextManager; ///< Context manager for audit tracking.

		// Inherited via IPageAction
		void apply() override;
		void accept() override;
		void reject() override;
	};
}
