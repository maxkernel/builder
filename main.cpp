#include <QApplication>
#include "mainwindow.h"
#include "main.h"

const char *Global::getType(char t)
{
    switch (t)
    {
        case 'v':       return "Void";
        case 'b':       return "Boolean";
        case 'i':       return "Integer";
        case 'd':       return "Double";
        case 'c':       return "Character";
        case 's':       return "String";
        case 'X':       return "Buffer";
        case 'B':       return "Boolean Array";
        case 'I':       return "Integer Array";
        case 'D':       return "Double Array";
        default:        return "Unknown Type";
    }
}

model_t *Global::open(const QString &filepath)
{
    return model_new();
}



namespace Private
{
namespace Save
{
    void * a_scripts(void * udata, const model_t * model, const model_script_t * script);
};};

void * Private::Save::a_scripts(void * udata, const model_t * model, const model_script_t * script)
{
    return NULL;
}

bool Global::save(model_t *model, const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::warning(NULL, "Error saving file", "Unexpected error saving project file");
        return false;
    }

    file.write("Test");

    model_analysis_t funcs;
    funcs.scripts = Private::Save::a_scripts;

    model_analyse(model, traversal_scripts_modules_configs_linkables_links, &funcs);

    file.close();
    return true;
}



int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    app.setOrganizationName("MaxKernel");
    app.setApplicationName("Kernel Builder");

    MainWindow win;
    win.show();

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    app.exec();

    return 0;
}
