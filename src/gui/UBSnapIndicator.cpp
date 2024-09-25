/*
 * Copyright (C) 2015-2024 Département de l'Instruction Publique (DIP-SEM)
 *
 * This file is part of OpenBoard.
 *
 * OpenBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * OpenBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBoard. If not, see <http://www.gnu.org/licenses/>.
 */


#include "UBSnapIndicator.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "board/UBBoardController.h"
#include "board/UBBoardPaletteManager.h"
#include "board/UBBoardView.h"
#include "core/UBApplication.h"

UBSnapIndicator::UBSnapIndicator(QWidget* parent)
    : QLabel(parent)
{
    mAnimation = new QPropertyAnimation(this, "alpha", this);
    mAnimation->setStartValue(0xff);
    mAnimation->setEndValue(0);
    mAnimation->setDuration(1000);

    connect(mAnimation, &QPropertyAnimation::finished, this, &QWidget::hide);
}

void UBSnapIndicator::appear(Qt::Corner corner, QPointF snapPoint)
{
    if (corner != mCorner)
    {
        mAnimation->stop();
        mCorner = corner;
        show();

        UBBoardView* view = dynamic_cast<UBBoardView*>(parentWidget());

        if (view)
        {
            QPoint indicationPoint{view->mapFromScene(snapPoint) - QPoint(width() / 2, height() / 2)};

            move(indicationPoint);
            mAnimation->start();
        }
    }
}

int UBSnapIndicator::alpha() const
{
    return mAlpha;
}

void UBSnapIndicator::setAlpha(int opacity)
{
    mAlpha = opacity;
    update();
}

void UBSnapIndicator::setColor(const QColor& color)
{
    mColor = color;
}

void UBSnapIndicator::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect area = rect();

    QPen pen;
    QColor penColor{mColor};
    penColor.setAlpha(mAlpha);
    pen.setColor(penColor);

    painter.setPen(pen);

    QPoint p1;
    QPoint p2(area.center());
    QPoint p3;
    int dist = area.width() / 3;

    switch (mCorner)
    {
    case Qt::TopLeftCorner:
        p1 = p2 + QPoint{0, dist};
        p3 = p2 + QPoint(dist, 0);
        break;

    case Qt::TopRightCorner:
        p1 = p2 + QPoint{0, dist};
        p3 = p2 + QPoint(-dist, 0);
        break;

    case Qt::BottomLeftCorner:
        p1 = p2 + QPoint{0, -dist};
        p3 = p2 + QPoint(dist, 0);
        break;

    case Qt::BottomRightCorner:
        p1 = p2 + QPoint{0, -dist};
        p3 = p2 + QPoint(-dist, 0);
        break;

    default:
        break;
    }

    QPolygon polygon;
    polygon << p1 << p2 << p3 << p1;

    QPainterPath path;
    path.addPolygon(polygon);

    painter.drawPolygon(polygon);
    painter.fillPath(path, penColor);
}
