#include "mainwindow.h"
#include "main.h"

namespace NewBlock
{
    model_linkable_t *newInstance(model_t *model, model_script_t *script, const QString &name, const void *userdata);
    model_linkable_t *newSyscall(model_t *model, model_script_t *script, const QString &name, const void *userdata);
    model_linkable_t *newRategroup(model_t *model, model_script_t *script, const QString &name, const void *userdata);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), model(NULL)
{
    setWindowTitle("Kernel Builder");

    scene = new QGraphicsScene(this);
    view = new GraphicsView(scene);
    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setCentralWidget(view);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    // Create the actions
    {
        newProjectAct = new QAction(QIcon(":/images/newproject.png"), tr("&New"), this);
        newProjectAct->setShortcut(QKeySequence::New);
        newProjectAct->setStatusTip("Create a new project file");
        connect(newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));

        openProjectAct = new QAction(QIcon(":/images/openproject.png"), tr("&Open"), this);
        openProjectAct->setShortcuts(QKeySequence::Open);
        openProjectAct->setStatusTip("Open an existing project file");
        connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

        saveProjectAct = new QAction(QIcon(":/images/saveproject.png"), tr("&Save"), this);
        saveProjectAct->setShortcuts(QKeySequence::Save);
        saveProjectAct->setStatusTip("Save project file");
        connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

        saveProjectAsAct = new QAction(QIcon(":/images/saveasproject.png"), tr("Save &As"), this);
        saveProjectAsAct->setShortcuts(QKeySequence::SaveAs);
        saveProjectAsAct->setStatusTip("Save current project file as new project file");
        connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveAsProject()));

        newScriptAct = new QAction(QIcon(":/images/newscript.png"), tr("New Script"), this);
        newScriptAct->setStatusTip("Create new script in project");
        connect(newScriptAct, SIGNAL(triggered()), this, SLOT(newScript()));

        removeScriptAct = new QAction(QIcon(":/images/removescript.png"), tr("Remove Script"), this);
        removeScriptAct->setStatusTip("Remove selected script from project");
        connect(removeScriptAct, SIGNAL(triggered()), this, SLOT(removeScript()));

        exportScriptAct = new QAction(QIcon(":/images/exportscript.png"), tr("Export Script"), this);
        exportScriptAct->setStatusTip("Export the selected script");
        connect(exportScriptAct, SIGNAL(triggered()), this, SLOT(exportScript()));

        loadModuleAct = new QAction(QIcon(":/images/loadmodule.png"), tr("Load Module"), this);
        loadModuleAct->setStatusTip("Load a module into active script");
        connect(loadModuleAct, SIGNAL(triggered()), this, SLOT(loadModule()));

        removeModuleAct = new QAction(QIcon(":/images/removemodule.png"), tr("Remove Module"), this);
        removeModuleAct->setStatusTip("Remove the selected module from the script");
        connect(removeModuleAct, SIGNAL(triggered()), this, SLOT(removeModule()));

        configModuleAct = new QAction(QIcon(":/images/configmodule.png"), tr("Configure Module"), this);
        configModuleAct->setStatusTip("Configure selected module properties");
        connect(configModuleAct, SIGNAL(triggered()), this, SLOT(configModule()));

        newInstanceAct = new QAction(QIcon(":/images/newinstance.png"), tr("New Instance"), this);
        newInstanceAct->setStatusTip("Create new block instance from selection");
        connect(newInstanceAct, SIGNAL(triggered()), this, SLOT(newInstance()));
    }

    // Create the toolbars
    {
        QToolBar *fileToolBar = addToolBar(tr("File"));
        fileToolBar->addAction(newProjectAct);
        fileToolBar->addAction(openProjectAct);
        fileToolBar->addAction(saveProjectAct);
        fileToolBar->addAction(saveProjectAsAct);
    }

    // Create block dock
    {
        QToolBar *blockToolbar = new QToolBar;
        blockToolbar->addAction(loadModuleAct);
        blockToolbar->addAction(removeModuleAct);
        blockToolbar->addAction(configModuleAct);
        blockToolbar->addAction(newInstanceAct);

        blocks = new QTreeWidget;
        blocks->setColumnCount(1);
        blocks->header()->close();

        QVBoxLayout *blockPanelLayout = new QVBoxLayout;
        blockPanelLayout->setSpacing(0);
        blockPanelLayout->addWidget(blockToolbar);
        blockPanelLayout->addWidget(blocks);

        QWidget *blockPanel = new QWidget;
        blockPanel->setLayout(blockPanelLayout);

        QDockWidget *blockDock = new QDockWidget(tr("Blocks"), this);
        blockDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        blockDock->setWidget(blockPanel);
        addDockWidget(Qt::LeftDockWidgetArea, blockDock);
    }

    // Create script dock
    {
        QToolBar *scriptToolbar = new QToolBar;
        scriptToolbar->addAction(newScriptAct);
        scriptToolbar->addAction(removeScriptAct);
        scriptToolbar->addAction(exportScriptAct);

        scripts = new QListWidget;
        scripts->setSortingEnabled(true);
        connect(scripts, SIGNAL(itemSelectionChanged()), this, SLOT(updateBlockList()));
        connect(scripts, SIGNAL(itemSelectionChanged()), this, SLOT(updateView()));

        QVBoxLayout *scriptPanelLayout = new QVBoxLayout;
        scriptPanelLayout->setSpacing(0);
        scriptPanelLayout->addWidget(scriptToolbar);
        scriptPanelLayout->addWidget(scripts);

        QWidget *scriptPanel = new QWidget;
        scriptPanel->setLayout(scriptPanelLayout);

        QDockWidget *scriptDock = new QDockWidget(tr("Scripts"), this);
        scriptDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        scriptDock->setWidget(scriptPanel);
        addDockWidget(Qt::LeftDockWidgetArea, scriptDock);
    }

    // Create inspector dock
    {
        info = new QPlainTextEdit();
        info->setEnabled(false);

        QDockWidget *inspectorDock = new QDockWidget(tr("Inspector"), this);
        inspectorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        inspectorDock->setWidget(info);
        addDockWidget(Qt::RightDockWidgetArea, inspectorDock);
    }

    // Create the status bar
    {
        statusBar()->showMessage("Ready.");
    }

    // Read the window geometry settings
    {
        QSettings settings("MaxKernel", "KBuilder");
        QPoint position = settings.value("position", QPoint(200, 200)).toPoint();
        QSize size = settings.value("size", QSize(400, 400)).toSize();
        resize(size);
        move(position);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateScriptList()
{
    scripts->clear();

    if (model == NULL)
    {
        return;
    }

    iterator_t sitr = model_scriptitr(model);
    {
        const model_script_t *script = NULL;
        while (model_scriptnext(sitr, &script))
        {
            const char *s_name = NULL;
            model_getscript(script, &s_name);

            scripts->addItem(new Script(s_name, script));
        }
    }
    iterator_free(sitr);
}

void MainWindow::updateBlockList()
{
    blocks->clear();

    if (model == NULL)
    {
        return;
    }

    Script *s = getSelectedScript();
    if (s == NULL)
    {
        return;
    }

    blocks->addTopLevelItem(new Block(NULL, QIcon(":/images/syscall.png"), "New Syscall", NewBlock::newSyscall, NULL));
    blocks->addTopLevelItem(new Block(NULL, QIcon(":/images/rategroup.png"), "New Rategroup", NewBlock::newRategroup, NULL));

    iterator_t mitr = model_moduleitr(model, s->getScript());
    {
        const model_module_t *module = NULL;
        while (model_modulenext(mitr, &module))
        {
            const meta_t *meta = NULL;
            model_getmodule(module, NULL, &meta);

            const char *module_name = NULL;
            meta_getinfo(meta, NULL, &module_name, NULL, NULL, NULL);

            Module *module_item = new Module(NULL, QIcon(":/images/module.png"), module_name, module);
            iterator_t bitr = meta_blockitr(meta);
            {
                const meta_block_t *block = NULL;
                while(meta_blocknext(bitr, &block))
                {
                    const char * block_name = NULL;
                    meta_getblock(block, &block_name, NULL, NULL, NULL, NULL, NULL);
                    module_item->addChild(new Block(module_item, QIcon(":/images/block.png"), block_name, NewBlock::newInstance, module_item));
                }
            }
            iterator_free(bitr);

            blocks->addTopLevelItem(module_item);
            module_item->setExpanded(true);
        }
    }
    iterator_free(mitr);
}

void MainWindow::updateView()
{
    view->closeScript();

    Script *s = getSelectedScript();
    if (s == NULL)
    {
        return;
    }

    const model_script_t *script = s->getScript();

    view->openScript(model, script);
}

bool MainWindow::querySave()
{
    if (model == NULL)
    {
        return false;
    }

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Save existing project?");
    msgBox.setInformativeText(QString("Would you like to save the existing project file '%1'?").arg(path));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setEscapeButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    switch (msgBox.exec())
    {
    case QMessageBox::Yes:
        return true;

    default:
        return false;
    }
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    // Write the window geometry settings
    {
        QSettings settings("MaxKernel", "KBuilder");
        settings.setValue("position", pos());
        settings.setValue("size", size());
    }
}

void MainWindow::newProject()
{
    QString filename = QFileDialog::getSaveFileName(this, "Create new project file", QDir::currentPath(), "Kernel Stitcher File (*.stitcher)");
    if (filename.isEmpty())
    {
        return;
    }

    if (!filename.endsWith(".stitcher"))
    {
        filename += ".stitcher";
    }

    /* Dialog box already handles this!
    if (QFile::exists(filename) && QMessageBox::question(this, "Overwrite file?", "Are you sure you want to overwrite existing file?", tr("&Yes"), tr("&No"), QString::null, 0, 1) != 0)
    {
        return;
    }
    */

    if (querySave() && !Global::save(model, path))
    {
        QMessageBox::critical(this, "Failed to save", "Failed to save existing project. Aborting new project creation.");
        return;
    }

    model_destroy(model, NULL);
    model = NULL;


    model_t *newmodel = model_new();
    Global::save(newmodel, filename);

    // Save the directory path to prime the open dialog box next time
    QDir::setCurrent(QFileInfo(filename).absoluteDir().path());

    // Populate the class members
    path = filename;
    model = newmodel;

    // Refresh the entire view
    setWindowTitle(QString("Kernel Builder - %1").arg(path));
    view->closeScript();
    updateScriptList();
    updateBlockList();

    // Create a new script
    newScript();
}

void MainWindow::openProject()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open existing project file", QDir::currentPath(), "Kernel Stitcher File (*.stitcher)");
    if (filename.isEmpty())
    {
        return;
    }

    if (querySave() && !Global::save(model, path))
    {
        QMessageBox::critical(this, "Failed to save", "Failed to save existing project. Aborting project open.");
        return;
    }

    model_destroy(model, NULL);
    model = NULL;

    model_t *newmodel = Global::open(filename);
    if (newmodel == NULL)
    {
        QMessageBox::critical(this, "Open failed", "Opening selected project failed! (Corrupt file?)");
    }

    // Save the directory path to prime the open dialog box next time
    QDir::setCurrent(QFileInfo(filename).absoluteDir().path());

    path = filename;
    model = newmodel;

    setWindowTitle(QString("Kernel Builder - %1").arg(path));
    view->closeScript();
    updateScriptList();
    updateBlockList();

    updateScriptList();
}

