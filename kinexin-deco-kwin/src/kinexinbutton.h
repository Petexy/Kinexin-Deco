#pragma once

#include <KDecoration3/DecorationButton>

class QPainter;
class QPixmap;

namespace Kinexin
{

class Decoration;

class Button : public KDecoration3::DecorationButton
{
    Q_OBJECT

public:
    explicit Button(KDecoration3::DecorationButtonType type,
                    Decoration *decoration,
                    QObject *parent = nullptr);

    void paint(QPainter *painter, const QRectF &repaintArea) override;

    static Button *create(KDecoration3::DecorationButtonType type,
                          KDecoration3::Decoration *decoration,
                          QObject *parent);

private:
    QColor buttonColor() const;
    void paintIcon(QPainter *painter, const QRectF &g) const;
};

} // namespace Kinexin
