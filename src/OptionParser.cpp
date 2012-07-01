#include "OptionParser.h"
#include <QtCore/QDir>
#include "ProgramOptions.h"
#include "ThumbTask.h"
#include "tools/Tools.h"
#include "Config.h"

//priority: cmd > config.ini > auto detect
namespace po = ProgramOptions;

namespace PhotoKit {
QStringList OptionParser::images;
void OptionParser::parseCmd(int argc, const char *const*argv)
{
    static OptionParser cmd;
	po::parse(argc, argv);
    if (po::get("h"))
        po::help();
    if (po::get("opengl")) {
        Config::openGlRendering = true;
    }
    if (po::get("no-opengl")) {
        Config::openGlRendering = false;
    }
    QString d(po::get("d").str().c_str());
    if (!d.isEmpty()) {
        getImagesFromDirs(d.split(";"));
    }
    QString f(po::get("f").str().c_str());
	if (!f.isEmpty()) {
		Config::showLastDisplayed = false;
		images << f.split(";");
	}
	if (images.isEmpty()) {
		images << ThumbRecorder::displayedThumbs();
	}
}

OptionParser::OptionParser()
{
    po::summary("PhotoKit is a photo browser with impressive 3d effects.\n"
                            "Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>")
            ["Usage:"]
            ("-h,--help", "show this message")
            ("--opengl", "enable opengl")
            ("--no-opengl", "disable opengl")
            ("-d,--dirs", "", "load images from dirs")
            ("-f,--files", "", "load image from files")
    ;

	//the read after detect so that the configuration in config file will be applied
	Config::detectSystemResources();
	Config::read();
	Config::postConfigure();
}

void OptionParser::getImagesFromDirs(const QStringList &dirs)
{
    images.clear();
    foreach(const QString& dir, dirs) {
        QDir d(dir);
        if (!d.exists())
            continue;
		QStringList list = d.entryList(Tools::imageNameFilters(), QDir::Files);
        list.replaceInStrings(QRegExp("^(.*)"), dir + "/\\1");
        images << list;
    }
}

}