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

#include "banddialog.h"
#include "ui_banddialog.h"

#include <QSettings>

namespace Avogadro {
namespace QtPlugins {

BandDialog::BandDialog(QWidget* aParent)
  : QDialog(aParent), m_ui(new Ui::BandDialog)
{
  m_ui->setupUi(this);

  // Read the settings
  QSettings settings;
  m_ui->spin_numKPoints->setValue(
    settings.value("yaehmop/bandoptions/numkpoints", 40).toUInt());
}

BandDialog::~BandDialog() = default;

unsigned long long BandDialog::numKPoints() const
{
  return m_ui->spin_numKPoints->value();
}

void BandDialog::accept()
{
  // Write the settings and accept
  QSettings settings;
  settings.setValue("yaehmop/bandoptions/numkpoints", numKPoints());

  QDialog::accept();
}

} // namespace QtPlugins
} // namespace Avogadro
