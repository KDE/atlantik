#include <kdebug.h>

#include <klocale.h>
 
#include "display_widget.moc"

BoardDisplay::BoardDisplay(const QString description, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	// Player list.
	m_playerGroupBox = new QVGroupBox(i18n("Chance card"), this, "groupBox");
	m_mainLayout->addWidget(m_playerGroupBox); 

	m_label = new QTextEdit(description, NULL, m_playerGroupBox);
	m_label->setReadOnly(true);
}

void BoardDisplay::slotClicked()
{
	m_playerGroupBox->setEnabled(true);
}
