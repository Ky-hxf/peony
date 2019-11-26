#include "desktop-menu.h"

#include "directory-view-plugin-iface.h"

#include "file-info.h"
#include "file-utils.h"
#include "file-launch-action.h"
#include "file-launch-manager.h"

#include "clipboard-utils.h"
#include "file-operation-utils.h"
#include "file-enumerator.h"

#include "desktop-icon-view.h"

#include "desktop-menu-plugin-manager.h"

#include <QProcess>
#include <QStandardPaths>
#include <QMessageBox>

#include <QUrl>

using namespace Peony;

DesktopMenu::DesktopMenu(DirectoryViewIface *view, QWidget *parent) : QMenu(parent)
{
    m_view = view;
    m_directory = view->getDirectoryUri();
    m_selections = view->getSelections();

    fillActions();
}

void DesktopMenu::fillActions()
{
    //add open actions
    auto openActions = constructOpenOpActions();
    if (!openActions.isEmpty())
        addSeparator();

    //add view actions
    auto viewActions = constructViewOpActions();
    if (!viewActions.isEmpty())
        addSeparator();

    //add operation actions
    auto fileOpActions = constructFileOpActions();
    if (!fileOpActions.isEmpty())
        addSeparator();

    //add plugin actions
    auto pluginActions = constructMenuPluginActions();
    if (!pluginActions.isEmpty())
        addSeparator();

    //add propertries actions
    auto propertiesAction = constructFilePropertiesActions();
}

const QList<QAction *> DesktopMenu::constructOpenOpActions()
{
    QList<QAction *> l;

    bool isBackgroundMenu = m_selections.isEmpty();
    if (isBackgroundMenu) {
        l<<addAction(QIcon::fromTheme("window-new-symbolic"), tr("&Open in new Window"));
        connect(l.last(), &QAction::triggered, [=](){
            this->openWindow(m_directory);
        });
    } else {
        if (m_selections.count() == 1) {
            auto info = FileInfo::fromUri(m_selections.first());
            auto displayName = info->displayName();
            if (displayName.isEmpty())
                displayName = FileUtils::getFileDisplayName(info->uri());
            if (info->isDir()) {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open \"%1\"").arg(displayName));
                connect(l.last(), &QAction::triggered, [=](){
                    this->openWindow(m_selections);
                });

                auto openWithAction = addAction(tr("Open \"%1\" &with...").arg(displayName));
                QMenu *openWithMenu = new QMenu(this);
                auto recommendActions = FileLaunchManager::getRecommendActions(m_selections.first());
                for (auto action : recommendActions) {
                    action->setParent(openWithMenu);
                    openWithMenu->addAction(static_cast<QAction*>(action));
                }
                auto fallbackActions = FileLaunchManager::getFallbackActions(m_selections.first());
                for (auto action : fallbackActions) {
                    action->setParent(openWithMenu);
                    openWithMenu->addAction(static_cast<QAction*>(action));
                }
                openWithMenu->addSeparator();
                openWithMenu->addAction(tr("&More applications..."));
                openWithAction->setMenu(openWithMenu);
            } else if (!info->isVolume()) {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open \"%1\"").arg(displayName));
                connect(l.last(), &QAction::triggered, [=](){
                    auto uri = m_selections.first();
                    FileLaunchManager::openAsync(uri);
                });
                auto openWithAction = addAction(tr("Open \"%1\" with...").arg(displayName));
                //FIXME: add sub menu for open with action.
                QMenu *openWithMenu = new QMenu(this);
                auto recommendActions = FileLaunchManager::getRecommendActions(m_selections.first());
                for (auto action : recommendActions) {
                    action->setParent(openWithMenu);
                    openWithMenu->addAction(static_cast<QAction*>(action));
                }
                auto fallbackActions = FileLaunchManager::getFallbackActions(m_selections.first());
                for (auto action : fallbackActions) {
                    action->setParent(openWithMenu);
                    openWithMenu->addAction(static_cast<QAction*>(action));
                }
                openWithMenu->addSeparator();
                openWithMenu->addAction(tr("&More applications..."));
                openWithAction->setMenu(openWithMenu);
            } else {
                l<<addAction(tr("&Open"));
                connect(l.last(), &QAction::triggered, [=](){
                    auto uri = m_selections.first();
                    //FIXME:
                });
            }
        } else {
            l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open %1 selected files").arg(m_selections.count()));
            connect(l.last(), &QAction::triggered, [=](){
                qDebug()<<"triggered";
                QStringList dirs;
                QStringList files;
                for (auto uri : m_selections) {
                    auto info = FileInfo::fromUri(uri);
                    if (info->isDir() || info->isVolume()) {
                        dirs<<uri;
                    } else {
                        files<<uri;
                    }
                }
                if (!dirs.isEmpty())
                    this->openWindow(dirs);
                if (!files.isEmpty()) {
                    for (auto uri : files) {
                        FileLaunchManager::openAsync(uri);
                    }
                }
            });
        }
    }

    return l;
}

