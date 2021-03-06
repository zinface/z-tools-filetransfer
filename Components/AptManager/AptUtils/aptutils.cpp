#include "aptutils.h"

#include <QDebug>
#include <QSysInfo>
#include <QTextStream>

AptUtils::AptUtils(QObject *parent)
{
    Q_UNUSED(parent);

    m_backend.init();
    m_backend.reloadCache();
    plist.append(m_backend.availablePackages());
}

int AptUtils::GetInstalledPackagesCount()
{
     return m_backend.installedCount();
}

int AptUtils::GetMirrorsPackagesCount()
{
    return m_backend.packageCount();
}

QApt::PackageList AptUtils::GetPackageList()
{
    return plist;
}