void MainWindow::saveProject()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Please create/open project before attempting to save it");
        return;
    }

    if (!Global::save(model, path))
    {
        QMessageBox::critical(this, "Save failed", "Failed to save project!");
    }
}

void MainWindow::saveAsProject()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Please create/open project before attempting to save it");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save project file as...", QDir::currentPath(), "Kernel Stitcher File (*.stitcher)");
    if (filename.isEmpty())
    {
        return;
    }

    if (!filename.endsWith(".stitcher"))
    {
        filename += ".stitcher";
    }

    if (!Global::save(model, filename))
    {
        QMessageBox::critical(this, "Save failed", "Failed to save project!");
    }

    // Save the directory path to prime the open dialog box next time
    QDir::setCurrent(QFileInfo(filename).absoluteDir().path());

    path = filename;

    setWindowTitle(QString("Kernel Builder - %1").arg(path));
}

void MainWindow::newScript()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Must create/open project before creating new script");
        return;
    }

    QString name = QInputDialog::getText(this, "Enter new script name", "Please enter the name of the new script.");
    if (name.isNull())
    {
        return;
    }

    exception_t *e = NULL;
    model_script_t *script = model_newscript(model, name.toAscii().constData(), &e);

    if (script == NULL || exception_check(&e))
    {
        QMessageBox::critical(this, "Script creation error", QString("Could not create script: %1").arg(e == NULL? "Unknown error" : e->message));
        return;
    }

    Script *s = new Script(name, script);
    model_setuserdata(model_object(script), s);

    scripts->addItem(s);
    scripts->scrollToItem(s);
    scripts->setCurrentItem(s);
}