const QList<QAction *> DesktopMenu::constructViewOpActions()
{
    QList<QAction *> l;



    if (m_selections.isEmpty()) {
        auto viewTypeAction = addAction(tr("View Type..."));
        l<<viewTypeAction;
        QMenu *viewTypeSubMenu = new QMenu(this);
        auto desktopView = dynamic_cast<DesktopIconView*>(m_view);
        auto zoomLevel = desktopView->zoomLevel();

        auto smallAction = viewTypeSubMenu->addAction(tr("&Small"), [=](){
            desktopView->setDeafultZoomLevel(DesktopIconView::Small);
        });
        auto normalAction = viewTypeSubMenu->addAction(tr("&Normal"), [=](){
            desktopView->setDeafultZoomLevel(DesktopIconView::Normal);
        });
        auto largeAction = viewTypeSubMenu->addAction(tr("&Large"), [=](){
            desktopView->setDeafultZoomLevel(DesktopIconView::Large);
        });
        auto hugeAction = viewTypeSubMenu->addAction(tr("&Huge"), [=](){
            desktopView->setDeafultZoomLevel(DesktopIconView::Huge);
        });

        switch (zoomLevel) {
        case DesktopIconView::Small:
            smallAction->setCheckable(true);
            smallAction->setChecked(true);
            break;
        case DesktopIconView::Normal:
            normalAction->setCheckable(true);
            normalAction->setChecked(true);
            break;
        case DesktopIconView::Large:
            largeAction->setCheckable(true);
            largeAction->setChecked(true);
            break;
        case DesktopIconView::Huge:
            hugeAction->setCheckable(true);
            hugeAction->setChecked(true);
            break;
        default:
            break;
        }

        viewTypeAction->setMenu(viewTypeSubMenu);

        //sort type
        auto sortTypeAction = addAction(tr("Sort By..."));
        l<<sortTypeAction;
        QMenu *sortTypeMenu = new QMenu(this);

        QList<QAction *> tmp;
        tmp<<sortTypeMenu->addAction(tr("Name"));
        tmp<<sortTypeMenu->addAction(tr("File Type"));
        tmp<<sortTypeMenu->addAction(tr("File Size"));
        tmp<<sortTypeMenu->addAction(tr("Modified Date"));
        int sortType = m_view->getSortType();
        if (sortType >= 0) {
            tmp.at(sortType)->setCheckable(true);
            tmp.at(sortType)->setChecked(true);
        }

        for (int i = 0; i < tmp.count(); i++) {
            connect(tmp.at(i), &QAction::triggered, [=](){
                m_view->setSortType(i);
            });
        }

        sortTypeAction->setMenu(sortTypeMenu);

        //sort order
        auto sortOrderAction = addAction(tr("Sort Order..."));
        l<<sortOrderAction;
        QMenu *sortOrderMenu = new QMenu(this);
        tmp.clear();
        tmp<<sortOrderMenu->addAction(tr("Ascending Order"));
        tmp<<sortOrderMenu->addAction(tr("Descending Order"));
        int sortOrder = m_view->getSortOrder();
        tmp.at(sortOrder)->setCheckable(true);
        tmp.at(sortOrder)->setChecked(true);

        for (int i = 0; i < tmp.count(); i++) {
            connect(tmp.at(i), &QAction::triggered, [=](){
                m_view->setSortOrder(i);
            });
        }

        sortOrderAction->setMenu(sortOrderMenu);

        l<<addAction(QIcon::fromTheme("zoom-in-symbolic"), tr("Zoom &In"), [=](){
            auto desktopView = dynamic_cast<DesktopIconView*>(m_view);
            desktopView->zoomIn();
        });
        l<<addAction(QIcon::fromTheme("zoom-out-symbolic"), tr("Zoom &Out"), [=](){
            auto desktopView = dynamic_cast<DesktopIconView*>(m_view);
            desktopView->zoomOut();
        });
    }

    return l;
}

