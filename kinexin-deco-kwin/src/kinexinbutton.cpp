#include "kinexinbutton.h"
#include "kinexindecoration.h"

#include <KDecoration3/DecoratedWindow>

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace Kinexin
{

Button::Button(KDecoration3::DecorationButtonType type,
               Decoration *decoration,
               QObject *parent)
    : KDecoration3::DecorationButton(type, decoration, parent)
{
    const int size = decoration->buttonSize();
    setGeometry(QRectF(0, 0, size, size));

    // KeepAbove, KeepBelow, OnAllDesktops, Shade are checkable
    switch (type) {
    case KDecoration3::DecorationButtonType::KeepAbove:
    case KDecoration3::DecorationButtonType::KeepBelow:
    case KDecoration3::DecorationButtonType::OnAllDesktops:
    case KDecoration3::DecorationButtonType::Shade:
        setCheckable(true);
        break;
    default:
        break;
    }
}

Button *Button::create(KDecoration3::DecorationButtonType type,
                       KDecoration3::Decoration *decoration,
                       QObject *parent)
{
    auto *deco = qobject_cast<Decoration *>(decoration);
    if (!deco) {
        return nullptr;
    }

    switch (type) {
    case KDecoration3::DecorationButtonType::Close:
    case KDecoration3::DecorationButtonType::Maximize:
    case KDecoration3::DecorationButtonType::Minimize:
    case KDecoration3::DecorationButtonType::KeepAbove:
    case KDecoration3::DecorationButtonType::KeepBelow:
    case KDecoration3::DecorationButtonType::OnAllDesktops:
    case KDecoration3::DecorationButtonType::Shade:
    case KDecoration3::DecorationButtonType::Menu:
    case KDecoration3::DecorationButtonType::ApplicationMenu:
    case KDecoration3::DecorationButtonType::ContextHelp:
    case KDecoration3::DecorationButtonType::ExcludeFromCapture:
    case KDecoration3::DecorationButtonType::Spacer:
        return new Button(type, deco, parent);
    default:
        return nullptr;
    }
}

QColor Button::buttonColor() const
{
    auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return Qt::gray;
    }

    QColor base;
    switch (type()) {
    case KDecoration3::DecorationButtonType::Close:
        base = deco->closeColor();
        break;
    case KDecoration3::DecorationButtonType::Maximize:
        base = deco->maximizeColor();
        break;
    case KDecoration3::DecorationButtonType::Minimize:
        base = deco->minimizeColor();
        break;
    case KDecoration3::DecorationButtonType::KeepAbove:
        base = deco->keepAboveColor();
        break;
    default:
        base = Qt::gray;
        break;
    }

    // Apply brightness for hover/press states
    if (isPressed()) {
        const qreal factor = deco->pressBrightness();
        base = QColor::fromRgbF(
            qMin(1.0, base.redF() * factor),
            qMin(1.0, base.greenF() * factor),
            qMin(1.0, base.blueF() * factor),
            base.alphaF());
    } else if (isHovered()) {
        const qreal factor = deco->hoverBrightness();
        base = QColor::fromRgbF(
            qMin(1.0, base.redF() * factor),
            qMin(1.0, base.greenF() * factor),
            qMin(1.0, base.blueF() * factor),
            base.alphaF());
    }

    return base;
}

void Button::paint(QPainter *painter, const QRectF & /*repaintArea*/)
{
    if (!isVisible()) {
        return;
    }

    // Spacer has no visual
    if (type() == KDecoration3::DecorationButtonType::Spacer) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QRectF g = geometry();

    // Menu button draws the window icon
    if (type() == KDecoration3::DecorationButtonType::Menu) {
        auto *deco = qobject_cast<Decoration *>(decoration());
        if (deco) {
            auto *w = deco->window();
            if (w) {
                const QIcon icon = w->icon();
                const int s = qRound(qMin(g.width(), g.height()));
                const QPixmap pix = icon.pixmap(s, s);
                painter->drawPixmap(g.topLeft(), pix);
            }
        }
        painter->restore();
        return;
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(buttonColor());

    const qreal radius = qMin(g.width(), g.height()) / 2.0;
    painter->drawEllipse(g.center(), radius, radius);

    painter->restore();
}

} // namespace Kinexin
