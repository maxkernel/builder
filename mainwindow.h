#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <maxmodel/model.h>
#include "script.h"
#include "module.h"
#include "block.h"
#include "blockinstance.h"
#include "graphicsview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveAsProject();

    void newScript();
    void removeScript();
    void exportScript();

    void loadModule();
    void removeModule();
    void configModule();
    void newInstance();

    void updateScriptList();
    void updateBlockList();

protected:
    void closeEvent(QCloseEvent *);

private:
    Script *getSelectedScript() { return dynamic_cast<Script *>(scripts->currentItem()); }
    Module *getSelectedModule() { return dynamic_cast<Module *>(blocks->currentItem()); }
    Block *getSelectedBlock() { return dynamic_cast<Block *>(blocks->currentItem()); }

    bool querySave();

    QAction *newProjectAct, *openProjectAct, *saveProjectAct, *saveProjectAsAct;
    QAction *newScriptAct, *removeScriptAct, *exportScriptAct;
    QAction *loadModuleAct, *removeModuleAct, *configModuleAct, *newInstanceAct;

    QGraphicsScene *scene;
    GraphicsView *view;
    QListWidget *scripts;
    QTreeWidget *blocks;
    QPlainTextEdit *info;

    QString path;
    model_t *model;
};

#endif // MAINWINDOW_H
