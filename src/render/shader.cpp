#include "render/shader.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

QString Shader::preprocess(const QString &shaderFilePath) {
    QString output;
    QFile file(shaderFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.trimmed().startsWith("#include")) {
            int firstQuote = line.indexOf('"');
            int lastQuote = line.lastIndexOf('"');
            if (firstQuote != -1 && lastQuote != -1 && lastQuote > firstQuote) {
                QString includeName = line.mid(firstQuote + 1, lastQuote - firstQuote - 1);
                QString localPath = QFileInfo(shaderFilePath).path() + "/" + includeName;
                QString includePath;
                if (QFileInfo::exists(localPath)) {
                    includePath = localPath;
                } else {
                    includePath = QString(PROJECT_SOURCE_DIR) + "/" + includeName;
                }
                output += Shader::preprocess(includePath);
            }
        } else {
            output += line + "\n";
        }
    }
    file.close();
    return output;
}
