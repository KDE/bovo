/*******************************************************************
*
* Copyright 2007  Aron Boström <c02ab@efd.lth.se>
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


#include "theme.h"

#include "settings.h"

#include <KConfig>
#include <KConfigGroup>
#include <KDesktopFile>

#include <QStandardPaths>

namespace gui {

Theme::Theme() {
}

Theme::Theme(const QString& path, const int id)
  : m_id(id), m_path(path) {
    QString themePath = QStringLiteral("themes/%1/").arg(m_path);
            themePath = QStandardPaths::locate(QStandardPaths::AppDataLocation, themePath, QStandardPaths::LocateDirectory);
    QString themerc = themePath + QLatin1String("themerc");
    KDesktopFile themeConfig(themerc);
    m_name = themeConfig.readName();
    m_comment = themeConfig.readComment();

    KConfig config(themerc);
    KConfigGroup configGroup(&config, "Config");
    m_backgroundColor = configGroup.readEntry("BackgroundColor", "white");
    m_fill = configGroup.readEntry("Fill", 0.75);
    m_gridColor = configGroup.readEntry("GridColor", "black");

    QString gridTypeStr = configGroup.readEntry("GridType", "svg");
    if (gridTypeStr == QLatin1String("svg")) {
        m_gridType = SvgGrid;
    } else if (gridTypeStr == QLatin1String("gomoku")) {
        m_gridType = GomokuGrid;
    } else if (gridTypeStr == QLatin1String("squares")) {
        m_gridType = SquaresGrid;
    }

    m_svg = themePath + configGroup.readEntry("Svg", "theme.svg");
}

QColor Theme::backgroundColor() const {
    return m_backgroundColor;
}

QString Theme::comment() const {
    return m_comment;
}

qreal Theme::fill() const {
    return m_fill;
}

QColor Theme::gridColor() const {
    return m_gridColor;
}

GridType Theme::gridType() const {
    return m_gridType;
}

int Theme::id() const {
    return m_id;
}

QString Theme::name() const {
    return m_name;
}

QString Theme::path() const {
    return m_path;
}

QString Theme::svg() const {
    return m_svg;
}



} /* namespace gui */
