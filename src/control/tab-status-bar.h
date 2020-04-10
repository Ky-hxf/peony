/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: MeihongHe <hemeihong@kylinos.cn>
 *
 */

#ifndef TABSTATUSBAR_H
#define TABSTATUSBAR_H

#include <QStatusBar>

class QLabel;
class QToolBar;
class TabWidget;
class ElidedLabel;

class TabStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit TabStatusBar(TabWidget *tab, QWidget *parent = nullptr);
    ~TabStatusBar() override;

public Q_SLOTS:
    void update();
    void update(const QString &message);

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    TabWidget *m_tab = nullptr;
    ElidedLabel *m_label = nullptr;
    QToolBar *m_styled_toolbar = nullptr;
};

class ElidedLabel : public QWidget
{
    Q_OBJECT
public:
    explicit ElidedLabel(QWidget *parent);

    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
};

#endif // TABSTATUSBAR_H