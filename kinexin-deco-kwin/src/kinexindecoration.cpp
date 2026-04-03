#include "kinexindecoration.h"
#include "kinexinbutton.h"

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/DecorationSettings>

#include <KConfigGroup>
#include <KSharedConfig>

#include <QPainter>
#include <QPainterPath>
#include <QFontMetricsF>
#include <QGuiApplication>

#include <cmath>

namespace Kinexin
{

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
{
}

Decoration::~Decoration() = default;

void Decoration::loadConfig()
{
    auto config = KSharedConfig::openConfig(QStringLiteral("kinexindecorc"));
    config->reparseConfiguration();
    const KConfigGroup group = config->group(QStringLiteral("General"));

    m_barWidth = group.readEntry("BarWidth", 36);
    m_buttonSize = group.readEntry("ButtonSize", 16);
    m_buttonPadding = group.readEntry("ButtonPadding", 10);
    m_barRounding = group.readEntry("BarRounding", 12);
    m_barColor = group.readEntry("BarColor", QColor(0x31, 0x31, 0x3A));
    m_closeColor = group.readEntry("CloseColor", QColor(0xE6, 0x4D, 0x4D));
    m_maximizeColor = group.readEntry("MaximizeColor", QColor(0x59, 0x73, 0xBF));
    m_keepAboveColor = group.readEntry("KeepAboveColor", QColor(0xA6, 0x4D, 0xA6));
    m_minimizeColor = group.readEntry("MinimizeColor", QColor(0x4D, 0xA6, 0x4D));
    m_titleColor = group.readEntry("TitleColor", QColor(0xA6, 0xA6, 0xAE, 0xB3));
    m_titleFontSize = group.readEntry("TitleFontSize", 11);
    m_titleFont = group.readEntry("TitleFont", QString());
    m_titleRotation = group.readEntry("TitleRotation", 0) == 1 ? 270 : 90;
    if (m_titleFont.isEmpty()) {
        m_titleFontObj = QGuiApplication::font();
    } else {
        m_titleFontObj = QFont(m_titleFont);
    }
    m_titleFontObj.setPixelSize(m_titleFontSize);
    m_hoverBrightness = group.readEntry("ButtonHoverBrightness", 1.2);
    m_pressBrightness = group.readEntry("ButtonPressBrightness", 0.7);
    m_barOpacity = qBound(0, group.readEntry("BarOpacity", 80), 100);
    {
        const int side = group.readEntry("BarSide", 0);
        switch (side) {
        case 1:  m_barSide = BarSide::Right;  break;
        case 2:  m_barSide = BarSide::Top;    break;
        case 3:  m_barSide = BarSide::Bottom; break;
        default: m_barSide = BarSide::Left;   break;
        }
    }

    // Apply opacity to bar color
    m_barColor.setAlpha(qRound(m_barOpacity * 255.0 / 100.0));
}

void Decoration::reconfigure()
{
    loadConfig();

    setState([this](KDecoration3::DecorationState *state) {
        switch (m_barSide) {
        case BarSide::Left:
            state->setBorders(QMarginsF(m_barWidth, 0, 0, 0));
            state->setBorderRadius(KDecoration3::BorderRadius(m_barRounding, 0, 0, m_barRounding));
            break;
        case BarSide::Right:
            state->setBorders(QMarginsF(0, 0, m_barWidth, 0));
            state->setBorderRadius(KDecoration3::BorderRadius(0, m_barRounding, m_barRounding, 0));
            break;
        case BarSide::Top:
            state->setBorders(QMarginsF(0, m_barWidth, 0, 0));
            state->setBorderRadius(KDecoration3::BorderRadius(m_barRounding, m_barRounding, 0, 0));
            break;
        case BarSide::Bottom:
            state->setBorders(QMarginsF(0, 0, 0, m_barWidth));
            state->setBorderRadius(KDecoration3::BorderRadius(0, 0, m_barRounding, m_barRounding));
            break;
        }
    });

    createButtons();
    updateLayout();
    update();
}

void Decoration::createButtons()
{
    qDeleteAll(m_topButtons);
    m_topButtons.clear();
    qDeleteAll(m_bottomButtons);
    m_bottomButtons.clear();

    auto *w = window();
    auto *s = settings().get();
    if (!w || !s) {
        return;
    }

    auto connectButton = [this](Button *btn, KDecoration3::DecorationButtonType btnType, auto *w) {
        if (btnType == KDecoration3::DecorationButtonType::KeepAbove && w->isKeepAbove()) {
            btn->setChecked(true);
        }
        // Repaint decoration when button hover/press state changes
        connect(btn, &KDecoration3::DecorationButton::hoveredChanged,
                this, [this]() { update(); });
        connect(btn, &KDecoration3::DecorationButton::pressedChanged,
                this, [this]() { update(); });
    };

    // Left group in KDE settings → top of our vertical bar
    for (auto btnType : s->decorationButtonsLeft()) {
        auto *btn = Button::create(btnType, this, this);
        if (btn) {
            m_topButtons.append(btn);
            connectButton(btn, btnType, w);
        }
    }

    // Right group in KDE settings → bottom of our vertical bar
    for (auto btnType : s->decorationButtonsRight()) {
        auto *btn = Button::create(btnType, this, this);
        if (btn) {
            m_bottomButtons.append(btn);
            connectButton(btn, btnType, w);
        }
    }
}

bool Decoration::init()
{
    loadConfig();

    auto *w = window();
    if (!w) {
        return false;
    }

    // Set borders: decoration is on the configured side
    setState([this](KDecoration3::DecorationState *state) {
        switch (m_barSide) {
        case BarSide::Left:
            state->setBorders(QMarginsF(m_barWidth, 0, 0, 0));
            state->setBorderRadius(KDecoration3::BorderRadius(m_barRounding, 0, 0, m_barRounding));
            break;
        case BarSide::Right:
            state->setBorders(QMarginsF(0, 0, m_barWidth, 0));
            state->setBorderRadius(KDecoration3::BorderRadius(0, m_barRounding, m_barRounding, 0));
            break;
        case BarSide::Top:
            state->setBorders(QMarginsF(0, m_barWidth, 0, 0));
            state->setBorderRadius(KDecoration3::BorderRadius(m_barRounding, m_barRounding, 0, 0));
            break;
        case BarSide::Bottom:
            state->setBorders(QMarginsF(0, 0, 0, m_barWidth));
            state->setBorderRadius(KDecoration3::BorderRadius(0, 0, m_barRounding, m_barRounding));
            break;
        }
    });

    // Enable resize on all non-bar sides (top, right, bottom)
    setResizeOnlyBorders(QMarginsF(6, 6, 6, 6));

    // No top bar — decoration is non-opaque (rounded corners, no top area)
    setOpaque(false);

    // Set titlebar to cover the entire bar (enables drag-to-move)
    switch (m_barSide) {
    case BarSide::Left:
        setTitleBar(QRectF(0, 0, m_barWidth, w->height()));
        break;
    case BarSide::Right:
        setTitleBar(QRectF(w->width(), 0, m_barWidth, w->height()));
        break;
    case BarSide::Top:
        setTitleBar(QRectF(0, 0, w->width(), m_barWidth));
        break;
    case BarSide::Bottom:
        setTitleBar(QRectF(0, w->height(), w->width(), m_barWidth));
        break;
    }

    // Create buttons from configured order
    createButtons();
    updateLayout();

    // Connect signals for state changes
    connect(w, &KDecoration3::DecoratedWindow::activeChanged,
            this, [this]() { update(); });
    connect(w, &KDecoration3::DecoratedWindow::captionChanged,
            this, [this]() { update(); });
    connect(w, &KDecoration3::DecoratedWindow::sizeChanged,
            this, [this]() {
                updateLayout();
                update();
            });
    connect(w, &KDecoration3::DecoratedWindow::maximizedChanged,
            this, [this]() { update(); });
    connect(w, &KDecoration3::DecoratedWindow::keepAboveChanged,
            this, [this](bool keepAbove) {
                for (auto *btn : m_topButtons) {
                    if (btn->type() == KDecoration3::DecorationButtonType::KeepAbove) {
                        btn->setChecked(keepAbove);
                    }
                }
                for (auto *btn : m_bottomButtons) {
                    if (btn->type() == KDecoration3::DecorationButtonType::KeepAbove) {
                        btn->setChecked(keepAbove);
                    }
                }
                update();
            });

    // React to button order changes from KDE Titlebar Buttons settings
    auto *s = settings().get();
    connect(s, &KDecoration3::DecorationSettings::decorationButtonsLeftChanged,
            this, [this]() { createButtons(); updateLayout(); update(); });
    connect(s, &KDecoration3::DecorationSettings::decorationButtonsRightChanged,
            this, [this]() { createButtons(); updateLayout(); update(); });
    // Reload custom config when backend signals reconfigure
    connect(s, &KDecoration3::DecorationSettings::reconfigured,
            this, &Decoration::reconfigure);

    return true;
}

QString Decoration::extractTitle(const QString &caption)
{
    int dashPos = caption.lastIndexOf(QStringLiteral(" - "));
    if (dashPos >= 0 && dashPos + 3 < caption.size()) {
        return caption.mid(dashPos + 3);
    }
    int emDashPos = caption.lastIndexOf(QStringLiteral(" \u2014 "));
    if (emDashPos >= 0 && emDashPos + 3 < caption.size()) {
        return caption.mid(emDashPos + 3);
    }
    return caption;
}

void Decoration::updateLayout()
{
    auto *w = window();
    if (!w) {
        return;
    }

    const bool vertical = (m_barSide == BarSide::Left || m_barSide == BarSide::Right);

    if (vertical) {
        const qreal totalH = w->height();
        const qreal barCenterX = (m_barWidth - m_buttonSize) / 2.0;
        const qreal barOffsetX = (m_barSide == BarSide::Left) ? 0.0 : w->width();
        const qreal btnX = barOffsetX + barCenterX;

        // Position top buttons from top downward
        qreal currentY = m_buttonPadding;
        for (auto *btn : m_topButtons) {
            btn->setGeometry(QRectF(btnX, currentY, m_buttonSize, m_buttonSize));
            currentY += m_buttonSize + m_buttonPadding;
        }

        // Position bottom buttons from bottom upward
        qreal bottomY = totalH - m_buttonPadding;
        for (int i = m_bottomButtons.size() - 1; i >= 0; --i) {
            bottomY -= m_buttonSize;
            m_bottomButtons[i]->setGeometry(QRectF(btnX, bottomY, m_buttonSize, m_buttonSize));
            bottomY -= m_buttonPadding;
        }

        // Update titlebar to cover the entire bar for drag-to-move
        if (m_barSide == BarSide::Left) {
            setTitleBar(QRectF(0, 0, m_barWidth, totalH));
        } else {
            setTitleBar(QRectF(w->width(), 0, m_barWidth, totalH));
        }
    } else {
        const qreal totalW = w->width();
        const qreal barCenterY = (m_barWidth - m_buttonSize) / 2.0;
        const qreal barOffsetY = (m_barSide == BarSide::Top) ? 0.0 : w->height();
        const qreal btnY = barOffsetY + barCenterY;

        // Position left buttons from left to right
        qreal currentX = m_buttonPadding;
        for (auto *btn : m_topButtons) {
            btn->setGeometry(QRectF(currentX, btnY, m_buttonSize, m_buttonSize));
            currentX += m_buttonSize + m_buttonPadding;
        }

        // Position right buttons from right to left
        qreal rightX = totalW - m_buttonPadding;
        for (int i = m_bottomButtons.size() - 1; i >= 0; --i) {
            rightX -= m_buttonSize;
            m_bottomButtons[i]->setGeometry(QRectF(rightX, btnY, m_buttonSize, m_buttonSize));
            rightX -= m_buttonPadding;
        }

        // Update titlebar
        if (m_barSide == BarSide::Top) {
            setTitleBar(QRectF(0, 0, totalW, m_barWidth));
        } else {
            setTitleBar(QRectF(0, w->height(), totalW, m_barWidth));
        }
    }
}

void Decoration::paint(QPainter *painter, const QRectF & /*repaintArea*/)
{
    auto *w = window();
    if (!w) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const qreal totalH = w->height();
    const qreal totalW = w->width();
    const qreal rounding = m_barRounding;
    const bool vertical = (m_barSide == BarSide::Left || m_barSide == BarSide::Right);

    // Draw background bar
    QPainterPath barPath;
    switch (m_barSide) {
    case BarSide::Left:
        barPath.moveTo(m_barWidth, 0);
        barPath.lineTo(rounding, 0);
        barPath.arcTo(0, 0, rounding * 2, rounding * 2, 90, 90);
        barPath.lineTo(0, totalH - rounding);
        barPath.arcTo(0, totalH - rounding * 2, rounding * 2, rounding * 2, 180, 90);
        barPath.lineTo(m_barWidth, totalH);
        break;
    case BarSide::Right: {
        const qreal bx = totalW;
        barPath.moveTo(bx, 0);
        barPath.lineTo(bx + m_barWidth - rounding, 0);
        barPath.arcTo(bx + m_barWidth - rounding * 2, 0, rounding * 2, rounding * 2, 90, -90);
        barPath.lineTo(bx + m_barWidth, totalH - rounding);
        barPath.arcTo(bx + m_barWidth - rounding * 2, totalH - rounding * 2, rounding * 2, rounding * 2, 0, -90);
        barPath.lineTo(bx, totalH);
        break;
    }
    case BarSide::Top:
        barPath.moveTo(0, m_barWidth);
        barPath.lineTo(0, rounding);
        barPath.arcTo(0, 0, rounding * 2, rounding * 2, 180, -90);
        barPath.lineTo(totalW - rounding, 0);
        barPath.arcTo(totalW - rounding * 2, 0, rounding * 2, rounding * 2, 90, -90);
        barPath.lineTo(totalW, m_barWidth);
        break;
    case BarSide::Bottom: {
        const qreal by = totalH;
        barPath.moveTo(0, by);
        barPath.lineTo(0, by + m_barWidth - rounding);
        barPath.arcTo(0, by + m_barWidth - rounding * 2, rounding * 2, rounding * 2, 180, 90);
        barPath.lineTo(totalW - rounding, by + m_barWidth);
        barPath.arcTo(totalW - rounding * 2, by + m_barWidth - rounding * 2, rounding * 2, rounding * 2, 270, 90);
        barPath.lineTo(totalW, by);
        break;
    }
    }
    barPath.closeSubpath();

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_barColor);
    painter->drawPath(barPath);