void MainWindow::removeScript()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Must create/open project before removing script");
        return;
    }

    Script *s = getSelectedScript();
    if (s == NULL)
    {
        statusBar()->showMessage("Error: Must select script before attempting to remove it");
        return;
    }

    const model_script_t *script = s->getScript();
    model_destroy(model, model_object(script));

    updateScriptList();
}

void MainWindow::exportScript()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Please create/open project before exporting script");
        return;
    }

    QMessageBox::information(this, "TODO", "Unimplemented!!");
}

void MainWindow::loadModule()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Please create/open project before loading a module");
        return;
    }

    Script *s = getSelectedScript();
    if (s == NULL)
    {
        return;
    }

    const model_script_t *script = s->getScript();

    QString filename = QFileDialog::getOpenFileName(this, "Select which module to load", QDir::currentPath(), "MaxKernel module (*.mo)");
    if (filename.isEmpty())
    {
        return;
    }

    exception_t *e = NULL;
    meta_t *meta = meta_parseelf(filename.toAscii().constData(), &e);
    if (meta == NULL || exception_check(&e))
    {
        QMessageBox::warning(0, "Load error", (e == NULL)? "Unknown error" : e->message);
        exception_free(e);
        return;
    }

    model_module_t * module = model_newmodule(model, (model_script_t *)script, meta, &e);
    meta_destroy(meta);

    if (module == NULL || exception_check(&e))
    {
        QMessageBox::warning(0, "Creation error", (e == NULL)? "Unknown error" : e->message);
        exception_free(e);
        return;
    }

    // Save the directory path to prime the open dialog box next time
    QDir::setCurrent(QFileInfo(filename).absoluteDir().path());

    updateBlockList();

    // Select the newly loaded module
    {
        int count = blocks->topLevelItemCount();
        for (int i = 0; i < count; i++)
        {
            Module *m = dynamic_cast<Module *>(blocks->topLevelItem(i));
            if (m == NULL)
            {
                continue;
            }

            if (model_objectequals(model_object(module), model_object(m->getModule())))
            {
                blocks->setCurrentItem(m);
                break;
            }
        }
    }
}

