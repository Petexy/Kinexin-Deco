#pragma once

#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationButton>

#include <QVariantList>
#include <QStringList>
#include <QFont>

namespace Kinexin
{

enum class BarSide { Left = 0, Right = 1, Top = 2, Bottom = 3 };

class Button;

class Decoration : public KDecoration3::Decoration
{
    Q_OBJECT

public:
    explicit Decoration(QObject *parent, const QVariantList &args);
    ~Decoration() override;

    void paint(QPainter *painter, const QRectF &repaintArea) override;
    bool init() override;

    // Configuration
    int barWidth() const { return m_barWidth; }
    int buttonSize() const { return m_buttonSize; }
    int buttonPadding() const { return m_buttonPadding; }
    int barRounding() const { return m_barRounding; }
    QColor barColor() const { return m_barColor; }
    BarSide barSide() const { return m_barSide; }
    QColor closeColor() const { return m_closeColor; }
    QColor maximizeColor() const { return m_maximizeColor; }
    QColor keepAboveColor() const { return m_keepAboveColor; }
    QColor minimizeColor() const { return m_minimizeColor; }
    QColor titleColor() const { return m_titleColor; }
    int titleFontSize() const { return m_titleFontSize; }
    QString titleFont() const { return m_titleFont; }
    qreal hoverBrightness() const { return m_hoverBrightness; }
    qreal pressBrightness() const { return m_pressBrightness; }

private:
    void loadConfig();
    void updateLayout();
    void createButtons();
    void reconfigure();
    static QString extractTitle(const QString &caption);

    // Buttons (managed manually for vertical layout)
    QList<Button *> m_topButtons;    // from KDE left group
    QList<Button *> m_bottomButtons; // from KDE right group

    // Config values
    int m_barWidth = 36;
    int m_buttonSize = 16;
    int m_buttonPadding = 10;
    int m_barRounding = 12;
    QColor m_barColor{0x31, 0x31, 0x3A};
    QColor m_closeColor{0xE6, 0x4D, 0x4D};
    QColor m_maximizeColor{0x59, 0x73, 0xBF};
    QColor m_keepAboveColor{0xA6, 0x4D, 0xA6};
    QColor m_minimizeColor{0x4D, 0xA6, 0x4D};
    QColor m_titleColor{0xA6, 0xA6, 0xAE, 0xB3};
    int m_titleFontSize = 11;
    QFont m_titleFontObj;
    QString m_titleFont;
    int m_titleRotation = 90;
    qreal m_hoverBrightness = 1.2;
    qreal m_pressBrightness = 0.7;
    int m_barOpacity = 100;
    BarSide m_barSide = BarSide::Left;
};

} // namespace Kinexin
