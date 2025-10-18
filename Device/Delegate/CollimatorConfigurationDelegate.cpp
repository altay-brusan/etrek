#include "CollimatorConfigurationDelegate.h"

namespace Etrek::Device::Delegate
{

	CollimatorConfigurationDelegate::CollimatorConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	CollimatorConfigurationDelegate::~CollimatorConfigurationDelegate()
	{
	}
	void CollimatorConfigurationDelegate::apply()
	{
	}
	void CollimatorConfigurationDelegate::accept()
	{
	}
	void CollimatorConfigurationDelegate::reject()
	{
	}
	QString Etrek::Device::Delegate::CollimatorConfigurationDelegate::name() const
	{
		return QString();
	}
	void Etrek::Device::Delegate::CollimatorConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}
}

