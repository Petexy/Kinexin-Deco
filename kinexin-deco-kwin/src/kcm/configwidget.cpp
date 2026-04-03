#include "configwidget.h"

#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KColorButton>

#include <QComboBox>
#include <QCheckBox>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

KinexinConfigWidget::KinexinConfigWidget(QObject *parent, const KPluginMetaData &data)
    : KinexinConfigWidget(qobject_cast<QWidget *>(parent), data)
{
}

KinexinConfigWidget::KinexinConfigWidget(QWidget *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    auto *layout = new QFormLayout(widget());

    m_barSide = new QComboBox(widget());
    m_barSide->addItem(tr("Left"));
    m_barSide->addItem(tr("Right"));
    m_barSide->addItem(tr("Top"));
    m_barSide->addItem(tr("Bottom"));
    layout->addRow(tr("Bar side:"), m_barSide);

    m_buttonSize = new QSpinBox(widget());
    m_buttonSize->setRange(8, 48);
    m_buttonSize->setSuffix(QStringLiteral(" px"));
    layout->addRow(tr("Button size:"), m_buttonSize);

    m_buttonPadding = new QSpinBox(widget());
    m_buttonPadding->setRange(2, 30);
    m_buttonPadding->setSuffix(QStringLiteral(" px"));
    layout->addRow(tr("Button spacing:"), m_buttonPadding);

    m_barWidth = new QSpinBox(widget());
    m_barWidth->setRange(20, 80);
    m_barWidth->setSuffix(QStringLiteral(" px"));
    layout->addRow(tr("Bar width:"), m_barWidth);

    m_barRounding = new QSpinBox(widget());
    m_barRounding->setRange(0, 30);
    m_barRounding->setSuffix(QStringLiteral(" px"));
    layout->addRow(tr("Bar rounding:"), m_barRounding);

    m_barColor = new KColorButton(widget());
    m_barColor->setAlphaChannelEnabled(false);
    auto *colorRow = new QHBoxLayout;
    colorRow->addWidget(m_barColor);
    m_defaultColorBtn = new QPushButton(tr("Default"), widget());
    colorRow->addWidget(m_defaultColorBtn);
    layout->addRow(tr("Bar color:"), colorRow);

    m_barOpacity = new QSpinBox(widget());
    m_barOpacity->setRange(0, 100);
    m_barOpacity->setSuffix(QStringLiteral(" %"));
    layout->addRow(tr("Bar opacity:"), m_barOpacity);

    m_useSystemFont = new QCheckBox(tr("Use system font"), widget());
    layout->addRow(tr("Title font:"), m_useSystemFont);

    m_titleFont = new QFontComboBox(widget());
    layout->addRow(QStringLiteral(""), m_titleFont);

    m_titleFontSize = new QSpinBox(widget());
    m_titleFontSize->setRange(6, 36);
    m_titleFontSize->setSuffix(QStringLiteral(" px"));
    layout->addRow(tr("Title font size:"), m_titleFontSize);

    m_titleColor = new KColorButton(widget());
    m_titleColor->setAlphaChannelEnabled(true);
    layout->addRow(tr("Title color:"), m_titleColor);

    m_titleFlipped = new QCheckBox(tr("Read bottom to top"), widget());
    layout->addRow(tr("Title rotation:"), m_titleFlipped);

    connect(m_useSystemFont, &QCheckBox::toggled, m_titleFont, [this](bool checked) {
        m_titleFont->setEnabled(!checked);
    });

    // Disable rotation for horizontal bar modes
    connect(m_barSide, &QComboBox::currentIndexChanged, this, [this](int index) {
        const bool horizontal = (index == 2 || index == 3);
        m_titleFlipped->setEnabled(!horizontal);
        if (horizontal) {
            m_titleFlipped->setChecked(false);
        }
    });

    auto markChanged = [this]() { markAsChanged(); };
    connect(m_barSide, &QComboBox::currentIndexChanged, this, markChanged);
    connect(m_buttonSize, &QSpinBox::valueChanged, this, markChanged);
    connect(m_buttonPadding, &QSpinBox::valueChanged, this, markChanged);
    connect(m_barWidth, &QSpinBox::valueChanged, this, markChanged);
    connect(m_barRounding, &QSpinBox::valueChanged, this, markChanged);
    connect(m_barColor, &KColorButton::changed, this, markChanged);
    connect(m_barOpacity, &QSpinBox::valueChanged, this, markChanged);
    connect(m_titleFont, &QFontComboBox::currentFontChanged, this, markChanged);
    connect(m_titleFontSize, &QSpinBox::valueChanged, this, markChanged);
    connect(m_useSystemFont, &QCheckBox::toggled, this, markChanged);
    connect(m_titleColor, &KColorButton::changed, this, markChanged);
    connect(m_titleFlipped, &QCheckBox::toggled, this, markChanged);
    connect(m_defaultColorBtn, &QPushButton::clicked, this, [this]() {
        m_barColor->setColor(QColor(0x31, 0x31, 0x3A));
        m_barOpacity->setValue(80);
        markAsChanged();
    });

    load();
}

