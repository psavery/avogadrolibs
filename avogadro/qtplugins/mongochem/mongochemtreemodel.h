/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2019 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#ifndef AVOGADRO_QTPLUGINS_MONGOCHEMTREEMODEL_H
#define AVOGADRO_QTPLUGINS_MONGOCHEMTREEMODEL_H

#include <QAbstractTableModel>
#include <QScopedPointer>
#include <QList>
#include <QVariantMap>

namespace Avogadro {
namespace QtPlugins {

class MongoChemTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  MongoChemTreeModel(QObject* parent = nullptr);
  ~MongoChemTreeModel();
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;

  QModelIndex parent(const QModelIndex& child) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const override;

  QString moleculeId(int row);
  QString moleculeName(int row);
  void addMolecule(const QVariantMap& molecule);
  void deleteMolecule(const QModelIndex& index);
  void clear();

private:
  class TreeItem;

  TreeItem* getItem(const QModelIndex& index) const;

  QScopedPointer<TreeItem> m_rootItem;
};

} // namespace QtPlugins
} // namespace Avogadro
#endif