const QList<QAction *> DesktopMenu::constructFileOpActions()
{
    QList<QAction *> l;

    if (!m_selections.isEmpty()) {
        if (m_selections.count() == 1 && m_selections.first() == "trash:///") {
            FileEnumerator e;
            e.setEnumerateDirectory("trash:///");
            e.enumerateSync();
            auto trashChildren = e.getChildrenUris();
            l<<addAction(QIcon::fromTheme("view-refresh-symbolic"), tr("&Restore all"), [=](){
                FileOperationUtils::restore(trashChildren);
            });
            l.last()->setEnabled(!trashChildren.isEmpty());
            l<<addAction(QIcon::fromTheme("edit-clear-symbolic"), tr("&Clean the trash"), [=](){
                auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                          "Once you start a deletion, the files deleting will never be "
                                                                                          "restored again."));
                if (result == QMessageBox::Yes) {
                    FileEnumerator e;
                    FileOperationUtils::remove(trashChildren);
                }
            });
            l.last()->setEnabled(!trashChildren.isEmpty());
        } else if (m_selections.count() == 1 && m_selections.first() == "computer:///") {

        } else {
            l<<addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("&Copy"));
            connect(l.last(), &QAction::triggered, [=](){
                ClipboardUtils::setClipboardFiles(m_selections, false);
            });
            l<<addAction(QIcon::fromTheme("edit-cut-symbolic"), tr("Cu&t"));
            connect(l.last(), &QAction::triggered, [=](){
                ClipboardUtils::setClipboardFiles(m_selections, true);
            });

            if (!m_selections.contains("trash:///")) {
                l<<addAction(QIcon::fromTheme("edit-delete-symbolic"), tr("&Delete"));
                connect(l.last(), &QAction::triggered, [=](){
                    FileOperationUtils::trash(m_selections, true);
                });
            }

            if (m_selections.count() == 1) {
                l<<addAction(QIcon::fromTheme("document-edit-symbolic"), tr("&Rename"));
                connect(l.last(), &QAction::triggered, [=](){
                    m_view->editUri(m_selections.first());
                });
            }
        }
    } else {
        auto pasteAction = addAction(QIcon::fromTheme("edit-paste-symbolic"), tr("&Paste"));
        l<<pasteAction;
        pasteAction->setEnabled(ClipboardUtils::isClipboardHasFiles());
        connect(l.last(), &QAction::triggered, [=](){
            ClipboardUtils::pasteClipboardFiles(m_directory);
        });
        l<<addAction(QIcon::fromTheme("view-refresh-symbolic"), tr("&Refresh"));
        connect(l.last(), &QAction::triggered, [=](){
            auto desktopView = dynamic_cast<DesktopIconView*>(m_view);
            desktopView->refresh();
        });
    }

    return l;
}

const QList<QAction *> DesktopMenu::constructFilePropertiesActions()
{
    QList<QAction *> l;

    l<<addAction(QIcon::fromTheme("preview-file"), tr("&Properties"));
    connect(l.last(), &QAction::triggered, [=](){
        //FIXME:
        if (m_selections.isEmpty()) {
            this->showProperties(m_directory);
        } else {
            this->showProperties(m_selections);
        }
    });

    return l;
}

void DesktopMenu::openWindow(const QString &uri)
{
    QUrl url = uri;
    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    p.setProgram("peony-qt");
    p.setArguments(QStringList()<<"--show-folders"<<url.toEncoded());
    p.startDetached();
#else
    p.startDetached("peony-qt", QStringList()<<"--show-folders"<<url.toEncoded);
#endif
}

const QList<QAction *> DesktopMenu::constructMenuPluginActions()
{
    QList<QAction *> l;
    //FIXME:
    auto mgr = DesktopMenuPluginManager::getInstance();
    if (mgr->isLoaded()) {
        auto plugins = mgr->getPlugins();
        qDebug()<<plugins;
        for (auto plugin : plugins) {
            auto actions = plugin->menuActions(MenuPluginInterface::DesktopWindow,
                                               m_directory,
                                               m_selections);
            for (auto action : actions) {
                action->setParent(this);
            }
            l<<actions;
        }
    }
    addActions(l);
    return l;
}

void DesktopMenu::openWindow(const QStringList &uris)
{
    QStringList args;
    for (auto arg : uris) {
        QUrl url = arg;
        args<<url.toEncoded();
    }
    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    p.setProgram("peony-qt");
    p.setArguments(QStringList()<<"--show-folders"<<args);
    p.startDetached();
#else
    p.startDetached("peony-qt", QStringList()<<"--show-folders"<<args);
#endif
}

void DesktopMenu::showProperties(const QString &uri)
{
    QUrl url = uri;
    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    p.setProgram("peony-qt");
    p.setArguments(QStringList()<<"--show-properties"<<url.toEncoded());
    p.startDetached();
#else
    p.startDetached("peony-qt", QStringList()<<"--show-properties"<<url.toEncoded());
#endif
}

void DesktopMenu::showProperties(const QStringList &uris)
{
    QStringList args;
    for (auto arg : uris) {
        QUrl url = arg;
        args<<url.toEncoded();
    }
    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    p.setProgram("peony-qt");
    p.setArguments(QStringList()<<"--show-properties"<<args);
    p.startDetached();
#else
    p.startDetached("peony-qt", QStringList()<<"--show-properties"<<args);
#endif
}
