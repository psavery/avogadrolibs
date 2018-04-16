/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2018 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include <QAction>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QString>

#include <avogadro/qtgui/molecule.h>

#include "banddialog.h"
#include "yaehmop.h"

using Avogadro::QtGui::Molecule;

namespace Avogadro {
namespace QtPlugins {

Yaehmop::Yaehmop(QObject* parent_)
  : Avogadro::QtGui::ExtensionPlugin(parent_), m_actions(QList<QAction*>()),
    m_molecule(nullptr),
    m_bandDialog(new BandDialog(qobject_cast<QWidget*>(parent()))),
    m_displayBandDialogAction(new QAction(this))

{
  m_displayBandDialogAction->setText(tr("Calculate band structure..."));
  connect(m_displayBandDialogAction.get(), &QAction::triggered, this,
          &Yaehmop::displayBandDialog);
  m_actions.push_back(m_displayBandDialogAction.get());
  m_displayBandDialogAction->setProperty("menu priority", 90);

  updateActions();
}

Yaehmop::~Yaehmop() = default;

QList<QAction*> Yaehmop::actions() const
{
  return m_actions;
}

QStringList Yaehmop::menuPath(QAction*) const
{
  return QStringList() << tr("&Extensions") << tr("&Yaehmop");
}

void Yaehmop::setMolecule(QtGui::Molecule* mol)
{
  if (m_molecule == mol)
    return;

  if (m_molecule)
    m_molecule->disconnect(this);

  m_molecule = mol;

  if (m_molecule)
    connect(m_molecule, SIGNAL(changed(uint)), SLOT(moleculeChanged(uint)));

  updateActions();
}

void Yaehmop::moleculeChanged(unsigned int c)
{
  Q_ASSERT(m_molecule == qobject_cast<Molecule*>(sender()));

  Molecule::MoleculeChanges changes = static_cast<Molecule::MoleculeChanges>(c);

  if (changes & Molecule::UnitCell) {
    if (changes & Molecule::Added || changes & Molecule::Removed)
      updateActions();
  }
}

void Yaehmop::updateActions()
{
  // Disable everything for nullptr molecules.
  if (!m_molecule) {
    foreach (QAction* action, m_actions)
      action->setEnabled(false);
    return;
  }

  // Only display the actions if there is a unit cell
  if (m_molecule->unitCell()) {
    foreach (QAction* action, m_actions)
      action->setEnabled(true);
  } else {
    foreach (QAction* action, m_actions)
      action->setEnabled(false);
  }
}

void Yaehmop::displayBandDialog()
{
  // Do nothing if the user cancels
  if (m_bandDialog->exec() != QDialog::Accepted)
    return;

  // Otherwise, fetch the options and perform the run
  unsigned long long numKPoints = m_bandDialog->numKPoints();

  // Perform the yaehmop calculation

  // Now generate a plot with the data
}

bool Yaehmop::executeYaehmop(const QStringList& args,
                                   const QByteArray& input, QByteArray& output,
                                   QString& err)
{
  QString program;
  // If the YAEHMOP_EXECUTABLE environment variable is set, then
  // use that
  QByteArray yaehmopExec = qgetenv("YAEHMOP_EXECUTABLE");
  if (!yaehmopExec.isEmpty()) {
    program = yaehmopExec;
  } else {
// Otherwise, search in the current directory, and then ../bin
#ifdef _WIN32
    QString executable = "yaehmop.exe";
#else
    QString executable = "yaehmop";
#endif
    QString path = QCoreApplication::applicationDirPath();
    if (QFile::exists(path + "/" + executable))
      program = path + "/" + executable;
    else if (QFile::exists(path + "/../bin/" + executable))
      program = path + "/../bin/" + executable;
    else {
      err = tr("Error: could not find yaehmop executable!");
      qDebug() << err;
      return false;
    }
  }

  QProcess p;
  p.start(program, args);

  if (!p.waitForStarted()) {
    err = tr("Error: " + program.toLocal8Bit() + " failed to start");
    qDebug() << err;
    return false;
  }

  // Give it the input!
  p.write(input.data());

  // Close the write channel
  p.closeWriteChannel();

  if (!p.waitForFinished()) {
    err = tr("Error: " + program.toLocal8Bit() + " failed to finish");
    qDebug() << err;
    output = p.readAll();
    qDebug() << "Output is as follows:\n" << output;
    return false;
  }

  int exitStatus = p.exitStatus();
  output = p.readAll();

  if (exitStatus == QProcess::CrashExit) {
    err = tr("Error: " + program.toLocal8Bit() + " crashed!");
    qDebug() << err;
    qDebug() << "Output is as follows:\n" << output;
    return false;
  }

  if (exitStatus != QProcess::NormalExit) {
    err = tr("Error: " + program.toLocal8Bit() +
             " finished abnormally with exit code " +
             QString::number(exitStatus).toLocal8Bit());
    qDebug() << err;
    qDebug() << "Output is as follows:\n" << output;
    return false;
  }

  // We did it!
  return true;
}

} // namespace QtPlugins
} // namespace Avogadro