    // Paint buttons
    for (auto *btn : m_topButtons) {
        btn->paint(painter, btn->geometry());
    }
    for (auto *btn : m_bottomButtons) {
        btn->paint(painter, btn->geometry());
    }

    // Draw window title
    if (vertical) {
        // Vertical bar: rotated title text
        qreal titleStartY = m_buttonPadding;
        if (!m_topButtons.isEmpty()) {
            titleStartY = m_topButtons.last()->geometry().bottom() + m_buttonPadding;
        }

        qreal titleEndY = totalH - m_buttonPadding;
        if (!m_bottomButtons.isEmpty()) {
            titleEndY = m_bottomButtons.first()->geometry().top() - m_buttonPadding;
        }
        const qreal availH = titleEndY - titleStartY;
        const qreal barCenterX = (m_barSide == BarSide::Left ? 0.0 : totalW) + m_barWidth / 2.0;

        if (availH > 20.0) {
            const QString title = extractTitle(w->caption());
            if (!title.isEmpty()) {
                painter->setFont(m_titleFontObj);
                painter->setPen(m_titleColor);
                QFontMetricsF fm(m_titleFontObj);

                QString displayTitle = title;
                while (displayTitle.size() > 3 && fm.horizontalAdvance(displayTitle) > availH - 8.0) {
                    displayTitle = displayTitle.left(displayTitle.size() - 4) + QStringLiteral("...");
                }

                const qreal textWidth = fm.horizontalAdvance(displayTitle);
                const qreal textHeight = fm.ascent() + fm.descent();

                painter->save();
                painter->translate(barCenterX, totalH / 2.0);
                painter->rotate(m_titleRotation);
                painter->drawText(QRectF(-textWidth / 2.0, -textHeight / 2.0, textWidth, textHeight),
                                  Qt::AlignCenter, displayTitle);
                painter->restore();
            }
        }
    } else {
        // Horizontal bar: normal horizontal title text
        qreal titleStartX = m_buttonPadding;
        if (!m_topButtons.isEmpty()) {
            titleStartX = m_topButtons.last()->geometry().right() + m_buttonPadding;
        }

        qreal titleEndX = totalW - m_buttonPadding;
        if (!m_bottomButtons.isEmpty()) {
            titleEndX = m_bottomButtons.first()->geometry().left() - m_buttonPadding;
        }
        const qreal availW = titleEndX - titleStartX;
        const qreal barCenterY = (m_barSide == BarSide::Top ? 0.0 : totalH) + m_barWidth / 2.0;

        if (availW > 20.0) {
            const QString title = extractTitle(w->caption());
            if (!title.isEmpty()) {
                painter->setFont(m_titleFontObj);
                painter->setPen(m_titleColor);
                QFontMetricsF fm(m_titleFontObj);

                QString displayTitle = title;
                while (displayTitle.size() > 3 && fm.horizontalAdvance(displayTitle) > availW - 8.0) {
                    displayTitle = displayTitle.left(displayTitle.size() - 4) + QStringLiteral("...");
                }

                const qreal textWidth = fm.horizontalAdvance(displayTitle);
                const qreal textHeight = fm.ascent() + fm.descent();

                painter->drawText(QRectF(titleStartX + (availW - textWidth) / 2.0,
                                         barCenterY - textHeight / 2.0,
                                         textWidth, textHeight),
                                  Qt::AlignCenter, displayTitle);
            }
        }
    }

    painter->restore();
}

} // namespace Kinexin