void MainWindow::removeModule()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Must create/open project before attempting to remove a module");
        return;
    }

    Module *m = getSelectedModule();
    if (m == NULL)
    {
        statusBar()->showMessage("Error: Must select a module to remove");
        return;
    }

    const model_module_t *module = m->getModule();
    model_destroy(model, model_object(module));

    updateBlockList();
}

void MainWindow::configModule()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Must create or open a project and load a module before you can configure it");
        return;
    }

    Module *m = getSelectedModule();
    if (m == NULL)
    {
        statusBar()->showMessage("Error: Must select a module to configure");
        return;
    }

    const model_module_t *module = m->getModule();

    // Pull the meta out of the module
    const meta_t *meta = NULL;
    model_getmodule(module, NULL, &meta);


    // Iterate over configs, creating GUI entry for each
    QMap<const meta_variable_t *, QLineEdit *> configs;
    iterator_t citr = meta_configitr(meta);
    {
        const meta_variable_t *config = NULL;
        while(meta_confignext(citr, &config))
        {
            configs[config] = new QLineEdit;

            const char *v_name = NULL;
            meta_getvariable(config, &v_name, NULL, NULL, NULL);

            const model_config_t *config_entry = NULL;
            if (model_lookupconfig(model, module, v_name, &config_entry))
            {
                const char *value = NULL;
                model_getconfig(config_entry, NULL, NULL, NULL, &value);
                configs[config]->setText(value);
            }
        }
    }
    iterator_free(citr);


    QDialog popup;
    popup.resize(500, 400);

    // Create the dialog
    {
        QGridLayout *popupLayout = new QGridLayout;

        popupLayout->addWidget(new QLabel("Module Configuration"), 0, 0, 1, 2);

        QPlainTextEdit *config_info = new QPlainTextEdit;
        config_info->setEnabled(false);
        popupLayout->addWidget(config_info, 1, 0, 1, 2);

        int row = 2;
        QMap<const meta_variable_t *, QLineEdit *>::iterator i;
        for (i = configs.begin(); i != configs.end(); i += 1, row += 1)
        {
            const char * v_name = NULL, *v_desc = NULL;
            char v_sig = '\0';

            meta_getvariable(i.key(), &v_name, &v_sig, &v_desc, NULL);      // TODO - Print the default value of the config
            config_info->appendPlainText(QString("%1: %2\n").arg(v_name, v_desc));

            popupLayout->addWidget(new QLabel(QString("(%1) %2").arg(Global::getType(v_sig), v_name)), row, 0);
            popupLayout->addWidget(i.value(), row, 1);
        }

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, SIGNAL(accepted()), &popup, SLOT(accept()));
        connect(buttons, SIGNAL(rejected()), &popup, SLOT(reject()));
        popupLayout->addWidget(buttons, row, 0, 1, 2);

        popup.setLayout(popupLayout);
    }

    // Show the dialog
    {
        if (popup.exec() != QDialog::Accepted)
        {
            return;
        }
    }

    // Update the model_config_t's
    {
        QMap<const meta_variable_t *, QLineEdit *>::iterator i;
        for (i = configs.begin(); i != configs.end(); i += 1)
        {
            const char * v_name = NULL;
            meta_getvariable(i.key(), &v_name, NULL, NULL, NULL);

            const QString config_name(v_name);
            const model_config_t *rmconfig = NULL;
            if (model_lookupconfig(model, module, v_name, &rmconfig))
            {
                // Delete this config value
                model_destroy(model, model_object(rmconfig));
            }

            QString value = i.value()->text();
            if (!value.isEmpty())
            {
                exception_t *e = NULL;
                model_config_t *config = model_newconfig(model, (model_module_t *)module, config_name.toAscii().constData(), value.toAscii().constData(), &e);
                if (config == NULL || exception_check(&e))
                {
                    QMessageBox::critical(this, "Error setting config", QString("Could not set config value %1\nReason: %2").arg(config_name, e == NULL? "Unknown error" : e->message));
                }
            }
        }
    }
}

