
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//
#include <QQuickView>

#include <QDir>
#include <QQmlEngine>

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
#include <QCommandLineParser>
#include <QSettings>
#endif

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "MyApplication.h"
#include "OAuthRedirectExample.h"

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
  MyApplication app(argc, argv);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  QCommandLineParser commandLineParser;
  commandLineParser.process(app);

  auto argList = commandLineParser.positionalArguments();

  if (app.sendMessage(argList.join(",")))
    return 0;
#endif

#ifdef Q_OS_WIN
  // Force usage of OpenGL ES through ANGLE on Windows
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

  const QString urlScheme = OAuthRedirectExample::customUrlProtocol();
  const QString appPath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
  const QString regPath = QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\") + urlScheme;

  QScopedPointer<QSettings> reg(new QSettings(regPath, QSettings::NativeFormat));

  reg->setValue(QStringLiteral("Default"), QString("URL:%1 protocol").arg(urlScheme));
  reg->setValue(QStringLiteral("URL Protocol"), QString());

  reg->beginGroup(QStringLiteral("DefaultIcon"));
  reg->setValue(QStringLiteral("Default"), QString("%1,1").arg(appPath));
  reg->endGroup();

  reg->beginGroup(QStringLiteral("shell"));
  reg->beginGroup(QStringLiteral("open"));
  reg->beginGroup(QStringLiteral("command"));
  reg->setValue(QStringLiteral("Default"), appPath + QLatin1String(" %1"));
#endif

  // Register the OAuthRedirectTest (QQuickItem) for QML
  qmlRegisterType<OAuthRedirectExample>("Esri.Samples", 1, 0, "OAuthRedirectExample");

  // Intialize application view
  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);

  // Add the import Path
  view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  // Add the Extras path
  view.engine()->addImportPath(QUOTE(ARCGIS_RUNTIME_IMPORT_PATH));
  // Add the Toolkit path
  view.engine()->addImportPath(QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH));

  // Set the source
  view.setSource(QUrl("qrc:/Samples/CloudAndPortal/OAuthRedirectExample/OAuthRedirectExample.qml"));

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  QObject::connect(&app, &MyApplication::messageReceived, [&app, &view](const QString& msg)
  {
    Q_UNUSED(msg);
    view.showMinimized();
    app.activateWindow();
    view.show();
    view.raise();
  });
#endif

  view.show();

  return app.exec();
}
