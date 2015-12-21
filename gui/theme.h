/*******************************************************************
*
* This file is part of the KDE project "Bovo"
*
* Bovo is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* Bovo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Bovo; see the file COPYING.  If not, write to
* the Free Software Foundation, 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*
********************************************************************/

#ifndef BOVO_THEME_H
#define BOVO_THEME_H

// Qt includes
#include <QColor>
#include <QString>

namespace gui {

enum GridType {SvgGrid, GomokuGrid, SquaresGrid};

class Theme {
public:
    Theme();
    Theme(const QString& path, const int id);

    QColor backgroundColor() const;
    QString comment() const;
    qreal fill() const;
    QColor gridColor() const;
    GridType gridType() const;
    int id() const;
    QString name() const;
    QString path() const;
    QString svg() const;

private:
    QColor m_backgroundColor;
    QString m_comment;
    qreal m_fill;
    QColor m_gridColor;
    GridType m_gridType;
    int m_id;
    QString m_name;
    QString m_path;
    QString m_svg;
};

} /* namespace gui */

#endif // BOVO_THEME_H