void MainWindow::newInstance()
{
    if (model == NULL)
    {
        statusBar()->showMessage("Error: Must create or open a project and load a module before creating new block instance");
        return;
    }

    Script *s = getSelectedScript();
    if (s == NULL)
    {
        statusBar()->showMessage("Error: Must select a script first");
        return;
    }
    const model_script_t *script = s->getScript();

    Block *b = getSelectedBlock();
    if (b == NULL)
    {
        statusBar()->showMessage("Error: Must select a block to insert");
        return;
    }

    model_linkable_t *l = b->create(model, (model_script_t *)script);
    if (l == NULL)
    {
        return;
    }

    view->addLinkable(l);
}

model_linkable_t *NewBlock::newInstance(model_t *model, model_script_t *script, const QString &name, const void *userdata)
{
    const Module *m = (const Module *)userdata;

    const model_module_t *module = m->getModule();

    const meta_t *meta = NULL;
    model_getmodule(module, NULL, &meta);

    const meta_block_t *block = NULL;
    meta_lookupblock(meta, name.toAscii().constData(), &block);

    const char *block_name = NULL, *block_desc = NULL;
    const char *constructor_sig = NULL, *constructor_desc = NULL;
    meta_getblock(block, &block_name, &block_desc, NULL, &constructor_sig, &constructor_desc, NULL);

    QList<QLineEdit *> args;

    // Create the forms
    QDialog popup;
    popup.resize(500, 400);

    // Create the GUI
    {
        QGridLayout *popupLayout = new QGridLayout;

        popupLayout->addWidget(new QLabel(block_desc), 0, 0, 1, 2);

        QString constructor_str(constructor_desc);
        constructor_str.replace(" (", "\n(");
        QPlainTextEdit *constructor_info = new QPlainTextEdit(constructor_str);
        constructor_info->setEnabled(false);
        popupLayout->addWidget(constructor_info, 1, 0, 1, 2);

        int row = 2;
        QByteArray sig(constructor_sig);
        for (int i=0; i<sig.length(); i++)
        {
            QLineEdit * arg = new QLineEdit();
            args.append(arg);

            popupLayout->addWidget(new QLabel(QString(Global::getType(sig.data()[i]))), row, 0);
            popupLayout->addWidget(arg, row, 1);

            row += 1;
        }

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttons, SIGNAL(accepted()), &popup, SLOT(accept()));
        QObject::connect(buttons, SIGNAL(rejected()), &popup, SLOT(reject()));
        popupLayout->addWidget(buttons, row, 0, 1, 2);

        popup.setLayout(popupLayout);
    }

    if (popup.exec() != QDialog::Accepted)
    {
        return NULL;
    }

    // Parse out the args
    const char ** args_raw = new const char *[args.size()];
    for (int i=0; i<args.size(); i++)
    {
        args_raw[i] = args.at(i)->text().toAscii().constData();
    }

    // Create the block instance
    exception_t *e = NULL;
    model_linkable_t *linkable = model_newblockinst(model, (model_module_t *)module, script, block_name, args_raw, args.size(), &e);
    if (linkable == NULL || exception_check(&e))
    {
        QMessageBox::warning(0, "BlockInstance creation error", (e == NULL)? "Unknown error" : e->message);
        exception_free(e);
        return NULL;
    }

    // Create the userdata
    QPair<QPointF, QList<IOEntry *> > *data = new QPair<QPointF, QList<IOEntry *> >;
    model_setuserdata(model_object(linkable), data);

    iterator_t ioitr = meta_blockioitr(meta);
    {
        const meta_blockio_t *blockio = NULL;
        while (meta_blockionext(ioitr, &blockio))
        {
            const char * test_block_name = NULL;
            const char * io_name = NULL;
            meta_iotype_t io_type = meta_unknownio;
            char io_sig = 0;
            meta_getblockio(blockio, &test_block_name, &io_name, &io_type, &io_sig, NULL);

            if (strcmp(block_name, test_block_name) == 0)
            {
                IOEntry::Type type = IOEntry::Unknown;

                switch (io_type)
                {
                    case meta_input:    type = IOEntry::Input;       break;
                    case meta_output:   type = IOEntry::Output;      break;
                    default:            continue;
                }

                data->second.append(new IOEntry(linkable, type, io_name, io_sig));
            }
        }
    }
    iterator_free(ioitr);

    return linkable;
}

model_linkable_t *NewBlock::newSyscall(model_t *model, model_script_t *script, const QString &name, const void *userdata)
{
    Q_UNUSED(userdata);
    return NULL;
}

model_linkable_t *NewBlock::newRategroup(model_t *model, model_script_t *script, const QString &name, const void *userdata)
{
    Q_UNUSED(userdata);
    return NULL;
}
