#pragma once

#include <KCModule>

class QComboBox;
class QSpinBox;
class QPushButton;
class QFontComboBox;
class QCheckBox;
class KColorButton;

class KinexinConfigWidget : public KCModule
{
    Q_OBJECT

public:
    explicit KinexinConfigWidget(QObject *parent, const KPluginMetaData &data);
    explicit KinexinConfigWidget(QWidget *parent, const KPluginMetaData &data);

    void load() override;
    void save() override;
    void defaults() override;

private:
    QComboBox *m_barSide = nullptr;
    QSpinBox *m_buttonSize = nullptr;
    QSpinBox *m_buttonPadding = nullptr;
    QSpinBox *m_barWidth = nullptr;
    QSpinBox *m_barRounding = nullptr;
    KColorButton *m_barColor = nullptr;
    QSpinBox *m_barOpacity = nullptr;
    QPushButton *m_defaultColorBtn = nullptr;
    QFontComboBox *m_titleFont = nullptr;
    QSpinBox *m_titleFontSize = nullptr;
    QCheckBox *m_useSystemFont = nullptr;
    KColorButton *m_titleColor = nullptr;
    QCheckBox *m_titleFlipped = nullptr;
};