void KinexinConfigWidget::load()
{
    auto config = KSharedConfig::openConfig(QStringLiteral("kinexindecorc"));
    const KConfigGroup group = config->group(QStringLiteral("General"));

    m_barSide->setCurrentIndex(group.readEntry("BarSide", 0));
    m_buttonSize->setValue(group.readEntry("ButtonSize", 16));
    m_buttonPadding->setValue(group.readEntry("ButtonPadding", 10));
    m_barWidth->setValue(group.readEntry("BarWidth", 36));
    m_barRounding->setValue(group.readEntry("BarRounding", 12));
    m_barColor->setColor(group.readEntry("BarColor", QColor(0x31, 0x31, 0x3A)));
    m_barOpacity->setValue(group.readEntry("BarOpacity", 80));

    const QString savedFont = group.readEntry("TitleFont", QString());
    if (savedFont.isEmpty()) {
        m_useSystemFont->setChecked(true);
        m_titleFont->setCurrentFont(QGuiApplication::font());
        m_titleFont->setEnabled(false);
    } else {
        m_useSystemFont->setChecked(false);
        m_titleFont->setCurrentFont(QFont(savedFont));
        m_titleFont->setEnabled(true);
    }
    m_titleFontSize->setValue(group.readEntry("TitleFontSize", 11));
    m_titleColor->setColor(group.readEntry("TitleColor", QColor(0xA6, 0xA6, 0xAE, 0xB3)));
    m_titleFlipped->setChecked(group.readEntry("TitleRotation", 0) == 1);
    m_titleFlipped->setEnabled(m_barSide->currentIndex() < 2);

    setNeedsSave(false);
}

void KinexinConfigWidget::save()
{
    auto config = KSharedConfig::openConfig(QStringLiteral("kinexindecorc"));
    KConfigGroup group = config->group(QStringLiteral("General"));

    group.writeEntry("BarSide", m_barSide->currentIndex());
    group.writeEntry("ButtonSize", m_buttonSize->value());
    group.writeEntry("ButtonPadding", m_buttonPadding->value());
    group.writeEntry("BarWidth", m_barWidth->value());
    group.writeEntry("BarRounding", m_barRounding->value());
    group.writeEntry("BarColor", m_barColor->color());
    group.writeEntry("BarOpacity", m_barOpacity->value());

    if (m_useSystemFont->isChecked()) {
        group.deleteEntry("TitleFont");
    } else {
        group.writeEntry("TitleFont", m_titleFont->currentFont().family());
    }
    group.writeEntry("TitleFontSize", m_titleFontSize->value());
    group.writeEntry("TitleColor", m_titleColor->color());
    group.writeEntry("TitleRotation", m_titleFlipped->isChecked() ? 1 : 0);

    config->sync();
    setNeedsSave(false);

    // Tell KWin to reconfigure decorations so changes take effect immediately
    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/KWin"),
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("reconfigure"));
    QDBusConnection::sessionBus().send(message);
}

void KinexinConfigWidget::defaults()
{
    m_barSide->setCurrentIndex(0);
    m_buttonSize->setValue(16);
    m_buttonPadding->setValue(10);
    m_barWidth->setValue(36);
    m_barRounding->setValue(12);
    m_barColor->setColor(QColor(0x31, 0x31, 0x3A));
    m_barOpacity->setValue(80);
    m_useSystemFont->setChecked(true);
    m_titleFont->setCurrentFont(QGuiApplication::font());
    m_titleFont->setEnabled(false);
    m_titleFontSize->setValue(11);
    m_titleColor->setColor(QColor(0xA6, 0xA6, 0xAE, 0xB3));
    m_titleFlipped->setChecked(false);
    markAsChanged();
}

K_PLUGIN_FACTORY_WITH_JSON(
    KinexinConfigWidgetFactory,
    "kcm_kinexindecoration.json",
    registerPlugin<KinexinConfigWidget>();
)

#include "configwidget.moc"
